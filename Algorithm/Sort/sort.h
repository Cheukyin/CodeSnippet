#ifndef SORT_H_
#define SORT_H_

#include <alloca.h>
#include <assert.h>
#include <stddef.h>
#ifndef NDEBUG
#include <stdio.h>
#endif
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
  if a1 < a2, cmp_fn > 0
  if a1 = a2, cmp_fn = 0
  if a1 > a2, cmp_fn < 0
  Ascending
*/

#define _ARRAY_ELE_ADDR(arr, idx, size) ( (arr) + (idx) * (size) )

typedef int (* cmp_func_t)(const void *a1, const void *a2);

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  Insertion              Sort  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//righ shift the array with nmemb elements of size size
//arr[1:nmemb] = arr[0:nmemb-1]
static inline void _right_shift(void *arr, size_t nmemb, size_t size)
{
    char *from = _ARRAY_ELE_ADDR(arr, nmemb, size) - 1;
    char *to = from + size;
    while( from >= (char *)arr )
        *to-- = *from--;
}

//insert arr[ins_idx] into the sorted array arr[0:ins_idx-1]
//arr[0] must be less than arr[ins_idx]
static inline void _ungarded_insert_sorted_arr(void *arr, size_t ins_idx, size_t size, cmp_func_t cmp_fn)
{
    assert( cmp_fn(arr, _ARRAY_ELE_ADDR(arr, ins_idx, size)) >= 0 ); //arr[0] <= arr[ins_idx]

    void *ins_ele = alloca( size );
    memcpy(ins_ele, _ARRAY_ELE_ADDR(arr, ins_idx, size), size);

    void *iter = _ARRAY_ELE_ADDR(arr, ins_idx-1, size);
    while(1)
    {
        if( cmp_fn(iter, ins_ele) >= 0 ) //*iter <= *ins_ele
        {
            memcpy(iter+size, ins_ele, size);
            return;
        }

        memcpy(iter+size, iter, size);
        iter -= size;
    }
}

//insert arr[ins_idx] into the sorted array arr[0:ins_idx-1]
static inline void _insert_sorted_arr(void *arr, size_t ins_idx, size_t size, cmp_func_t cmp_fn)
{
    if( cmp_fn(arr, _ARRAY_ELE_ADDR(arr, ins_idx, size)) < 0 ) //arr[0] > arr[ins_idx]
    {
        void *ins_ele = alloca( size );
        memcpy(ins_ele, _ARRAY_ELE_ADDR(arr, ins_idx, size), size);

        _right_shift(arr, ins_idx, size);

        memcpy(arr, ins_ele, size);
        return;
    }

    _ungarded_insert_sorted_arr(arr, ins_idx, size, cmp_fn);
}

