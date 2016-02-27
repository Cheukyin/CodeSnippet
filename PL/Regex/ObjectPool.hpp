#ifndef REGEX_OBJECTPOOL_H
#define REGEX_OBJECTPOOL_H

#include "NFA.hpp"
#include <queue>
#include <vector>

namespace RE
{
    using std::queue;

    template<class T>
    class ObjectPool
    {
    public:
        ObjectPool(): chunkSize_(20)
        {}

        T* getObject()
        {
            if( freeQueue_.empty() )
                allocChunk();
            T* obj = freeQueue_.front();
            freeQueue_.pop();
            return obj;
        }

        void releaseObject(T* obj)
        { freeQueue_.push(obj); }

        ~ObjectPool()
        {
            for(T* obj : pool_)
                delete [] obj;
        }

        static ObjectPool<T>* getPool()
        {
            static ObjectPool<T>* pool = new ObjectPool<T>();
            return pool;
        }

    private:
        vector<T*> pool_;
        int chunkSize_;
        queue<T*> freeQueue_;

        void allocChunk(int num)
        {
            NodePtr *p = new NodePtr[chunkSize_];
            pool_.push_back(p);

            for(int i=0; i < chunkSize_; i++)
                freeQueue_.push(p++);
        }

    }; // class ObjectPool

    using NodePool = ObjectPool<Node>;

} // namespace RE

#endif // REGEX_OBJECTPOOL_H