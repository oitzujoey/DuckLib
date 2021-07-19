
#include "memory.h"
#include <stdio.h>


dl_error_t dl_memory_init(dl_memoryAllocation_t *memoryAllocation, void *memory, dl_size_t size, dl_memoryFit_t fit) {
	dl_error_t error;
	
	// Might as well since we are only doing it once.
	if ((memoryAllocation == dl_null) || (memory == dl_null)) {
		error = dl_error_nullPointer;
		goto l_cleanup;
	}
	
	// Initialize.
	memoryAllocation->memory = memory;
	memoryAllocation->size = size;
	
	memoryAllocation->blockList = memory;
	/*
	0   Block list
	1   Unallocated block
	*/
	memoryAllocation->blockList_length = 2;
	memoryAllocation->blockList_indexOfBlockList = 0;
	
	if (memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t) > memoryAllocation->size) {
		error = dl_error_outOfMemory;
		goto l_cleanup;
	}
	
	
	// Our allocations table is our first allocation.
	memoryAllocation->blockList[0].block = memory;
	memoryAllocation->blockList[0].block_size = memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
	memoryAllocation->blockList[0].allocated = dl_true;
	
	memoryAllocation->blockList[0].previousBlock = -1;
	// Next block is the massive chunk of unallocated memory.
	memoryAllocation->blockList[0].nextBlock = 1;
	memoryAllocation->blockList[0].unlinked = dl_false;
	
	
	// Stick the block after the block list.
	memoryAllocation->blockList[1].block = memory + memoryAllocation->blockList[0].block_size;
	// Claim the rest of the block.
	memoryAllocation->blockList[1].block_size = size - memoryAllocation->blockList[0].block_size;
	if (memoryAllocation->blockList[1].block_size == 0) {
		error = dl_error_outOfMemory;
		goto l_cleanup;
	}
	memoryAllocation->blockList[1].allocated = dl_false;
	
	// Previous block is the block list.
	memoryAllocation->blockList[1].previousBlock = 0;
	memoryAllocation->blockList[1].nextBlock = -1;
	memoryAllocation->blockList[1].unlinked = dl_false;
	
	
	// First block is the block list.
	memoryAllocation->firstBlock = 0;
	// Last block is the block of unallocated memory.
	memoryAllocation->lastBlock = 1;
	
	
	memoryAllocation->mostRecentBlock = 0;
	memoryAllocation->fit = fit;
	
	
	error = dl_error_ok;
	l_cleanup:
	
	if (error) {
		memoryAllocation = dl_null;
	}
	
	return error;
}

// Actually optional if you don't care about dangling pointers.
void dl_memory_quit(dl_memoryAllocation_t *memoryAllocation) {
	memoryAllocation->memory = dl_null;
	memoryAllocation->size = 0;
	memoryAllocation->blockList = dl_null;
	memoryAllocation->blockList_length = 0;
	memoryAllocation->blockList_indexOfBlockList = -1;
	memoryAllocation->firstBlock = -1;
	memoryAllocation->lastBlock = -1;
	memoryAllocation->mostRecentBlock = -1;
	// memoryAllocation->fit = 
}

