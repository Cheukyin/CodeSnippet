#include "sort.h"
#include <stdio.h>

int cmp(const void *a1, const void *a2)
{
    return  (*(int *)a2) - (*(int *)a1);
}

int main()
{
    int a[84] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                 43, 23, 45, 64, 34, 21, 34, 34, 6, 1,
                 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                 43, 23, 45, 64, 34, 21, 34, 34, 6, 1,
                 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                 43, 23, 45, 64, 34, 21, 34, 34, 6, 1,
                 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
                 43, 23, 45, 64, 34, 21, 34, 34, 6, 1};
    /* int a[14] = {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}; */

    /* printf("median: %d", a[_qsort_medianOf3(a, 2, 0, 1, sizeof(int), cmp)]); */
    /* _swap_ele(a, 2, 3, sizeof(int)); */
    /* size_t less, greater; */
    /* _qsort_3_way_partition(a, 0, 10, sizeof(int), cmp, &less, &greater); */
    /* printf("%d, %d\n", less, greater); */

    /* quick_sort(a, 11, sizeof(int), cmp); */
    /* _build_heap(a, 11, sizeof(int), cmp); */
    /* heap_sort(a, 11, sizeof(int), cmp); */
    intro_sort(a, 84, sizeof(int), cmp);

    printf("\n");

    /* insertion_sort(a, 11, sizeof(int), cmp); */
    for(int i=0; i < sizeof(a)/sizeof(int); i++)
        printf("%d ", a[i]);
}