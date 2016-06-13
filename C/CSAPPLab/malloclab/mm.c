/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Archepelogo",
    /* First member's full name */
    "Cheukyin Yip",
    /* First member's email address */
    "cheukyinyip0077@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/*
  block:
           | HEADER | prev-free-blk | next-free-blk | ... | FOOTER |
                    | <--------------- CONTENT ------------------> |
           The address of content will be aligned to 8 bytes,
           so the whole size of the blcok must be aligned to 8 bytes,
           then the size of content will be 8k+4 bytes
           therefore the low 2 bits of content size will be unused
           prev-free-blk && next-free-blk && FOOTER are only used in free list,
           so they can be used by the user

  HEADER:
          |      31 ..... 2   |                1                    |                0                    |
          |       size =      | a(a=1: previous block is allocated; | b(b=1: current block is allocated;  |
          | whole block size  |   a=0: previous block is free)      |   b=0: current block is free)       |
          |         -         |                                     |                                     |
          |    HEADER_SIZE    |                                     |                                     |

  FOOTER's structure is the same as HEADER,
  except that there's no nedd for a.
*/

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;

// type of header and footer
typedef uint32 info_t;


typedef struct block{ // starts at address aligned by 8 bytes
    info_t       header;
    struct block *prev_free_blk;
    struct block *next_free_blk;
    // the last 4 bytes will be reused by the next block to store the content size(the 4 bytes are not included)
    char         content[4];
} __attribute__((packed)) block_t;


#define WORD_SIZE 4 // single word size
#define DWORD_SIZE (2 * (WORD_SIZE)) // double word size

// Size of FOOTER and prev_free_blk and next_free_blk
#define OVERHEAD (2 * sizeof(block_t *) + 4)


/* single word (4) or double word (8) alignment */
#define ALIGNMENT DWORD_SIZE

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define CONTENT_SIZE_MASK ( ~(ALIGNMENT - 1) ) // ~0b0000 0111
#define CUR_ALLOC_MASK (0x1)
#define PREV_ALLOC_MASK (0x2)
#define ALLOC_MASK (PREV_ALLOC_MASK | CUR_ALLOC_MASK)

#define CHUNK_SIZE ( ALIGN(1<<12) ) // 4k bytes

#define MAX(a, b) ( ((a) > (b)) ? (a) : (b) )

#define GET_BLK_CONTENT_SIZE(blk) ((blk)->header & CONTENT_SIZE_MASK)
#define GET_BLK_FOOTER(blk) \
    ( *(info_t *)((blk)->content + GET_BLK_CONTENT_SIZE(blk)) )
#define SET_BLK_FOOTER(blk, content_size) \
    ( *(info_t *)((blk)->content + GET_BLK_CONTENT_SIZE(blk)) = (content_size) )

#define USR_OFFSET sizeof(info_t)

#define CONTENT_SIZE_2_REAL_SIZE(content_size) ((content_size) + OVERHEAD)
#define REAL_SIZE_2_CONTENT_SIZE(real_size) ((real_size) - OVERHEAD)
#define GET_BLK_REAL_SIZE(blk) ( CONTENT_SIZE_2_REAL_SIZE( GET_BLK_CONTENT_SIZE(blk) ) )
#define BLK2USR(blk) ( (char *)&( (blk)->prev_free_blk ) )
#define USR2BLK(usr) ((block_t *)((char *)(usr) - USR_OFFSET))

#define IS_BLK_ALLOC(blk) ((blk)->header & CUR_ALLOC_MASK)
#define IS_PREV_BLK_ALLOC(blk) ((blk)->header & PREV_ALLOC_MASK)
#define GET_BLK_ALLOC_FLAG(blk) ((blk)->header & ALLOC_MASK)

#define SET_CUR_ALLOC_FLAG(blk) ((blk)->header |= CUR_ALLOC_MASK)
#define SET_PREV_ALLOC_FLAG(blk) ((blk)->header |= PREV_ALLOC_MASK)

// only used when physically previous block of blk is free
#define GET_PREV_PHY_BLK_SIZE(blk) (*(info_t *)((char *)blk - sizeof(info_t)))
#define LOCATE_PREV_PHY_BLK(blk) \
    ((block_t *)((char *)blk - GET_PREV_PHY_BLK_SIZE(blk) - sizeof(block_t)))
