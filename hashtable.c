#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define HASHTABLE_INIT_CAPACITY 2
#define HASHTABLE_LOAD_FACTOR 0.5

typedef struct {
    char* key;
    void* value;
} HashTableEntry;

typedef struct {
    uint32_t capacity;
    uint32_t size;
    HashTableEntry* entries;
} HashTable;

typedef struct {
    bool ok;
    void* value;
} MaybeHashTableValue;

void hashtable_init(HashTable* ht) {
    ht->capacity = 0;
    ht->size = 0;
    ht->entries = NULL;
}

void hashtable_free(HashTable* ht) {
    if (ht->entries == NULL) return;

    for (uint32_t i = 0; i < ht->capacity; i++) {
        char* key = ht->entries[i].key;
        if (key != NULL) free(key);
    }
    free(ht->entries);
}

// djb2
uint32_t hash(const char *str) {
    uint32_t hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

uint32_t probe(uint32_t capacity, uint32_t prev_index) {
    return (prev_index + 1) % capacity;
}

uint32_t find_index(HashTableEntry* entries, uint32_t capacity, const char* key) {
    uint32_t index = hash(key) % capacity;
    char* key_;
    while ((key_ = entries[index].key) != NULL && strcmp(key_, key)) index = probe(capacity, index);
    return index;
}

bool hashtable_resize(HashTable* ht, uint32_t new_capacity) {
    HashTableEntry* entries;
    if ((entries = calloc(new_capacity, sizeof(ht->entries[0]))) == NULL) return false;

    for (uint32_t i = 0; i < ht->capacity; i++) {
        HashTableEntry* entry = &ht->entries[i];
        char* key = entry->key;
        if (key == NULL) continue;

        uint32_t index = find_index(entries, new_capacity, key);
        entries[index] = *entry;
    }

    if (ht->entries != NULL) {
        free(ht->entries);
    }

    ht->entries = entries;
    ht->capacity = new_capacity;
    return true;
}

bool hashtable_grow(HashTable* ht) {
    uint32_t new_capacity = ht->capacity == 0 ? HASHTABLE_INIT_CAPACITY : ht->capacity * 2;
    return hashtable_resize(ht, new_capacity);
}

bool hashtable_shrink(HashTable* ht) {
    if (ht->capacity <= HASHTABLE_INIT_CAPACITY) return true;
    return hashtable_resize(ht, ht->capacity / 2);
}

bool hashtable_set(HashTable* ht, const char* key, void* value) {
    if (key == NULL) {
        return false;
    }
    if ((ht->entries == NULL || (ht->size >= ht->capacity * HASHTABLE_LOAD_FACTOR)) && !hashtable_grow(ht)) {
        return false;
    }

    uint32_t index = find_index(ht->entries, ht->capacity, key);
    HashTableEntry* entry = &ht->entries[index];
    if (entry->key != NULL) {
        entry->value = value;
        return true;
    }

    if ((entry->key = strdup(key)) == NULL) return false;
    entry->value = value;
    ht->size++;
    return true;
}

MaybeHashTableValue hashtable_get(HashTable* ht, const char* key) {
    if (ht->entries == NULL) return (MaybeHashTableValue) { .ok = false };

    uint32_t index = find_index(ht->entries, ht->capacity, key);
    HashTableEntry* entry = &ht->entries[index];

    MaybeHashTableValue result;
    if (entry->key) {
        result.ok = true;
        result.value = entry->value;
    } else {
        result.ok = false;
    }
    return result;
}

void hashtable_backward_shift(HashTable* ht, uint32_t index) {
    uint32_t next = probe(ht->capacity, index);
    while (ht->entries[next].key != NULL) {
        ht->entries[index] = ht->entries[next];
        index = next;
        next = probe(ht->capacity, index);
    }
    memset(&ht->entries[index], 0, sizeof(HashTableEntry));
}

bool hashtable_delete(HashTable* ht, char* key) {
    if (key == NULL) return false;

    uint32_t index = find_index(ht->entries, ht->capacity, key);
    HashTableEntry* entry = &ht->entries[index];
    if (entry->key == NULL) return true;

    free(entry->key);
    ht->size--;

    hashtable_backward_shift(ht, index);
    if ((ht->size < ht->capacity * HASHTABLE_LOAD_FACTOR) && !hashtable_shrink(ht)) return false;

    return true;
}

void hashtable_print(HashTable* ht) {
    printf("Hash table meta info: capacity = %d, size = %d\n", ht->capacity, ht->size);
    for (uint32_t i = 0; i < ht->capacity; i++) {
        if (ht->entries[i].key == NULL) {
            printf("Empty at pos %d\n", i);
        } else {
            printf("A value with a key '%s' and a value %p at pos %d\n", ht->entries[i].key, ht->entries[i].value, i);
        }
    }
}
