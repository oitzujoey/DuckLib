
#ifndef DUCKLIB_CORE_H
#define DUCKLIB_CORE_H

typedef unsigned char dl_bool_t;
#define dl_false ((dl_bool_t) 0)
#define dl_true ((dl_bool_t) 1)

typedef unsigned long long dl_size_t;
typedef long long dl_ptrdiff_t;

#define dl_null ((void *) 0)

typedef enum {
	dl_error_ok = 0,
	dl_error_invalidValue,
	dl_error_bufferOverflow,
	dl_error_nullPointer,
	dl_error_danglingPointer,
	dl_error_outOfMemory,
	dl_error_shouldntHappen,
	dl_error_cantHappen,
} dl_error_t;

extern const char *dl_errorString[];

dl_error_t dl_memcopy(void *destination, dl_size_t destination_size, void *source, dl_size_t source_size);

#endif // DUCKLIB_CORE