void dl_memory_printMemoryAllocation(dl_memoryAllocation_t memoryAllocation) {

// Console colors
#define COLOR_NORMAL    "\x1B[0m"
#define COLOR_BLACK     "\x1B[30m"
#define COLOR_RED       "\x1B[31m"
#define COLOR_GREEN     "\x1B[32m"
#define COLOR_YELLOW    "\x1B[33m"
#define COLOR_BLUE      "\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"
#define COLOR_CYAN      "\x1B[36m"
#define COLOR_WHITE     "\x1B[37m"

// Console background colors
#define B_COLOR_BLACK     "\x1B[40m"
#define B_COLOR_RED       "\x1B[41m"
#define B_COLOR_GREEN     "\x1B[42m"
#define B_COLOR_YELLOW    "\x1B[43m"
#define B_COLOR_BLUE      "\x1B[44m"
#define B_COLOR_MAGENTA   "\x1B[45m"
#define B_COLOR_CYAN      "\x1B[46m"
#define B_COLOR_WHITE     "\x1B[47m"

	printf("(dl_memoryAllocation_t) {\n");
	printf("\t.memory = %llu,\n", memoryAllocation.memory);
	printf("\t.size = %llu,\n", memoryAllocation.size);
	printf("\t.fit = %s,\n",
		((memoryAllocation.fit == dl_memoryFit_first) ?
			"dl_memoryFit_first"
		: ((memoryAllocation.fit == dl_memoryFit_next) ?
			"dl_memoryFit_next"
		: ((memoryAllocation.fit == dl_memoryFit_best) ?
			"dl_memoryFit_best"
		: ((memoryAllocation.fit == dl_memoryFit_worst) ?
			"dl_memoryFit_worst"
		:
			"Illegal value"))))
	);
	printf("\t.mostRecentBlock = %lli,\n", memoryAllocation.mostRecentBlock);
	printf("\t.firstBlock = %lli,\n", memoryAllocation.firstBlock);
	printf("\t.lastBlock = %lli,\n", memoryAllocation.lastBlock);
	printf("\t.blockList_length = %llu,\n", memoryAllocation.blockList_length);
	printf("\t.blockList_indexOfBlockList = %lli,\n", memoryAllocation.blockList_indexOfBlockList);
	printf("\t.blockList_indexOfBlockList = {\n");
	for (dl_size_t i = 0; i < memoryAllocation.blockList_length; i++) {
		if (memoryAllocation.blockList[i].allocated) {
			if (memoryAllocation.blockList[i].unlinked) {
				printf(COLOR_RED);
			}
			else {
				printf(COLOR_GREEN);
			}
		}
		else {
			if (memoryAllocation.blockList[i].unlinked) {
				printf(COLOR_MAGENTA);
			}
			else {
				printf(COLOR_YELLOW);
			}
		}
		printf("\t\t(dl_memoryBlock_t) {\n");
		printf("\t\t\t.block = %llu, /* offset = %llu */\n", memoryAllocation.blockList[i].block, memoryAllocation.blockList[i].block - memoryAllocation.memory);
		printf("\t\t\t.block_size = %llu,\n", memoryAllocation.blockList[i].block_size);
		printf("\t\t\t.allocated = %s,\n", memoryAllocation.blockList[i].allocated ? "true" : "false");
		printf("\t\t\t.unlinked = %s,\n", memoryAllocation.blockList[i].unlinked ? "true" : "false");
		printf("\t\t\t.previousBlock = %lli,\n", memoryAllocation.blockList[i].previousBlock);
		printf("\t\t\t.nextBlock = %lli,\n", memoryAllocation.blockList[i].nextBlock);
		printf("\t\t},\n");
		printf(COLOR_NORMAL);
	}
	printf("\t},\n");
	printf("}\n");
}

// Ask specifically for fit, because who knows, maybe there's exceptions.
dl_error_t dl_memory_findBlock(dl_memoryAllocation_t *memoryAllocation, dl_ptrdiff_t *block, dl_size_t size, dl_memoryFit_t fit) {
	dl_error_t error = dl_error_ok;
	
	dl_ptrdiff_t optimumBlock = -1;
	dl_ptrdiff_t currentBlock = -1;
	dl_bool_t found = dl_false;
	
	if (fit == dl_memoryFit_next) {
		currentBlock = memoryAllocation->blockList[memoryAllocation->mostRecentBlock].nextBlock;
	}
	else {
		currentBlock = memoryAllocation->firstBlock;
	}
	
	if (currentBlock < 0) {
		printf("negative\n");
	}
	
	// Find the smallest of the empty blocks.
	while (!found && (currentBlock != -1)) {
		if (!memoryAllocation->blockList[currentBlock].allocated && (memoryAllocation->blockList[currentBlock].block_size >= size)) {
			if (optimumBlock == -1) {
				optimumBlock = currentBlock;
				found = dl_true;
			}
			else switch (fit) {
			case dl_memoryFit_first:
				optimumBlock = currentBlock;
				break;
			
			// case dl_memoryFit_next:
			// 	optimumBlock = currentBlock;
			// 	break;
			
			case dl_memoryFit_best:
				if (memoryAllocation->blockList[currentBlock].block_size < memoryAllocation->blockList[optimumBlock].block_size) {
					optimumBlock = currentBlock;
				}
				break;
			
			case dl_memoryFit_worst:
				if (memoryAllocation->blockList[currentBlock].block_size > memoryAllocation->blockList[optimumBlock].block_size) {
					optimumBlock = currentBlock;
				}
				break;
			
			default:
				error = dl_error_shouldntHappen;
				goto l_cleanup;
			}
		}
		currentBlock = memoryAllocation->blockList[currentBlock].nextBlock;
	}
	
	if (optimumBlock == -1) {
		error = dl_error_outOfMemory;
		goto l_cleanup;
	}
	
	error = dl_error_ok;
	l_cleanup:
	
	if (error) {
		*block = -1;
	}
	else {
		*block = optimumBlock;
	}
	
	return error;
}

