#ifndef DUCKLIB_ARRAY_H
#define DUCKLIB_ARRAY_H

#include "core.h"
#include "memory.h"

typedef enum {
	array_strategy_fit,
	array_strategy_double
} array_strategy_t;

typedef struct {
	void *elements;
	dl_size_t element_size;
	dl_size_t elements_length;
	dl_size_t elements_memorySize;
	array_strategy_t strategy;
	dl_memoryAllocation_t *memoryAllocation;
} array_t;

void array_init(array_t *array, dl_memoryAllocation_t *memoryAllocation, dl_size_t element_size, array_strategy_t strategy);
dl_error_t array_quit(array_t *array);
dl_error_t array_pushElement(array_t *array, void *element);
dl_error_t array_pushElements(array_t *array, void *elements, dl_size_t elements_length);
// dl_error_t array_popElement(array_t *array, void **element, dl_ptrdiff_t index);
dl_error_t array_popElement(array_t *array, void *element);
dl_error_t array_getTop(array_t *array, void *element);
dl_error_t array_get(array_t *array, void *element, dl_ptrdiff_t index);
dl_error_t array_set(array_t *array, const void *element, dl_ptrdiff_t index);

#endif // DUCKLIB_ARRAY_H
