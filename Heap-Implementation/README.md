# Heap Implementation in C

In this project, we try to emulate memory allocation as done by the heap, using C.

## Functions Implemented:

- get_previous_block() - Find the start of the previous block in memory
- coalesce() - Coalesce a block with its consecutive block, only if both are free
- get_size_to_allocate() - Determine the size of the block we need to allocate given the size the user requested
- prepare_block_for_use() - Turn a free block into one the user can utilize
- heap_print() - Print the structure of the heap to the screen
- heap_find_avg_free_block_size() - Determine the average size of all free blocks
- heap_free() - Free a block on the heap
- malloc_first_fit() - Malloc a block on the heap, using first fit allocation policy
- malloc_best_fit() - Malloc a block on the heap, using best fit allocation policy
- malloc_next_fit() - Malloc a block on the heap, using next fit allocation policy