// We simply mark deleted block descriptors as unlinked because dl_memory_pushBlockEntry uses this function and we want to touch the table as little as possible.
dl_error_t dl_memory_mergeBlocks(dl_memoryAllocation_t *memoryAllocation, dl_ptrdiff_t *block) {
	dl_error_t error = dl_error_ok;
	
	dl_size_t previousBlock = memoryAllocation->blockList[*block].previousBlock;
	dl_size_t nextBlock = memoryAllocation->blockList[*block].nextBlock;
	
	if ((nextBlock != -1) && !memoryAllocation->blockList[nextBlock].allocated) {
		// Increase size of *block.
		memoryAllocation->blockList[*block].block_size += memoryAllocation->blockList[nextBlock].block_size;
		// Unlink nextBlock.
		memoryAllocation->blockList[*block].nextBlock = memoryAllocation->blockList[nextBlock].nextBlock;
		if (memoryAllocation->blockList[*block].nextBlock == -1) {
			memoryAllocation->lastBlock = *block;
		}
		else {
			memoryAllocation->blockList[memoryAllocation->blockList[*block].nextBlock].previousBlock = *block;
		}
		// Free nextBlock's entry.
		memoryAllocation->blockList[nextBlock].unlinked = dl_true;
		nextBlock = memoryAllocation->blockList[*block].nextBlock;
	}
	
	if ((previousBlock != -1) && !memoryAllocation->blockList[previousBlock].allocated) {
		// Increase size of previousBlock.
		memoryAllocation->blockList[previousBlock].block_size += memoryAllocation->blockList[*block].block_size;
		// Unlink *block.
		memoryAllocation->blockList[previousBlock].nextBlock = nextBlock;
		memoryAllocation->blockList[nextBlock].previousBlock = previousBlock;
		// Free *block's entry.
		memoryAllocation->blockList[*block].unlinked = dl_true;
		// Make *block point to previousBlock.
		*block = previousBlock;
	}
	
	// error = dl_error_ok;
	// l_cleanup:
	
	return error;
}

// // Remove unlinked block descriptors.
// dl_error_t dl_memory_cleanBlockList(dl_memoryAllocation_t *memoryAllocation) {
// 	dl_error_t error = dl_error_ok;
	
// 	dl_ptrdiff_t previousBlock = -1;
// 	dl_ptrdiff_t nextBlock = -1;
	
// 	// This is about the one time we can do this with the block list.
// 	for (dl_size_t i = 0; i < memoryAllocation->blockList_length; i++) {
// 		if (memoryAllocation->blockList[i].unlinked) {
// 			previousBlock = memoryAllocation->blockList[i].previousBlock;
// 			nextBlock = memoryAllocation->blockList[i].nextBlock;
			
// 			if (previousBlock != -1) {
// 				memoryAllocation->blockList[previousBlock].
// 			}
// 		}
// 	}
	
// 	error = dl_error_ok;
// 	l_cleanup:
	
// 	return error;
// }

