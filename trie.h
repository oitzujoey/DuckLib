
#ifndef DUCKLIB_TRIE_H
#define DUCKLIB_TRIE_H

#include "core.h"
#include "memory.h"

typedef struct dl_trie_node_s {
	struct {
		char **nodes_name;
		dl_size_t *nodes_name_lengths;
		struct dl_trie_node_s *nodes;
		dl_size_t nodes_length;
	} value;
	dl_ptrdiff_t index;
} dl_trie_node_t;

typedef struct {
	dl_memoryAllocation_t *memoryAllocation;
	dl_trie_node_t trie;
} dl_trie_t;

void dl_trie_init(dl_trie_t *trie, dl_memoryAllocation_t *memoryAllocation, dl_ptrdiff_t nullIndex);
dl_error_t dl_trie_quit(dl_trie_t *trie);
void dl_trie_print(dl_trie_t trie);
void dl_trie_print_compact(dl_trie_t trie);
dl_error_t dl_trie_insert(dl_trie_t *trie, const char *key, const dl_size_t key_length, const dl_ptrdiff_t index);
void dl_trie_find(const dl_trie_t trie, dl_ptrdiff_t *index, const char *key, const dl_size_t key_length);

#endif /* DUCKLIB_TRIE_H */
