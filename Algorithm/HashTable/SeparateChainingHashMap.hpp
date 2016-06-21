#ifndef __CYTL__SEPARATECHAININGHASHMAP__
#define __CYTL__SEPARATECHAININGHASHMAP__

#include "HashCode.hpp"
#include <stdlib.h>

namespace CYTL
{
    template<class KeyType, class ValueType>
    class SeparateChainingHashMap
    {
    private:
        struct Node
        {
            KeyType k;
            ValueType v;
            Node* next;

            Node(): next(NULL) {}

            Node(const KeyType& k): k(k), next(NULL) {}

            Node(const KeyType& k, const ValueType& v)
                : k(k), v(v), next(NULL)
            {}
        };

        Node* findNodeByKey(Node* head, const KeyType& k)
        {
            for(Node* n = head->next; n; n = n->next)
            { if(n->k == k) return n; }
            return NULL;
        }

        void removeNodeByKey(Node* head, const KeyType& k)
        {
            for(Node* prev = head, *n = head->next; n; prev = prev->next, n = n->next)
            {
                if( !(n->k == k) ) continue;

                prev->next = n->next;
                delete n;
                return;
            }
        }

        int hash(const KeyType& key) const
        {
            int hashcode =  HashCode<KeyType>()(key);
            return (hashcode & 0x7fffffff) % bucketSize;
        }

        int bucketSize;
        Node* buckets;

    public:
        SeparateChainingHashMap()
            : bucketSize(97), buckets(new Node[bucketSize])
        {}

        ValueType& operator[](const KeyType& key)
        {
            Node* head = &buckets[hash(key)];
            Node* n = findNodeByKey(head, key);

            if(n) return n->v;

            n = new Node(key);
            n->next = head->next;
            head->next = n;
            return n->v;
        }

        bool isKeyExist(const KeyType& key)
        {
            Node* head = &buckets[hash(key)];
            return findNodeByKey(head, key);
        }

        void eraseKey(const KeyType& key)
        {
            Node* head = &buckets[hash(key)];
            removeNodeByKey(head, key);
        }

        ~SeparateChainingHashMap()
        {
            for(int i = 0; i < bucketSize; i++)
            {
                for(Node* n = buckets[i].next; n; n = n->next)
                    delete n;
            }
            delete [] buckets;
        }

    };
}

#endif // __CYTL__SEPARATECHAININGHASHMAP__