// New block will be at the address (*block)->nextBlock after the function returns.
dl_error_t dl_memory_splitBlock(dl_memoryAllocation_t *memoryAllocation, dl_ptrdiff_t *block, dl_size_t index) {
	dl_error_t error = dl_error_ok;
	
	dl_ptrdiff_t oldBlock = memoryAllocation->blockList_indexOfBlockList;
	dl_ptrdiff_t tempBlock = -1;
	dl_ptrdiff_t newBlock = -1;
	dl_ptrdiff_t unlinkedBlock = -1;
	
	/*
	What we are trying to do here is find a safe place in memory to put the
	block list. After that, we copy the block list to its new home. We of course
	leave some room for a block descriptor at the end of the list, but we don't
	touch it yet, mainly because I want to keep this complicated section as
	simple as possible. I may have failed in that endeavor.
	*/
	
	// Find an unlinked descriptor for the NEW BLOCK if possible.
	for (dl_ptrdiff_t i = 0; i < memoryAllocation->blockList_length; i++) {
		if (memoryAllocation->blockList[i].unlinked) {
			unlinkedBlock = i;
			break;
		}
	}
	if (unlinkedBlock == -1) {
		// Mark block that the table is currently residing in deleted.
		memoryAllocation->blockList[memoryAllocation->blockList_indexOfBlockList].allocated = dl_false;
		
		// Our table will still be here. We just need to be careful that we don't give the memory to anyone else before we copy it.
		// The table size will remain unchanged until we run dl_memory_cleanTable (if that function even exists).
		// This may create unlinked blocks that we can use in a moment. This would be the best case scenario.
		tempBlock = oldBlock;
		error = dl_memory_mergeBlocks(memoryAllocation, &tempBlock);
		if (error) {
			goto l_cleanup;
		}
		
		// +1 because we are expanding the list by one.
		error = dl_memory_findBlock(memoryAllocation, &newBlock, (memoryAllocation->blockList_length + 1) * sizeof(dl_memoryBlock_t), dl_memoryFit_first);
		if (error) {
			goto l_cleanup;
		}
		if ((memoryAllocation->blockList_length + 1) * sizeof(dl_memoryBlock_t) != memoryAllocation->blockList[newBlock].block_size) {
			// Split.
			// Note reuse of `unlinkedBlock`.
			// Find an unlinked descriptor for the BLOCK LIST if possible.
			for (dl_ptrdiff_t i = 0; i < memoryAllocation->blockList_length; i++) {
				if (memoryAllocation->blockList[i].unlinked) {
					unlinkedBlock = i;
					break;
				}
			}
			if (unlinkedBlock == -1) {
				// Doh! We need to create one.
				// Find a block with more room.
				// +2 for the original push + the split.
				error = dl_memory_findBlock(memoryAllocation, &newBlock, (memoryAllocation->blockList_length + 2) * sizeof(dl_memoryBlock_t), dl_memoryFit_first);
				if (error) {
					goto l_cleanup;
				}
				
				if ((memoryAllocation->blockList_length + 2) * sizeof(dl_memoryBlock_t) != memoryAllocation->blockList[newBlock].block_size) {
					// This is what we prepared for. Split the block.
					
					// Copy the block list to the new block.
					if (oldBlock != newBlock) {
						printf("1a\n");
						error = dl_memcopy(memoryAllocation->blockList[newBlock].block,
						                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t),
						                   memoryAllocation->blockList[memoryAllocation->blockList_indexOfBlockList].block,
						                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t));
						if (error) {
							goto l_cleanup;
						}
						
						memoryAllocation->blockList[newBlock].allocated = dl_true;
						
						// Transfer control to the new block.
						memoryAllocation->blockList = memoryAllocation->blockList[newBlock].block;
						memoryAllocation->blockList_indexOfBlockList = newBlock;
					}
					printf("1b\n");
					memoryAllocation->blockList_length += 2;
					
					// Split.
					
					// Second to last entry.
					tempBlock = memoryAllocation->blockList[newBlock].nextBlock;
					memoryAllocation->blockList[newBlock].nextBlock = memoryAllocation->blockList_length - 2;
					
					memoryAllocation->blockList[memoryAllocation->blockList[newBlock].nextBlock].unlinked = dl_false;
					memoryAllocation->blockList[memoryAllocation->blockList[newBlock].nextBlock].allocated = dl_false;
					memoryAllocation->blockList[memoryAllocation->blockList[newBlock].nextBlock].block = memoryAllocation->blockList[newBlock].block
					    + memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
					memoryAllocation->blockList[memoryAllocation->blockList[newBlock].nextBlock].block_size
					    = memoryAllocation->blockList[newBlock].block_size
					    - memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
					memoryAllocation->blockList[memoryAllocation->blockList[newBlock].nextBlock].nextBlock = tempBlock;
					memoryAllocation->blockList[memoryAllocation->blockList[newBlock].nextBlock].previousBlock = newBlock;
					
					if (tempBlock != -1) {
						memoryAllocation->blockList[tempBlock].previousBlock = newBlock;
						printf("1c1\n");
					}
					else {
						memoryAllocation->lastBlock = newBlock;
						printf("1c2\n");
					}
					
					memoryAllocation->blockList[newBlock].block_size = memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
				}
				else {
					// We got (un)lucky. Add a dummy descriptor.
					// If the block is the same, then we're good. Otherwise, we need to copy.
					if (oldBlock != newBlock) {
						printf("2a\n");
						error = dl_memcopy(memoryAllocation->blockList[newBlock].block,
						                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t),
						                   memoryAllocation->blockList[memoryAllocation->blockList_indexOfBlockList].block,
						                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t));
						if (error) {
							goto l_cleanup;
						}
						
						memoryAllocation->blockList[newBlock].allocated = dl_true;
						
						// Transfer to the new blockList.
						memoryAllocation->blockList = memoryAllocation->blockList[newBlock].block;
						memoryAllocation->blockList_indexOfBlockList = newBlock;
					}
					printf("2b\n");
					memoryAllocation->blockList_length += 2;
					
					// Get index of dummy descriptor.
					tempBlock = memoryAllocation->blockList_length - 2;
					
					memoryAllocation->blockList[tempBlock].unlinked = dl_true;
					memoryAllocation->blockList[tempBlock].allocated = dl_false;
					memoryAllocation->blockList[tempBlock].block = dl_null;
					memoryAllocation->blockList[tempBlock].block_size = 0;
					memoryAllocation->blockList[tempBlock].nextBlock = -1;
					memoryAllocation->blockList[tempBlock].previousBlock = -1;
				}
			}
			else {
				// Split the block.
				
				// Copy the block list to the new block.
				if (oldBlock != newBlock) {
					printf("3a\n");
					error = dl_memcopy(memoryAllocation->blockList[newBlock].block,
					                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t),
					                   memoryAllocation->blockList[memoryAllocation->blockList_indexOfBlockList].block,
					                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t));
					if (error) {
						goto l_cleanup;
					}
					
					memoryAllocation->blockList[newBlock].allocated = dl_true;
					
					// Transfer control to the new block.
					memoryAllocation->blockList = memoryAllocation->blockList[newBlock].block;
					memoryAllocation->blockList_indexOfBlockList = newBlock;
				}
				printf("3b\n");
				memoryAllocation->blockList_length += 1;
				
				// Split.
				printf("newBlock %lli\n", newBlock);
				tempBlock = memoryAllocation->blockList[newBlock].nextBlock;
				memoryAllocation->blockList[newBlock].nextBlock = unlinkedBlock;
				
				memoryAllocation->blockList[unlinkedBlock].unlinked = dl_false;
				memoryAllocation->blockList[unlinkedBlock].allocated = dl_false;
				memoryAllocation->blockList[unlinkedBlock].block = memoryAllocation->blockList[newBlock].block
				                                                 + memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
				memoryAllocation->blockList[unlinkedBlock].block_size
				    = memoryAllocation->blockList[newBlock].block_size
				    - memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
				memoryAllocation->blockList[unlinkedBlock].nextBlock = tempBlock;
				memoryAllocation->blockList[unlinkedBlock].previousBlock = newBlock;
				
				if (tempBlock != -1) {
					printf("3c1\n");
					memoryAllocation->blockList[tempBlock].previousBlock = unlinkedBlock;
				}
				else {
					printf("3c2\n");
					memoryAllocation->lastBlock = unlinkedBlock;
				}
				
				memoryAllocation->blockList[newBlock].block_size = memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t);
			}
		}
		else {
			// If the block is the same, then we're good. Otherwise, we need to copy.
			if (oldBlock != newBlock) {
				printf("4a\n");
				error = dl_memcopy(memoryAllocation->blockList[newBlock].block,
				                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t),
				                   memoryAllocation->blockList[memoryAllocation->blockList_indexOfBlockList].block,
				                       memoryAllocation->blockList_length * sizeof(dl_memoryBlock_t));
				if (error) {
					goto l_cleanup;
				}
				
				memoryAllocation->blockList[newBlock].allocated = dl_true;
				
				// Transfer control to the new block.
				memoryAllocation->blockList = memoryAllocation->blockList[newBlock].block;
				memoryAllocation->blockList_indexOfBlockList = newBlock;
			}
			printf("4b\n");
			memoryAllocation->blockList_length += 1;
		}
		
		memoryAllocation->blockList[memoryAllocation->blockList_indexOfBlockList].allocated = dl_true;
		unlinkedBlock = memoryAllocation->blockList_length - 1;
	}
	else {
		// Woo-hoo! We found an unlinked block!
		printf("5\n");
	}
	
	error = dl_memory_findBlock(memoryAllocation, block, index, dl_memoryFit_first);
	if (error) {
		goto l_cleanup;
	}
	
	// OK. Now that we are done with that MESS. We can finally do what we were doing in the first place: creating a new block.
	// Actually, we have the block now. All we have to do now is initialize it.
	// This block will be inserted after the given block.
	memoryAllocation->blockList[unlinkedBlock].unlinked = dl_false;
	memoryAllocation->blockList[unlinkedBlock].allocated = dl_false;
	memoryAllocation->blockList[unlinkedBlock].block = memoryAllocation->blockList[*block].block + index;
	memoryAllocation->blockList[unlinkedBlock].block_size = memoryAllocation->blockList[*block].block_size - index;
	memoryAllocation->blockList[unlinkedBlock].nextBlock = memoryAllocation->blockList[*block].nextBlock;
	memoryAllocation->blockList[unlinkedBlock].previousBlock = *block;
	
	if (memoryAllocation->blockList[unlinkedBlock].nextBlock != -1) {
		memoryAllocation->blockList[memoryAllocation->blockList[unlinkedBlock].nextBlock].previousBlock = unlinkedBlock;
	}
	else {
		memoryAllocation->lastBlock = unlinkedBlock;
	}
	
	memoryAllocation->blockList[*block].nextBlock = unlinkedBlock;
	memoryAllocation->blockList[*block].block_size = index;
	
	
	memoryAllocation->mostRecentBlock = *block;
	
	// Done! (Except for bugs.)
	
	error = dl_error_ok;
	l_cleanup:
	
	return error;
}