#define LOCATE_NEXT_PHY_BLK(blk) \
    ((block_t *)((blk)->content + GET_BLK_CONTENT_SIZE(blk) + sizeof(info_t)))

static block_t *free_list_hdr; // the header of free_list, starts at the lowest adrress of head
static block_t *last_phy_blk;

static inline void insert_as_head(block_t *free_list_hdr, block_t *blk)
{
    printf("insert_as_head:\n");

    // insert the new block into the free_list_hdr
    block_t *first_blk = free_list_hdr->next_free_blk;
    free_list_hdr->next_free_blk = blk;
    blk->next_free_blk = first_blk;
    blk->prev_free_blk = free_list_hdr;
    if( first_blk ) // first_blk is not NULL
        first_blk->prev_free_blk = blk;
}

// cp must not be NULL
static inline void delete_blk_from_free_list(block_t *blk)
{
    assert( blk );
    block_t *prev = blk->prev_free_blk;
    block_t *next = blk->next_free_blk;
    prev->next_free_blk = next;
    if( next )
        next->prev_free_blk = prev;
}

// cut out content_size from the free block pointed by cp
// size of the block must be larger than content_size
static inline block_t *split_free_blk(block_t *bp, size_t content_size)
{
    printf("split_free_blk:\n");

    block_t *prev = bp->prev_free_blk;
    block_t *next = bp->next_free_blk;
    size_t original_content_size = GET_BLK_CONTENT_SIZE(bp);

    // cut out, set allocated flag
    bp->header = content_size | CUR_ALLOC_MASK | IS_PREV_BLK_ALLOC(bp);
    SET_BLK_FOOTER(bp, content_size);

    // reconstruct the remaining block
    block_t *new_bp = LOCATE_NEXT_PHY_BLK(bp);

    size_t remain_blk_size = original_content_size - content_size;
    size_t remain_content_size = remain_blk_size - sizeof(block_t);
    new_bp->header = remain_content_size | PREV_ALLOC_MASK;
    SET_BLK_FOOTER(new_bp, remain_content_size);

    new_bp->prev_free_blk = prev;
    new_bp->next_free_blk = next;
    prev->next_free_blk = new_bp;
    if(next)
        next->prev_free_blk = new_bp;

    if(bp == last_phy_blk)
        last_phy_blk = new_bp;

    return bp;
}

// search for the first free block whose content_size >= content_sizes
// content_size must have been aligned to 8 bytes
// when the block is found, split it and return the resulted block
// if nothing is found, return NULL
static inline block_t *find_first_fit(block_t *free_list_hdr, size_t content_size)
{
    printf("find_first_fit:\n");

    block_t *traveller = free_list_hdr->next_free_blk;
    while( traveller )
    {
        size_t traveller_content_size = GET_BLK_CONTENT_SIZE( traveller );

        if( traveller_content_size == content_size )
        {
            delete_blk_from_free_list( traveller );
            traveller->header |= CUR_ALLOC_MASK;
            if(traveller != last_phy_blk)
                LOCATE_NEXT_PHY_BLK(traveller)->header |= PREV_ALLOC_MASK;

            return traveller;
        }

        if( traveller_content_size > sizeof(block_t) )
        {
            assert( (traveller_content_size - content_size) % 8 == 0 );
            return split_free_blk(traveller, content_size);
        }

        traveller = traveller->next_free_blk;
    }

    return NULL;
}

