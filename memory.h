
#ifndef DUCKLIB_MEMORY_H
#define DUCKLIB_MEMORY_H

#include "core.h"
#include <stdlib.h>
// typedef struct {
// 	void *elements;
// 	dl_size_t element_size;
// 	dl_size_t elements_length;
// } dl_array_t;

typedef struct {
	void *block;
	dl_ptrdiff_t previousBlock;
	dl_ptrdiff_t nextBlock;
	dl_size_t block_size;
	dl_bool_t allocated;
	dl_bool_t unlinked;
} dl_memoryBlock_t;

// As described here:
// https://www.geeksforgeeks.org/partition-allocation-methods-in-memory-management/
typedef enum {
	dl_memoryFit_first,
	dl_memoryFit_next,
	dl_memoryFit_best,
	dl_memoryFit_worst
} dl_memoryFit_t;

typedef struct dl_memoryAllocation_s {
	void *memory;
	dl_size_t size;
	
	dl_memoryFit_t fit;
	dl_ptrdiff_t mostRecentBlock;
	
	// Firstblock will always equal zero.
	dl_ptrdiff_t firstBlock;
	// // Lastblock may change a lot.
	// dl_ptrdiff_t lastBlock;
	
	dl_memoryBlock_t *blockList;
	dl_size_t blockList_length;
	dl_ptrdiff_t blockList_indexOfBlockList;
	dl_size_t max_used;
	dl_size_t used;
} dl_memoryAllocation_t;

dl_error_t DECLSPEC dl_memory_init(dl_memoryAllocation_t *memoryAllocation, void *memory, dl_size_t size, dl_memoryFit_t fit);
void DECLSPEC dl_memory_quit(dl_memoryAllocation_t *memoryAllocation);
void DECLSPEC dl_memory_printMemoryAllocation(dl_memoryAllocation_t memoryAllocation);
dl_error_t DECLSPEC dl_memory_checkHealth(dl_memoryAllocation_t memoryAllocation);

#if 1
dl_error_t DECLSPEC dl_malloc(dl_memoryAllocation_t *memoryAllocation, void **memory, dl_size_t size);
dl_error_t DECLSPEC dl_free(dl_memoryAllocation_t *memoryAllocation, void **memory);
dl_error_t DECLSPEC dl_realloc(dl_memoryAllocation_t *memoryAllocation, void **memory, dl_size_t size);
#else
#define dl_malloc(memoryAllocation, memory, size) 0;*memory = malloc(size);
#define dl_free(memoryAllocation, memory) 0;free(*memory);
#define dl_realloc(memoryAllocation, memory, size) 0;*memory = realloc(*memory, size);
#endif

#endif // DUCKLIB_MEMORY_H
