#include <stdlib.h>
#include <string.h>

typedef struct {
    void* items;
    size_t itemsize;
    size_t length;
    size_t size;
} Sequence;

int sequence_init(Sequence* seq, size_t itemsize) {
    seq->items = NULL;
    seq->itemsize = itemsize;
    seq->length = 0;
    seq->size = 0;
    return 1;
}

int sequence_free(Sequence* seq) {
    free(seq->items);
}

void* sequence_get_at(Sequence* seq, size_t i) {
    if (i >= seq->length) {
        return NULL;
    }
    return seq->items + seq->itemsize * i;
}

int sequence_set_at(Sequence* seq, size_t i, void* item) {
    if (i >= seq->length) {
        return 0;
    }
    memcpy(seq->items + seq->itemsize * i, item, seq->itemsize);
    return 1;
}

int _sequence_resize(Sequence* seq) {
    size_t new_size = seq->size == 0 ? 1 : seq->size * 2;
    void* items = realloc(seq->items, new_size * seq->itemsize);
    if (items == NULL) {
        return 0;
    }

    memset(items + seq->size * seq->itemsize, 0, (new_size - seq->size) * seq->itemsize);

    seq->items = items;
    seq->size = new_size;
    return 1;
}

int sequence_insert_at(Sequence* seq, size_t i, void* item) {
    if (i > seq->length) {
        return 0;
    }
    if (seq->length == seq->size && _sequence_resize(seq) == 0) {
        return 0;
    }
    if (i < seq->length) {
        void* start = seq->items + seq->itemsize * i;
        memcpy(start + seq->itemsize, start, seq->length - i);
    }
    seq->length += 1;
    return sequence_set_at(seq, i, item);
}

int sequence_delete_at(Sequence* seq, size_t i) {
    if (i >= seq->length) {
        return 0;
    }
    if (i + 1 < seq->length) {
        void* end = seq->items + seq->itemsize * i;
        memcpy(end, end + seq->itemsize, (seq->length-i) * seq->itemsize);
    }
    seq->length -= 1;
    return 1;
}

int sequence_insert_first(Sequence* seq, void* item) {
    return sequence_insert_at(seq, 0, item);
}

int sequence_delete_first(Sequence* seq) {
    if (seq->length == 0) {
        return 0;
    }
    return sequence_delete_at(seq, 0);
}

int sequence_insert_last(Sequence* seq, void* item) {
    return sequence_insert_at(seq, seq->length, item);
}

int sequence_delete_last(Sequence* seq) {
    if (seq->length == 0) {
        return 0;
    }
    return sequence_delete_at(seq, seq->length-1);
}
