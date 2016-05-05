#ifndef REGEX_OBJECTPOOL_H
#define REGEX_OBJECTPOOL_H

#include <queue>
#include <vector>
#include <unistd.h>
#include <stdlib.h>

namespace RE
{
    using std::queue;
    using std::vector;

    template<class T>
    class CachedObject
    {
    public:
        CachedObject()
        {
            if(!hasRegDelPool)
            {
                hasRegDelPool = true;
                atexit( deletePool );
            }
        }

        void* operator new(size_t sz)
        {
            if( freeQueue_.empty() )
                allocChunk();
            T* obj = freeQueue_.front();
            freeQueue_.pop();
            return obj;
        }

        void operator delete(void* p, size_t sz)
        {
            if(!p) return;
            freeQueue_.push( static_cast<T*>(p) );
        }

        static void deletePool()
        {
            for(T* obj : pool_)
                ::operator delete(obj);
        }

    private:
        static bool hasRegDelPool;
        static vector<T*> pool_;
        static int chunkSize_;
        static queue<T*> freeQueue_;

        static void allocChunk()
        {
            for(int i=0; i < chunkSize_; i++)
            {
                T *p = static_cast<T*>( ::operator new( sizeof(T) ) );
                pool_.push_back(p);
                freeQueue_.push(p);
            }
        }

    }; // class CachedObject

    template<class T>
    bool CachedObject<T>::hasRegDelPool = false;

    template<class T>
    vector<T*> CachedObject<T>::pool_;

    template<class T>
    int CachedObject<T>::chunkSize_ = 20;

    template<class T>
    queue<T*> CachedObject<T>::freeQueue_;

} // namespace RE

#endif // REGEX_OBJECTPOOL_H