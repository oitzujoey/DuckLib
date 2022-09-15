
#include "trie.h"
#include "core.h"
#include "memory.h"
#include "string.h"
#include <stdio.h>

/*
It's called "nullIndex" because it is the index that is returned when passed a null string.
*/
void dl_trie_init(dl_trie_t *trie, dl_memoryAllocation_t *memoryAllocation, dl_ptrdiff_t nullIndex) {
	trie->memoryAllocation = memoryAllocation;
	trie->trie.index = nullIndex;
	trie->trie.value.nodes = dl_null;
	trie->trie.value.nodes_length = 0;
	trie->trie.value.nodes_name = dl_null;
	trie->trie.value.nodes_name_lengths = dl_null;
}

static void dl_trie_init_node(dl_trie_node_t *trieNode,
                              const dl_ptrdiff_t index,
                              dl_trie_node_t *nodes,
                              const dl_size_t nodes_length,
                              char **nodes_name,
                              dl_size_t *nodes_name_lengths) {
	trieNode->index = index;
	trieNode->value.nodes = nodes;
	trieNode->value.nodes_length = nodes_length;
	trieNode->value.nodes_name = nodes_name;
	trieNode->value.nodes_name_lengths = nodes_name_lengths;
}

static dl_error_t dl_trie_quit_node(dl_trie_t *trie, dl_trie_node_t *trieNode) {
	dl_error_t e = dl_error_ok;

	dl_ptrdiff_t node_index = 0;
	/*l_nodeTraverse:*/ {
		e = dl_free(trie->memoryAllocation, (void **) &trieNode->value.nodes_name[node_index]);
		if (e) {
			goto l_cleanup;
		}
		e = dl_trie_quit_node(trie, &trieNode->value.nodes[node_index]);
		if (e) {
			goto l_cleanup;
		}
	} // Fall through.

	e = dl_free(trie->memoryAllocation, (void **) &trieNode->value.nodes_name_lengths);
	if (e) {
		goto l_cleanup;
	}

	e = dl_free(trie->memoryAllocation, (void **) &trieNode->value.nodes_name);
	if (e) {
		goto l_cleanup;
	}

	e = dl_free(trie->memoryAllocation, (void **) &trieNode->value.nodes);
	if (e) {
		goto l_cleanup;
	}

	trieNode->index = -1;

	l_cleanup:

	return e;
}

dl_error_t dl_trie_quit(dl_trie_t *trie) {
	dl_error_t e = dl_error_ok;

	e = dl_trie_quit_node(trie, &trie->trie);
	if (e) {
		goto l_cleanup;
	}

	trie->memoryAllocation = dl_null;

	l_cleanup:

	return e;
}

static void dl_trie_print_node(dl_trie_node_t trieNode, dl_size_t indentation) {
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	printf("index: %lli\n", trieNode.index);
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	printf("nodes_length: %llu\n", trieNode.value.nodes_length);
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	printf("nodes: { /* %llX */\n", (long long) trieNode.value.nodes);
	for (dl_size_t i = 0; i < trieNode.value.nodes_length; i++) {
		dl_trie_print_node(trieNode.value.nodes[i], indentation + 1);
	}
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	puts("}");
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	printf("nodes_name_lengths: { /* %llX */\n", (long long) trieNode.value.nodes_name_lengths);
	for (dl_size_t i = 0; i < trieNode.value.nodes_length; i++) {
		for (dl_size_t j = 0; j < indentation + 1; putchar('\t'),j++);
		printf("%llu\n", trieNode.value.nodes_name_lengths[i]);
	}
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	puts("}");
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	printf("nodes_name: { /* %llX */\n", (long long) trieNode.value.nodes_name);
	for (dl_size_t i = 0; i < trieNode.value.nodes_length; i++) {
		for (dl_size_t j = 0; j < indentation + 1; putchar('\t'),j++);
		for (dl_size_t j = 0; j < trieNode.value.nodes_name_lengths[i]; j++) {
			putchar(trieNode.value.nodes_name[i][j]);
		}
		putchar('\n');
	}
	for (dl_size_t i = 0; i < indentation; putchar('\t'),i++);
	puts("}");
}

void dl_trie_print(dl_trie_t trie) {
	printf("memoryAllocation: %llX\n", (long long) trie.memoryAllocation);
	dl_trie_print_node(trie.trie, 0);
}