// // New block will be at the address (*block)->nextBlock after the function returns.
// dl_error_t dl_memory_splitBlock(dl_memoryAllocation_t *memoryAllocation, dl_ptrdiff_t *block, dl_size_t index) {
// 	dl_error_t error = dl_error_ok;
	
// 	dl_ptrdiff_t thirdBlock = memoryAllocation->blockList[*block].nextBlock;
// 	dl_ptrdiff_t secondBlock = -1;
	
// 	// Allocate memory for the block structure.
// 	error = dl_memory_pushBlockEntry(memoryAllocation, &secondBlock);
// 	if (error) {
// 		goto l_cleanup;
// 	}
	
	
// 	memoryAllocation->blockList[*block].nextBlock = secondBlock;
	
// 	memoryAllocation->blockList[secondBlock].previousBlock = *block;
// 	memoryAllocation->blockList[secondBlock].nextBlock = thirdBlock;
// 	memoryAllocation->blockList[secondBlock].block = &memoryAllocation->blockList[*block].block[index];
	
// 	if (thirdBlock != -1) {
// 		memoryAllocation->blockList[thirdBlock].previousBlock = secondBlock;
// 	}
	
	
// 	memoryAllocation->blockList[secondBlock].block_size = memoryAllocation->blockList[*block].block_size - index;
// 	memoryAllocation->blockList[*block].block_size = index;
	
	
// 	error = dl_error_ok;
// 	l_cleanup:
	
