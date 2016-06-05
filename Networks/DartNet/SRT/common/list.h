#include <assert.h>
#include <stdlib.h>

static node_t *tcbUsedList;
static node_t *tcbFreeList;

static inline void inserIntoList(node_t *lh, node_t *newNode)
{
    assert(lh); assert(newNode);

    node_t *tcbNode = lh->next;

    lh->next = newNode;
    newNode->next = tcbNode;
    newNode->prev = lh;
    if(tcbNode) tcbNode->prev = newNode;
}

static inline void allocTCB()
{
    for(int i=0; i < 10; i++)
    {
        node_t *newNode = (node_t*)malloc( sizeof(node_t) );
        if(!newNode) return;
        newNode->isFree = 1;
        inserIntoList(tcbFreeList, newNode);
    }
}

static inline void removeFromList(node_t *node)
{
    assert(node);

    node->prev->next = node->next;
    if(node->next)
        node->next->prev = node->prev;
}

static inline node_t* takeFromListHead(node_t *lh)
{
    if(lh == tcbFreeList && !lh->next) allocTCB();
    if(!lh->next) return NULL;

    node_t *node = lh->next;
    removeFromList(node);
    return node;
}

static inline tcb_t* sock2TCB(sock_t sock)
{
    return sock;
}










