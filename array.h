#ifndef DUCKLIB_ARRAY_H
#define DUCKLIB_ARRAY_H

#include "core.h"
#include "memory.h"

typedef enum {
	dl_array_strategy_fit,
	dl_array_strategy_double
} dl_array_strategy_t;

typedef struct {
	void *elements;
	dl_size_t element_size;
	dl_size_t elements_length;
	dl_size_t elements_memorySize;
	dl_array_strategy_t strategy;
	dl_memoryAllocation_t *memoryAllocation;
} dl_array_t;

void dl_array_init(dl_array_t *array, dl_memoryAllocation_t *memoryAllocation, dl_size_t element_size, dl_array_strategy_t strategy);
dl_error_t dl_array_quit(dl_array_t *array);
dl_error_t dl_array_pushElement(dl_array_t *array, void *element);
dl_error_t dl_array_pushElements(dl_array_t *array, void *elements, dl_size_t elements_length);
// dl_error_t dl_array_popElement(dl_array_t *array, void **element, dl_ptrdiff_t index);
dl_error_t dl_array_popElement(dl_array_t *array, void *element);
dl_error_t dl_array_getTop(dl_array_t *array, void *element);
dl_error_t dl_array_get(dl_array_t *array, void *element, dl_ptrdiff_t index);
dl_error_t dl_array_set(dl_array_t *array, const void *element, dl_ptrdiff_t index);

#endif // DUCKLIB_ARRAY_H