//sort arr with nmemb elements of size size using insert sort algorithm
static inline void insertion_sort(void *arr, size_t nmemb, size_t size, cmp_func_t cmp_fn)
{
    size_t i = 1;
    while(i < nmemb)
        _insert_sorted_arr(arr, i++, size, cmp_fn);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  Quick                  Sort  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//find the median of arr[lo], arr[mid] and arr[hi]
static inline size_t _qsort_medianOf3(void *arr, size_t lo, size_t mid, size_t hi, size_t size, cmp_func_t cmp_fn)
{
    void *lo_addr = _ARRAY_ELE_ADDR(arr, lo, size);
    void *mid_addr = _ARRAY_ELE_ADDR(arr, mid, size);
    void *hi_addr = _ARRAY_ELE_ADDR(arr, hi, size);

    if( cmp_fn(lo_addr, mid_addr) >= 0 )
    {
        if( cmp_fn(mid_addr, hi_addr) >=0 )
            return mid;
        else if( cmp_fn(lo_addr, hi_addr) >= 0 )
            return hi;
        else
            return lo;
    }
    else
    {
        if( cmp_fn(mid_addr, hi_addr) <= 0 )
            return mid;
        else if( cmp_fn(lo_addr, hi_addr) >= 0 )
            return lo;
        else
            return hi;
    }
}

//swap arr[e1] and arr[e2]
static inline void _swap_ele(void *arr, size_t e1, size_t e2, size_t size)
{
    void *tmp_ele = alloca( size );
    memcpy(tmp_ele, _ARRAY_ELE_ADDR(arr, e1, size), size);

    memcpy(_ARRAY_ELE_ADDR(arr, e1, size), _ARRAY_ELE_ADDR(arr, e2, size), size);
    memcpy(_ARRAY_ELE_ADDR(arr, e2, size), tmp_ele, size);
}

static inline void _qsort_3_way_partition(void *arr, size_t lo, size_t hi, size_t size, cmp_func_t cmp_fn, \
                                          size_t *less, size_t *greater)
{
    size_t pivot = _qsort_medianOf3(arr, lo, lo + (hi-lo)/2, hi, size, cmp_fn);

    _swap_ele(arr, pivot, lo, size);

    // |... less than ...|... equal ...|... not seen ...|... greater than ...|
    //                    ^             ^              ^
    //                    lt            eq             gt
    size_t lt = lo;
    size_t eq = lt + 1;
    size_t gt = hi;

    while( eq <= gt )
    {
        int cmp_eq_pivot = cmp_fn(_ARRAY_ELE_ADDR(arr, eq, size), _ARRAY_ELE_ADDR(arr, lt, size));
        if( !cmp_eq_pivot ) //arr[eq] == arr[lt]
            eq++;
        else if( cmp_eq_pivot < 0 ) //arr[eq] > arr[lt]
            _swap_ele(arr, eq, gt--, size);
        else //arr[eq] < pivot_ele
            _swap_ele(arr, lt++, eq++, size);
    }

    *less = lt;
    *greater = gt;
}

//[lo, hi]
static void _quick_sort(void *arr, size_t lo, size_t hi, size_t size, cmp_func_t cmp_fn)
{
    if( hi <= lo )
        return;

    size_t less, greater;
    _qsort_3_way_partition(arr, lo, hi, size, cmp_fn, &less, &greater);

    if( less > 0 ) //less is size_t which is unsigned, so it's necessary to judge if less==0
        _quick_sort(arr, lo, less-1, size, cmp_fn);
    _quick_sort(arr, greater+1, hi, size, cmp_fn);
}

static inline void quick_sort(void *arr, size_t nmemb, size_t size, cmp_func_t cmp_fn)
{
    _quick_sort(arr, 0, nmemb-1, size, cmp_fn);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  Heap                   Sort  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//the root of heap starts from 0
#define _HEAP_LEFT_CHIL(k) (2 * (k) + 1)
#define _HEAP_RIGHT_CHIL(k) (2 * (k) + 2)
#define _HEAP_PARENT(k) ( ((k) - 1) / 2 )

//assumed the caller has finished the following two steps before calling this function
// *ele = alloca( size );
// memcpy(ele, _ARRAY_ELE_ADDR(arr, root, size), size);
static inline void _sink_with_ele(void *arr, size_t nmemb, void *ele, size_t root, size_t size, cmp_func_t cmp_fn)
{
    while( _HEAP_LEFT_CHIL(root) < nmemb )
    {
        size_t greater_chil = _HEAP_LEFT_CHIL(root);
        //has right child and arr[left] < arr[right]
        if( greater_chil < nmemb-1 && \
            cmp_fn(_ARRAY_ELE_ADDR(arr, greater_chil, size), \
                   _ARRAY_ELE_ADDR(arr, greater_chil+1, size)) > 0 )
        greater_chil++; //less_chil is the right kid

        //ele > arr[greater_chil]
        if( cmp_fn(ele, _ARRAY_ELE_ADDR(arr, greater_chil, size)) < 0 )
            break;

        memcpy(_ARRAY_ELE_ADDR(arr, root, size), _ARRAY_ELE_ADDR(arr, greater_chil, size), size);
        /* _swap_ele(arr, root, greater_chil, size); */

        root = greater_chil;
    }

    memcpy(_ARRAY_ELE_ADDR(arr, root, size), ele, size);
}

static inline void _sink(void *arr, size_t nmemb, size_t root, size_t size, cmp_func_t cmp_fn)
{
    void *ele = alloca( size );
    memcpy(ele, _ARRAY_ELE_ADDR(arr, root, size), size);

    _sink_with_ele(arr, nmemb, ele, root, size, cmp_fn);
}

//min-heap or max-heap according to cmp_fn
static inline void _build_heap(void *arr, size_t nmemb, size_t size, cmp_func_t cmp_fn)
{
    if( nmemb == 1 ) return;

    //bottom-up
    size_t k = _HEAP_PARENT( nmemb-1 );
    while(1)
    {
        _sink(arr, nmemb, k, size, cmp_fn);
        if( !k ) break;
        k--;
    }
}

static inline void heap_sort(void *arr, size_t nmemb, size_t size, cmp_func_t cmp_fn)
{
    _build_heap(arr, 11, sizeof(int), cmp_fn);

    while( nmemb > 1 )
    {
        //ele = arr[nmemb-1]
        void *ele = alloca( size );
        memcpy(ele, _ARRAY_ELE_ADDR(arr, nmemb-1, size), size);

        //arr[nmemb-1] = arr[0]
        memcpy(_ARRAY_ELE_ADDR(arr, nmemb-1, size), arr, size);

        //adjust the heap
        _sink_with_ele(arr, --nmemb, ele, 0, size, cmp_fn);

        //the codes above is equivalent to the following two lines
        /* _swap_ele(arr, 0, nmemb-1, size); */
        /* _sink(arr, --nmemb, 0, size, cmp_fn); */
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  Introspective          Sort  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _INTRO_LEN_THRESHOLD 16
#define _DEPTH_LIMIT(k) (_lg( (k) ) * 2)
/* #define _DEPTH_LIMIT(k) (2) */

//equivalent to finding the leftmost '1' bit position
static inline size_t _lg(size_t n)
{
    size_t k = 0;
    for(; n > 1; n >>= 1)
        k++;

    return k;
}

//[lo, hi]
static void _intro_qsort(void *arr, size_t lo, size_t hi, size_t size, cmp_func_t cmp_fn, size_t recur_depth)
{
    //if array's len <= _INTRO_LEN_THRESHOLD, don't sort
    if( hi-lo+1 <= _INTRO_LEN_THRESHOLD )
        return;

    #ifndef NDEBUG
    printf("recursion depth: %ld\n", recur_depth);
    #endif

    //when recursion is too deep, use heap sort
    if( !recur_depth )
    {
        #ifndef NDEBUG
        printf("heap sort: lo: %ld, hi: %ld\n", lo, hi);
        #endif
        heap_sort(arr + size*lo, hi-lo+1, size, cmp_fn);
        return;
    }

    recur_depth--;

    //use qsort
    size_t less, greater;
    _qsort_3_way_partition(arr, lo, hi, size, cmp_fn, &less, &greater);

    if( less > 0 ) //less is size_t which is unsigned, so it's necessary to judge if less==0
        _intro_qsort(arr, lo, less-1, size, cmp_fn, recur_depth);
    _intro_qsort(arr, greater+1, hi, size, cmp_fn, recur_depth);
}

static inline void _intro_insert_sort(void *arr, size_t nmemb, size_t size, cmp_func_t cmp_fn)
{
    #ifndef NDEBUG
    printf("Now the insertion sort.\n");
    #endif

    if( nmemb > _INTRO_LEN_THRESHOLD )
    {
        insertion_sort(arr, _INTRO_LEN_THRESHOLD, size, cmp_fn);
        for(size_t i = _INTRO_LEN_THRESHOLD; i < nmemb; i++)
            _ungarded_insert_sorted_arr(arr, i, size, cmp_fn);
    }
    else
        insertion_sort(arr, nmemb, size, cmp_fn);
}

static inline void intro_sort(void *arr, size_t nmemb, size_t size, cmp_func_t cmp_fn)
{
    if( nmemb == 1 ) return;

    //mix qsort and heapsort
    _intro_qsort(arr, 0, nmemb-1, size, cmp_fn, _DEPTH_LIMIT(nmemb));
    //insertion sort
    _intro_insert_sort(arr, nmemb, size, cmp_fn);
}

#ifdef __cplusplus
extern "C"
}
#endif

#endif //SORT_H_