// allocate new extended block and insert into the free list
// content_size has bben aligned to 8 bytes
static inline block_t *extend_free_list(block_t *free_list_hdr, size_t content_size)
{
    printf("extend_free_list:\n");

    assert( content_size % 8 == 0 );

    size_t blk_size = content_size + sizeof(block_t);

    block_t *new_blk;
    if( (new_blk = mem_sbrk(blk_size)) < 0 )
    {
        fprintf(stderr, "sbrk error!");
        exit(1);
    }

<<<<<<< HEAD
    // last_phy_blk is free
    // then coalesce
    if( !IS_BLK_ALLOC( last_phy_blk ) )
=======
    // cp points to content
    ptr_t cp = (ptr_t)( (char *)bp + HEADER_SIZE );

    // if not firstly initialization and the previous block is free
    // coalesce previous free block
    if( !IS_CUR_ALLOCATED( last_content_in_heap ) )
>>>>>>> 56ade23afe06509ee60b8f78b3308289d3694461
    {
        size_t coalesced_blk_size = blk_size + GET_BLK_CONTENT_SIZE( last_phy_blk );
        assert( coalesced_blk_size % 8 == 0 );

        last_phy_blk->header = coalesced_blk_size;
        SET_BLK_FOOTER(last_phy_blk, coalesced_blk_size);

        assert( *(info_t *)(new_blk->content + content_size) == coalesced_blk_size );

        return last_phy_blk;
    }

    // last_phy_blk is allocated
    new_blk->header = content_size | PREV_ALLOC_MASK;
    SET_BLK_FOOTER(new_blk, content_size);
    last_phy_blk = new_blk;

    insert_as_head(free_list_hdr, new_blk);

    return new_blk;
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    assert( sizeof(block_t) % 8 == 0 );

    char *padding = mem_sbrk( sizeof(info_t) ); // alignment padding
    *padding = 0;
    assert( (int)padding % 8 == 0 );

<<<<<<< HEAD
    block_t *free_list_hdr = mem_sbrk( sizeof(block_t) );
    assert( (int)&(free_list_hdr->prev_free_blk) % 8 == 0 );
=======
    last_content_in_heap = free_list;
>>>>>>> 56ade23afe06509ee60b8f78b3308289d3694461

    free_list_hdr->header = 0 | CUR_ALLOC_MASK | PREV_ALLOC_MASK;
    free_list_hdr->prev_free_blk = free_list_hdr->next_free_blk = NULL;
    SET_BLK_FOOTER(free_list_hdr, 0);

    last_phy_blk = free_list_hdr;

    extend_free_list(free_list_hdr, CHUNK_SIZE); // initially allocate 4k bytes free block

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t usr_size)
{
    static int malloc_times = 0;
    printf("\n\n\nmalloc times: %d\n", malloc_times++);

    size_t content_size = 0;
    if(usr_size > sizeof(block_t))
        content_size = ALIGN( usr_size - OVERHEAD );

    block_t *bp = find_first_fit(free_list_hdr, content_size);

<<<<<<< HEAD
    // no fit block, extend free list
    while( !bp )
    {
        bp = extend_free_list(free_list_hdr, MAX(CHUNK_SIZE, content_size));
        assert(GET_BLK_REAL_SIZE(bp) >= usr_size);
        assert(GET_BLK_CONTENT_SIZE(bp) % 8 == 0);

        if(GET_BLK_CONTENT_SIZE(bp) > content_size)
            bp = split_free_blk(bp, content_size);
    }

    /* printf("malloc cp: %p ~~~~~~~~~~\n", cp); */
    return BLK2USR(bp);
=======
    ptr_t cp = find_first_fit(free_list, size);

    // no fit block, extend free list
    while( !cp )
    {
        extend_free_list(free_list, MAX(CHUNK_SIZE, size));
        cp = find_first_fit(free_list, size);
    }

    /* printf("malloc cp: %p ~~~~~~~~~~\n", cp); */
    return cp;
>>>>>>> 56ade23afe06509ee60b8f78b3308289d3694461
}

