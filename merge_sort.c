int* merge_sort(int* arr, size_t left, size_t right) {
    if (left == right) {
        int* result = malloc(sizeof(int));
        if (result == NULL) exit(1);

        result[0] = arr[left];
        return result;
    }

    size_t pivot = (right - left) / 2;
    int* larr = merge_sort(arr, left, left + pivot);
    int* rarr = merge_sort(arr, left + pivot + 1, right);

    size_t llen = pivot + 1;
    size_t rlen = right - left - pivot;

    int* result = malloc((llen + rlen) * sizeof(int));
    if (result == NULL) exit(1);

    int i, li, ri;
    i = li = ri = 0;

    while (li < llen && ri < rlen) {
        if (larr[li] <= rarr[ri]) {
            result[i++] = larr[li++];
        } else {
            result[i++] = rarr[ri++];
        }
    }

    while (li < llen) result[i++] = larr[li++];
    while (ri < rlen) result[i++] = rarr[ri++];

    free(larr);
    free(rarr);
    return result;
}