static void dl_trie_print_node_compact(dl_trie_node_t trieNode, dl_size_t indentation) {
	for (dl_size_t i = 0; i < trieNode.value.nodes_length; i++) {
		for (dl_size_t j = 0; j < indentation + 1; putchar('\t'),j++);
		for (dl_size_t j = 0; j < trieNode.value.nodes_name_lengths[i]; j++) {
			putchar(trieNode.value.nodes_name[i][j]);
		}
		printf(": %lli\n", trieNode.value.nodes[i].index);
		// printf(": ");
		dl_trie_print_node_compact(trieNode.value.nodes[i], indentation + 1);
	}
}

void dl_trie_print_compact(dl_trie_t trie) {
	printf(": %lli\n", trie.trie.index);
	dl_trie_print_node_compact(trie.trie, 0);
}

static dl_error_t dl_trie_pushNode(dl_trie_t *trie,
                                   dl_trie_node_t *trieNode,
                                   const char *key,
                                   const dl_size_t key_length,
                                   const dl_ptrdiff_t index) {
	dl_error_t e = dl_error_ok;

	e = dl_realloc(trie->memoryAllocation,
	               (void **) &trieNode->value.nodes,
	               (trieNode->value.nodes_length + 1) * sizeof(dl_trie_node_t));
	if (e) {
		goto l_cleanup;
	}

	/**/ dl_trie_init_node(&trieNode->value.nodes[trieNode->value.nodes_length],
	                       index,
	                       dl_null,
	                       0,
	                       dl_null,
	                       dl_null);

	// Assume that nothing has been allocated for the key either.

	// Key lengths.
	e = dl_realloc(trie->memoryAllocation,
	               (void **) &trieNode->value.nodes_name_lengths,
	               (trieNode->value.nodes_length + 1) * sizeof(char *));
	if (e) {
		goto l_cleanup;
	}

	// This key length.
	trieNode->value.nodes_name_lengths[trieNode->value.nodes_length] = key_length;

	// Keys array.
	e = dl_realloc(trie->memoryAllocation,
	               (void **) &trieNode->value.nodes_name,
	               (trieNode->value.nodes_length + 1) * sizeof(char *));
	if (e) {
		goto l_cleanup;
	}

	// This key.
	e = dl_malloc(trie->memoryAllocation,
	              (void **) &trieNode->value.nodes_name[trieNode->value.nodes_length],
	              key_length * sizeof(char));
	if (e) {
		goto l_cleanup;
	}

	/**/ dl_memcopy_noOverlap(trieNode->value.nodes_name[trieNode->value.nodes_length], key, key_length);
	trieNode->value.nodes_length++;

	l_cleanup:

	return e;
        }