//bp must hanve been set free flag
//bp must not be in the free list
// coalesce bp and insert into free list
static inline void coalesce(block_t *free_list_hdr, block_t *bp)
{
    printf("coalesce:\n");

    char is_prev_alloc = IS_PREV_BLK_ALLOC( bp );
    char is_next_alloc = CUR_ALLOC_MASK;
    if( bp != last_phy_blk )
        is_next_alloc = IS_BLK_ALLOC( LOCATE_NEXT_PHY_BLK(bp) );

    if( is_prev_alloc && is_next_alloc )
    {
        insert_as_head(free_list_hdr, bp);
        return;
    }

    if( !is_prev_alloc && is_next_alloc )
    {
        block_t *prev = LOCATE_PREV_PHY_BLK(bp);

        if( last_phy_blk == bp )
            last_phy_blk = prev;

        size_t coalesced_size = GET_BLK_CONTENT_SIZE(prev) + GET_BLK_CONTENT_SIZE(bp) + sizeof(block_t);
        assert( coalesced_size % 8 == 0 );
        prev->header = coalesced_size | GET_BLK_ALLOC_FLAG(prev);
        SET_BLK_FOOTER(prev, coalesced_size);
        assert( GET_BLK_FOOTER(bp) == coalesced_size );

        return;
    }

    if( is_prev_alloc && !is_next_alloc )
    {
        block_t *next = LOCATE_NEXT_PHY_BLK(bp);

        if( last_phy_blk == next )
            last_phy_blk = bp;

<<<<<<< HEAD
        block_t *prev_free_blk = next->prev_free_blk;
        block_t *next_free_blk = next->next_free_blk;
        bp->prev_free_blk = prev_free_blk;
        bp->next_free_blk = next_free_blk;
        prev_free_blk->next_free_blk = bp;
        if( next_free_blk )
            next_free_blk->prev_free_blk = bp;

        size_t coalesced_size = GET_BLK_CONTENT_SIZE(bp) + GET_BLK_CONTENT_SIZE(next) + sizeof(block_t);
        assert( coalesced_size % 8 == 0 );
        bp->header = coalesced_size | PREV_ALLOC_MASK;
        SET_BLK_FOOTER(bp, coalesced_size);
        assert( GET_BLK_FOOTER(next) == coalesced_size );
=======
        delete_node(next_cp);

        size_t size = GET_CUR_SIZE(cp) + GET_NEXT_SIZE(cp) + HEADER_SIZE;
        assert( (size + HEADER_SIZE) % 8 == 0 );
        SET_CUR_HEADER_SIZE(cp, size);
        SET_CUR_FOOTER_SIZE(cp, size);
>>>>>>> 56ade23afe06509ee60b8f78b3308289d3694461

        insert_as_head(free_list, cp);

        return;
    }

    if( !is_prev_alloc && !is_next_alloc )
    {
        block_t *prev = LOCATE_PREV_PHY_BLK(bp);
        block_t *next = LOCATE_NEXT_PHY_BLK(bp);

        if( last_phy_blk == next )
            last_phy_blk = prev;

<<<<<<< HEAD
        delete_blk_from_free_list(next);

        size_t coalesced_size = GET_BLK_CONTENT_SIZE(prev)
                                + GET_BLK_CONTENT_SIZE(bp)
                                + GET_BLK_CONTENT_SIZE(next)
                                + 2*sizeof(block_t);
        assert( coalesced_size % 8 == 0 );
        prev->header = coalesced_size | GET_BLK_ALLOC_FLAG(prev);
        SET_BLK_FOOTER(prev, coalesced_size);
        assert( GET_BLK_FOOTER(next) == coalesced_size );
=======
        delete_node(next_cp);

        size_t size = GET_PREV_SIZE(cp) + GET_CUR_SIZE(cp) + GET_NEXT_SIZE(cp) + 2*HEADER_SIZE;
        assert( (size + HEADER_SIZE) % 8 == 0 );
>>>>>>> 56ade23afe06509ee60b8f78b3308289d3694461

        return;
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    static int free_times = 0;
    printf("\n\n\nfree times: %d\n", free_times++);

    block_t *bp = USR2BLK(ptr);
    bp->header &= ~CUR_ALLOC_MASK;
    if(last_phy_blk != bp)
        bp->header &= ~PREV_ALLOC_MASK;

<<<<<<< HEAD
    coalesce(free_list_hdr, bp);
=======
    SET_CUR_FOOTER_SIZE(ptr, GET_CUR_SIZE(ptr));

    coalesce(free_list, ptr);
>>>>>>> 56ade23afe06509ee60b8f78b3308289d3694461
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    /* void *oldptr = ptr; */
    /* void *newptr; */
    /* size_t copySize; */

    /* newptr = mm_malloc(size); */
    /* if (newptr == NULL) */
    /*   return NULL; */
    /* copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE); */
    /* if (size < copySize) */
    /*   copySize = size; */
    /* memcpy(newptr, oldptr, copySize); */
    /* mm_free(oldptr); */
    /* return newptr; */
}