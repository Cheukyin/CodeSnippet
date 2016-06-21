#ifndef __CYTL__LINEARPROBINGHASHMAP__
#define __CYTL__LINEARPROBINGHASHMAP__

#include "HashCode.hpp"
#include <stdlib.h>
#include <assert.h>

namespace CYTL
{
    template<class KeyType, class ValueType>
    class LinearProbingHashMap
    {
    private:
        enum NodeStatus{AVAILABLE, FAKE, NONAVAILABLE};

        struct Node
        {
            NodeStatus st;
            KeyType k;
            ValueType v;

            Node(): st(NONAVAILABLE) {}
        };

        int hash(const KeyType& key) const
        {
            int hashcode =  HashCode<KeyType>()(key);
            return (hashcode & 0x7fffffff) % bucketSize;
        }

        int findNodeIdByKey(int startId, const KeyType& key)
        {
            for(int i = 0; i < bucketSize; i++)
            {
                if(buckets[startId].st == NONAVAILABLE) return -1;
                if(buckets[startId].st == FAKE) continue;

                if(buckets[startId].k == key) return startId;

                if(startId >= bucketSize-1) startId = 0;
                else startId++;
            }
            return -1;
        }

        int findNotAvailableNodeId(int startId)
        {
            for(int i = 0; i < bucketSize; i++)
            {
                if(buckets[startId].st == NONAVAILABLE) return startId;
                if(buckets[startId].st == FAKE) return startId;

                if(startId >= bucketSize-1) startId = 0;
                else startId++;
            }
            return -1;
        }

        void resize(int newBucketSize)
        {
            Node* old = buckets;
            int oldSize = bucketSize;
            int oldusecount = usecount;

            buckets = new Node[newBucketSize];
            bucketSize = newBucketSize;

            for(int i = 0; i < oldSize; i++)
            {
                if(old[i].st == AVAILABLE)
                    operator[]( old[i].k ) = old[i].v;
            }

            delete [] old;
            usecount = oldusecount;
        }

        int bucketSize;
        int usecount;
        Node* buckets;

        const double threshold;

    public:
        LinearProbingHashMap()
            : bucketSize(3), usecount(0), buckets(new Node[bucketSize]), threshold(0.75)
        {}

        LinearProbingHashMap(int size)
            : bucketSize(size), usecount(0), buckets(new Node[bucketSize]), threshold(0.75)
        {}

        ValueType& operator[](const KeyType& key)
        {
            int hashcode = hash(key);
            int idx = findNodeIdByKey(hashcode, key);
            if(idx >= 0) return buckets[idx].v;

            double loadfactor = (double)(usecount + 1) / bucketSize;
            if(loadfactor >= threshold) resize(2 * bucketSize + 1);

            hashcode = hash(key);
            idx = findNotAvailableNodeId(hashcode);
            assert(idx >= 0);

            buckets[idx].st = AVAILABLE;
            buckets[idx].k = key;
            usecount++;

            return buckets[idx].v;
        }

        bool isKeyExist(const KeyType& key)
        {
            int idx = hash(key);
            return findNodeIdByKey(idx, key) >= 0;
        }

        void eraseKey(const KeyType& key)
        {
            int hashcode = hash(key);
            int idx = findNodeIdByKey(hashcode, key);
            if(idx < 0) return;

            buckets[idx].st = FAKE;
            usecount--;

            if((double)usecount <= (double)bucketSize / 4)
            {
                int newSize = bucketSize / 2;
                resize(newSize % 2 ? newSize : newSize+1);
            }
        }

        ~LinearProbingHashMap()
        { delete [] buckets; }

    };
}

#endif // __CYTL__LINEARPROBINGHASHMAP__