// 	return error;
// }

dl_error_t dl_malloc(dl_memoryAllocation_t *memoryAllocation, void **memory, dl_size_t size) {
	dl_error_t error = dl_error_ok;
	
	dl_size_t block = -1;
	
	if (size == 0) {
		error = dl_error_invalidValue;
		goto l_cleanup;
	}
	
	// Find a fitting block.
	error = dl_memory_findBlock(memoryAllocation, &block, size, memoryAllocation->fit);
	if (error) {
		goto l_cleanup;
	}
	
	// Split.
	if (memoryAllocation->blockList[block].block_size != size) {
		error = dl_memory_splitBlock(memoryAllocation, &block, size);
		if (error) {
			goto l_cleanup;
		}
	}
	
	// Mark allocated.
	memoryAllocation->blockList[block].allocated = dl_true;
	
	// Pass the memory.
	*memory = memoryAllocation->blockList[block].block;
	
	error = dl_error_ok;
	l_cleanup:
	
	if (error) {
		memory = dl_null;
	}
	
	return error;
}

// Sets the given pointer to zero after freeing the memory.
dl_error_t dl_free(dl_memoryAllocation_t *memoryAllocation, void **memory) {
	dl_error_t error = dl_error_ok;
	
	// Find the block.
	for (dl_ptrdiff_t i = 0; i < memoryAllocation->blockList_length; i++) {
		if (memoryAllocation->blockList[i].block == *memory) {
			if (memoryAllocation->blockList[i].allocated) {
				memoryAllocation->blockList[i].allocated = dl_false;
				error = dl_memory_mergeBlocks(memoryAllocation, &i);
				if (error) {
					goto l_cleanup;
				}
				break;
			}
			else {
				error = dl_error_danglingPointer;
				goto l_cleanup;
			}
		}
	}
	
	error = dl_error_ok;
	l_cleanup:
	
	*memory = dl_null;
	
	return error;
}
