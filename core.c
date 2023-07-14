
#ifdef USE_STDLIB
#include <string.h>
#endif /* USE_STDLIB */
#include "core.h"

const char *dl_errorString[] = {
	"dl_error_ok",
	"dl_error_invalidValue",
	"dl_error_bufferUnderflow",
	"dl_error_bufferOverflow",
	"dl_error_nullPointer",
	"dl_error_danglingPointer",
	"dl_error_outOfMemory",
	"dl_error_shouldntHappen",
	"dl_error_cantHappen"
};

#ifdef USE_STDLIB
dl_error_t dl_memcopy(void *destination, const void *source, dl_size_t size) {
	(void) memmove(destination, source, size);
	return dl_error_ok;
}
#else /* USE_STDLIB */
dl_error_t dl_memcopy(void *destination, const void *source, dl_size_t size) {
	dl_error_t error = dl_error_ok;
	const char *s;

	if (destination > source) {
		s = (char*)source + size - 1;
		for (char *d = (char*)destination + size - 1; s >= (char *) source; --d, --s) {
			*d = *s;
		}
	}
	else if (destination < source) {
		s = source;
		for (char *d = destination; s < (char *) source + size; d++, s++) {
			*d = *s;
		}
	}

	error = dl_error_ok;
//	l_cleanup:

	return error;
}
#endif /* USE_STDLIB */

void dl_memcopy_noOverlap(void *destination, const void *source, const dl_size_t size) {
	const char *s;

	s = source;
	for (char *d = destination; s < (char *) source + size; d++, s++) {
		*d = *s;
	}
}

#ifdef USE_STDLIB
void dl_memclear(void *destination, dl_size_t size) {
	(void) memset(destination, 0, size);
}
#else /* USE_STDLIB */
void dl_memclear(void *destination, dl_size_t size) {
	for (dl_ptrdiff_t i = 0; (dl_size_t) i < size; i++) {
		((unsigned char *) destination)[i] = 0;
	}
}
#endif /* USE_STDLIB */

void dl_strlen(dl_size_t *length, const char *string) {
	dl_size_t i = 0;
	while (string[i] != '\0') {
		i++;
	}
	*length = i;
}

char dl_nybbleToHexChar(unsigned char i) {
	i &= 0xFU;
	if (i < 10) {
		return i + '0';
	}
	else {
		return i - 10 + 'A';
	}
}
