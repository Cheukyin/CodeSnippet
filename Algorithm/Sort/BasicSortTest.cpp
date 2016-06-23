#include "BasicSort.hpp"
#include <assert.h>
#include <algorithm>
#include <vector>
#include <iostream>

int main()
{
    // ---------------------------------------
    // insertion sort
    std::vector<int> vec{5,3,4,6,7,5,7,5,78};
    std::vector<int> vecbench(vec);

    CYTL::insertionSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5,5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::insertionSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);


    // ---------------------------------------
    // selection sort
    vec = {55,3,4,6,7,5,7,5,78};
    vecbench = vec;

    CYTL::selectionSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5,5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::selectionSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);


    // ---------------------------------------
    // shell sort
    vec = {55,3,4,6,7,5,7,5,78};
    vecbench = vec;

    CYTL::shellSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5, 5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::shellSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);


    // ---------------------------------------
    // quick sort
    vec = {55,3,4,6,7,5,7,5,78};
    vecbench = vec;

    CYTL::quickSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5, 5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::quickSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {1, 2, 3, 4, 5, 6};
    vecbench = vec;

    CYTL::quickSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {6, 5, 4, 3, 2, 1};
    vecbench = vec;

    CYTL::quickSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);


    // ---------------------------------------
    // recursive merge sort
    vec = {1, 2, 3, 4, 5, 6};
    vecbench = vec;

    CYTL::recursiveMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5, 5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::recursiveMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {6, 5, 4, 3, 2, 1};
    vecbench = vec;

    CYTL::recursiveMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {55,3,4,6,7,5,7,5,78};
    vecbench = vec;

    CYTL::recursiveMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);


    // ---------------------------------------
    // iterative merge sort
    vec = {1, 2, 3, 4, 5, 6};
    vecbench = vec;

    CYTL::iterativeMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {1};
    vecbench = vec;

    CYTL::iterativeMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {1, 2};
    vecbench = vec;

    CYTL::iterativeMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5, 5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::iterativeMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {6, 5, 4, 3, 2, 1};
    vecbench = vec;

    CYTL::iterativeMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {55,3,4,6,7,5,7,5,78};
    vecbench = vec;

    CYTL::iterativeMergeSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);


    return 0;
}