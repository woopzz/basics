#include <stdbool.h>

typedef int compare_fn_t (void* a, void* b);

typedef struct {
    void** entries;
    size_t size;
    size_t capacity;
    compare_fn_t* compare_fn;
} Heap;

typedef struct {
    bool ok;
    void* value;
} MaybeHeapValue;

void heap_init(Heap* heap, compare_fn_t* compare_fn) {
    heap->entries = NULL;
    heap->size = 0;
    heap->capacity = 0;
    heap->compare_fn = compare_fn;
}

void heap_free(Heap* heap) {
    if (heap->entries != NULL) free(heap->entries);
}

bool heap_resize(Heap* heap, size_t new_capacity) {
    void** entries;
    if ((entries = realloc(heap->entries, new_capacity * sizeof(void*))) == NULL) return false;

    heap->entries = entries;
    heap->capacity = new_capacity;
    return true;
}

bool heap_grow(Heap* heap) {
    size_t new_capacity = heap->capacity == 0 ? 1 : heap->capacity * 2;
    return heap_resize(heap, new_capacity);
}

bool heap_shrink(Heap* heap) {
    if (heap->capacity <= 1) return true;
    return heap_resize(heap, heap->capacity / 2);
}

void swap(void** arr, size_t i, size_t j) {
    void* tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

bool heap_push(Heap* heap, void* data) {
    if (heap->size == heap->capacity && !heap_grow(heap)) return false;

    size_t index = heap->size;
    heap->entries[index] = data;
    heap->size++;

    while (index) {
        size_t pindex = (index + 1) / 2 - 1;
        if (heap->compare_fn(heap->entries[index], heap->entries[pindex]) != -1) break;

        swap(heap->entries, index, pindex);
        index = pindex;
    }

    return true;
}

MaybeHeapValue heap_pop(Heap* heap) {
    if (heap->size == 0) return (MaybeHeapValue) { .ok = false };

    heap->size--;
    MaybeHeapValue result = { .ok = true, .value = heap->entries[0] };

    if (heap->size != 0) {
        heap->entries[0] = heap->entries[heap->size];

        size_t index = 0;
        while (1) {
            bool left_child_index = (index + 1) * 2 - 1;
            bool right_child_index = (index + 1) * 2;
            bool has_left_child = left_child_index < heap->size;
            bool has_right_child = right_child_index < heap->size;
            if (!has_left_child && !has_right_child) {
                break;
            }

            size_t won_child_index;
            if (has_right_child) {
                won_child_index = heap->compare_fn(heap->entries[left_child_index], heap->entries[right_child_index]) == 1 ? right_child_index : left_child_index;
            } else {
                won_child_index = left_child_index;
            }

            if (heap->compare_fn(heap->entries[won_child_index], heap->entries[index]) != -1) break;

            swap(heap->entries, index, won_child_index);
            index = won_child_index;
        }
    }
    if ((heap->size <= heap->capacity / 2) && !heap_shrink(heap)) return (MaybeHeapValue) { .ok = false };
    return result;
}
