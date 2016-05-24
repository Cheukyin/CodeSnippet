#include <assert.h>

// Iterator must be random accessed,
// typeof(*Iterator) = T
// array must be sorted ascendingly

template<class Iterator, class T>
Iterator BinarySearch(Iterator begin, Iterator end, const T& val)
{
    Iterator end_ = end;

    // val in [begin, end)
    while (begin < end-1)
    {
        Iterator mid = begin + (end - begin) / 2;
        if (*mid == val) return mid;
        if (*mid < val) begin = mid + 1;
        else end = mid;
    }

    return *begin == val ? begin : end_;
}

template<class Iterator, class T>
Iterator BinarySearchFindFirst(Iterator begin, Iterator end, const T& val)
{
    Iterator end_ = end;

    begin--; end--;
    // val in (begin, end]
    while (begin < end - 1)
    {
        Iterator mid = begin + (end - begin) / 2;
        if (*mid == val) end = mid;
        else if (*mid < val) begin = mid;
        else end = mid - 1;
    }

    return *end == val ? end : end_;
}

template<class Iterator, class T>
Iterator BinarySearchFindLast(Iterator begin, Iterator end, const T& val)
{
    Iterator end_ = end;

    // val in [begin, end)
    while (begin < end - 1)
    {
        Iterator mid = begin + (end - begin) / 2;
        if (*mid == val) begin = mid;
        else if (*mid < val) begin = mid + 1;
        else end = mid;
    }

    return *begin == val ? begin : end_;
}

template<class Iterator, class T>
Iterator BinarySearchFindFirstGT(Iterator begin, Iterator end, const T& val)
{
    Iterator end_ = end;

    // val in [begin, end)
    while (begin < end - 1)
    {
        Iterator mid = begin + (end - begin) / 2;
        if (*mid <= val) begin = mid + 1;
        else if (*mid > val) end = mid + 1;
    }

    return *begin > val ? begin : end_;
}

template<class Iterator, class T>
Iterator BinarySearchFindFirstGE(Iterator begin, Iterator end, const T& val)
{
    Iterator end_ = end;

    begin--; end--;
    // the answer in (begin, end]
    while (begin < end - 1)
    {
        Iterator mid = begin + (end - begin) / 2;
        if (*mid >= val) end = mid;
        else if (*mid < val) begin = mid;
    }

    return *end >= val ? end : end_;
}

int main()
{
    int num[] = { 1, 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 6, 7, 7, 7, 9, 9, 11 };
    int *num_begin = num, *num_end = num + sizeof(num) / sizeof(int);

    assert(*(BinarySearch(num_begin, num_end, 4)) == 4);
    assert(*(BinarySearch(num_begin, num_end, 7)) == 7);
    assert(BinarySearch(num_begin, num_end, 0) == num_end);

    assert(BinarySearchFindFirst(num_begin, num_end, 2) == num_begin + 4);
    assert(BinarySearchFindFirst(num_begin, num_end, 4) == num_begin + 9);
    assert(BinarySearchFindFirst(num_begin, num_end, 0) == num_end);

    assert(BinarySearchFindLast(num_begin, num_end, 2) == num_begin + 5);
    assert(BinarySearchFindLast(num_begin, num_end, 4) == num_begin + 10);
    assert(BinarySearchFindLast(num_begin, num_end, 0) == num_end);

    assert(BinarySearchFindFirstGT(num_begin, num_end, 2) == num_begin + 6);
    assert(BinarySearchFindFirstGT(num_begin, num_end, 4) == num_begin + 11);
    assert(BinarySearchFindFirstGT(num_begin, num_end, 8) == num_begin + 18);
    assert(BinarySearchFindFirstGT(num_begin, num_end, 0) == num_begin);
    assert(BinarySearchFindFirstGT(num_begin, num_end, 1000) == num_end);

    assert(BinarySearchFindFirstGE(num_begin, num_end, 2) == num_begin + 4);
    assert(BinarySearchFindFirstGE(num_begin, num_end, 4) == num_begin + 9);
    assert(BinarySearchFindFirstGE(num_begin, num_end, 8) == num_begin + 18);
    assert(BinarySearchFindFirstGE(num_begin, num_end, 0) == num_begin);
    assert(BinarySearchFindFirstGE(num_begin, num_end, 1000) == num_end);

    return 0;
}