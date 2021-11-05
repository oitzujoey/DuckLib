
#ifndef DUCKLIB_CORE_H
#define DUCKLIB_CORE_H

typedef unsigned char dl_bool_t;
#define dl_false ((dl_bool_t) 0)
#define dl_true ((dl_bool_t) 1)

typedef unsigned long long dl_size_t;
typedef long long dl_ptrdiff_t;

typedef unsigned char dl_uint8_t;
typedef unsigned short dl_uint16_t;

#define DL_UINT8_MAX    255U
#define DL_UINT16_MAX   65535U

#define dl_null ((void *) 0)

typedef enum {
	dl_error_ok = 0,
	dl_error_invalidValue,
	dl_error_bufferUnderflow,
	dl_error_bufferOverflow,
	dl_error_nullPointer,
	dl_error_danglingPointer,
	dl_error_outOfMemory,
	dl_error_shouldntHappen,
	dl_error_cantHappen,
} dl_error_t;

extern const char *dl_errorString[];

#define dl_max(a,b) ((a > b) ? (a) : (b))
#define dl_min(a,b) ((a < b) ? (a) : (b))

dl_error_t dl_memcopy(void *destination, const void *source, dl_size_t size);
void dl_memcopy_noOverlap(void *destination, const void *source, dl_size_t size);

void dl_memclear(void *destination, dl_size_t size);

void dl_strlen(dl_size_t *length, const char *string);
#define DL_STR(DL_STR_string) DL_STR_string, (sizeof(DL_STR_string) - 1)

#endif // DUCKLIB_CORE