static dl_error_t dl_trie_insert_helper(dl_trie_t *trie,
                                        dl_bool_t *success,
                                        dl_trie_node_t *node,
                                        char **node_name,
                                        dl_size_t *node_name_length,
                                        const char *key,
                                        const dl_size_t key_length,
                                        const dl_ptrdiff_t index) {
	dl_error_t e = dl_error_ok;
	/* dl_ptrdiff_t node_index = 0; */
	dl_ptrdiff_t name_index = 0;

	*success = dl_false;

	if (key_length == 0) {
		node->index = index;
		*success = dl_true;
		goto cleanup;
	}

	while (dl_true) {
		/* Correct, but key is too long for this node. */
		if ((dl_size_t) name_index == *node_name_length) {
			// Ran out of characters. All characters were correct. Try child nodes.
			DL_DOTIMES(node_index, node->value.nodes_length) {
				e = dl_trie_insert_helper(trie,
				                          success,
				                          &node->value.nodes[node_index],
				                          &node->value.nodes_name[node_index],
				                          &node->value.nodes_name_lengths[node_index],
				                          &key[name_index],
				                          key_length - name_index,
				                          index);
				if (*success) break;
			}
			if (!*success) {
				/* No child nodes match at all, so create a new one. */
				e = dl_trie_pushNode(trie, node, &key[name_index], key_length - name_index, index);
				if (e) break;
				*success = dl_true;
			}
			break;
		}
		/* Correct, but key is too short for this node. */
		else if ((dl_size_t) name_index == key_length) {
			/* Split. */
			/* Save current node. (Save the children!) */
			dl_trie_node_t tempNode = *node;
			/* Reinitialize the node. Put the index here. */
			/**/ dl_trie_init_node(node,
			                       index,
			                       dl_null,
			                       0,
			                       dl_null,
			                       dl_null);
			/* Push the saved node into the newly initialized node. */
			e = dl_trie_pushNode(trie,
			                     node,
			                     &(*node_name)[name_index],
			                     *node_name_length - name_index,
			                     tempNode.index);
			if (e) break;
			node->value.nodes[0] = tempNode;

			e = dl_realloc(trie->memoryAllocation,
			               (void **) node_name,
			               name_index * sizeof(char));
			if (e) break;
			/* Trim current node's name to match key. */
			*node_name_length = name_index;

			*success = dl_true;
			break;
		}
		/* Key mismatch */
		else if (key[name_index] != (*node_name)[name_index]) {  /* Exit point */
			if (name_index != 0) {
				/* Split. */
				dl_trie_node_t tempNode = *node;
				dl_trie_init_node(node,
				                  index,
				                  dl_null,
				                  0,
				                  dl_null,
				                  dl_null);
				e = dl_trie_pushNode(trie,
				                     node,
				                     &(*node_name)[name_index],
				                     *node_name_length - name_index,
				                     node->index);
				if (e) break;

				// Add node.
				e = dl_trie_pushNode(trie,
				                     node,
				                     &key[name_index],
				                     key_length - name_index,
				                     index);
				if (e) break;
				// Next node.
				// node = &node->value.nodes[0];
				// offset += name_index;
				// Splice to finish split.
				node->value.nodes[0] = tempNode;

				// Delete the redundant end of the original node's key.
				e = dl_realloc(trie->memoryAllocation,
				               (void **) node_name,
				               name_index * sizeof(char));
				if (e) break;
				*node_name_length = name_index;

				*success = dl_true;
				break;
			}
			else break;  /* Wrong key. */
		}
		name_index++;
	}

 cleanup:

	return e;
}

dl_error_t dl_trie_insert(dl_trie_t *trie, const char *key, const dl_size_t key_length, const dl_ptrdiff_t index) {
	dl_bool_t success;
	/* Danger: Static allocation.
	   This is only OK because there is no way for `dl_trie_insert_helper` to free this. */
	char *node_name = "";
	dl_size_t node_name_length = 0;
	return dl_trie_insert_helper(trie, &success, &trie->trie, &node_name, &node_name_length, key, key_length, index);
}

void dl_trie_find(dl_trie_t trie, dl_ptrdiff_t *index, const char *key, const dl_size_t key_length) {

	dl_trie_node_t *trieNode = &trie.trie;
	dl_ptrdiff_t offset = 0;

	dl_ptrdiff_t node_index = 0;
	dl_size_t name_index = 0;

	// I wonder how bad it would be if I made a while loop out of gotos? Let's try it.
	l_nodeTraverse: {
		// Did we find the leaf node?
		if ((dl_size_t) offset == key_length) {
			// Yes. Reassign the index.
			*index = trieNode->index;
			return;
		}

		if ((dl_size_t) node_index >= trieNode->value.nodes_length) {
			*index = -1;
			return;
		}

		name_index = 0;
		l_nameTraverse: {
			if (name_index >= trieNode->value.nodes_name_lengths[node_index]) {
				// Ran out of characters. All characters were correct. Goto to this child node.
				offset += trieNode->value.nodes_name_lengths[node_index];
				trieNode = &trieNode->value.nodes[node_index];
				node_index = 0;
				goto l_nodeTraverse;
			}
			else if (offset + name_index >= key_length) {
				*index = -1;
				return;
			}
			else if (key[offset + name_index] != trieNode->value.nodes_name[node_index][name_index]) {
				if (name_index != 0) {
					*index = -1;
					return;
				}
				else {
					node_index++;
					goto l_nodeTraverse;
				}
			}
			else {
				name_index++;
				goto l_nameTraverse;
			}
		} // Can't fall through.

		node_index++;
		goto l_nodeTraverse;
	} // Can't fall through.
	// OK. `goto`s weren't ideal, though they did allow me to do some interesting things.
}
