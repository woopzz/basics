#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    void* items;
    size_t itemsize;
    size_t length;
    size_t size;
} Sequence;

typedef struct {
    void* just;
    char nothing;
} MaybeSequenceItem;

void sequence_init(Sequence* seq, size_t itemsize) {
    seq->items = NULL;
    seq->itemsize = itemsize;
    seq->length = 0;
    seq->size = 0;
}

void sequence_free(Sequence* seq) {
    free(seq->items);
}

MaybeSequenceItem sequence_get_at(Sequence* seq, size_t i) {
    if (i >= seq->length) {
        return (MaybeSequenceItem) { .nothing = 1 };
    }
    void* item = seq->items + seq->itemsize * i;
    return (MaybeSequenceItem) { .nothing = 0, .just = item };
}

bool sequence_set_at(Sequence* seq, size_t i, void* item) {
    if (i >= seq->length) return false;
    memcpy(seq->items + seq->itemsize * i, item, seq->itemsize);
    return true;
}

bool _sequence_grow(Sequence* seq) {
    size_t new_size = seq->size == 0 ? 1 : seq->size * 2;
    void* items = realloc(seq->items, new_size * seq->itemsize);
    if (items == NULL) return false;

    memset(items + seq->size * seq->itemsize, 0, (new_size - seq->size) * seq->itemsize);

    seq->items = items;
    seq->size = new_size;
    return true;
}

bool _sequence_shrink(Sequence* seq) {
    size_t new_size = seq->size / 2;
    if (new_size <= seq->length) return true;

    void* items = realloc(seq->items, new_size * seq->itemsize);
    if (items == NULL) return false;

    seq->items = items;
    seq->size = new_size;
    return true;
}

bool sequence_insert_at(Sequence* seq, size_t i, void* item) {
    if (i > seq->length) {
        return false;
    }
    if (seq->length == seq->size && _sequence_grow(seq) == false) {
        return false;
    }
    if (i < seq->length) {
        void* start = seq->items + seq->itemsize * i;
        memcpy(start + seq->itemsize, start, seq->length - i);
    }
    seq->length += 1;
    return sequence_set_at(seq, i, item);
}

bool sequence_delete_at(Sequence* seq, size_t i) {
    if (i >= seq->length) {
        return false;
    }
    if (i + 1 < seq->length) {
        void* end = seq->items + seq->itemsize * i;
        memcpy(end, end + seq->itemsize, (seq->length-i) * seq->itemsize);
    }
    seq->length -= 1;
    return _sequence_shrink(seq);
}

bool sequence_insert_first(Sequence* seq, void* item) {
    return sequence_insert_at(seq, 0, item);
}

bool sequence_delete_first(Sequence* seq) {
    if (seq->length == 0) return false;
    return sequence_delete_at(seq, 0);
}

bool sequence_insert_last(Sequence* seq, void* item) {
    return sequence_insert_at(seq, seq->length, item);
}

bool sequence_delete_last(Sequence* seq) {
    if (seq->length == 0) return false;
    return sequence_delete_at(seq, seq->length-1);
}
