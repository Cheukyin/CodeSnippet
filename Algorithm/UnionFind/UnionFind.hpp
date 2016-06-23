#ifndef __CYTL__UNIONFIND__
#define __CYTL__UNIONFIND__

#include <cstddef>

namespace CYTL
{
    class UnionFind
    {
    public:
        UnionFind(int n)
            : size_(n),
              parent_(new std::size_t[size_]),
              weight_(new std::size_t[size_])
        {
            for(std::size_t i = 0; i < size_; i++)
            {
                parent_[i] = i;
                weight_[i] = 1;
            }
        }

        bool isConnected(std::size_t p, std::size_t q)
        { return root(p) == root(q); }

        void connect(std::size_t p, std::size_t q)
        {
            size_t rootp = root(p);
            size_t rootq = root(q);
            if(rootp == rootq) return;

            if(weight_[rootp] > weight_[rootq])
            {
                weight_[rootp] += weight_[rootq];
                parent_[rootq] = rootp;
            }
            else
            {
                weight_[rootq] += weight_[rootp];
                parent_[rootp] = rootq;
            }
        }

        ~UnionFind()
        { delete parent_; }

    private:
        std::size_t size_;
        std::size_t* parent_;
        std::size_t* weight_;

        std::size_t root(std::size_t i)
        {
            if(parent_[i] != i) parent_[i] = root(parent_[i]);
            return parent_[i];
        }
    };

}

#endif // __CYTL__UNIONFIND__