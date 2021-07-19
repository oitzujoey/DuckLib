
#include "core.h"

const char *dl_errorString[] = {
	"dl_error_ok",
	"dl_error_invalidValue",
	"dl_error_bufferOverflow",
	"dl_error_nullPointer",
	"dl_error_danglingPointer",
	"dl_error_outOfMemory",
	"dl_error_shouldntHappen",
	"dl_error_cantHappen"
};

dl_error_t dl_memcopy(void *destination, dl_size_t destination_size, void *source, dl_size_t source_size) {
	dl_error_t error = dl_error_ok;

	if (destination_size < source_size) {
		error = dl_error_bufferOverflow;
		goto l_cleanup;
	}

	if (destination > source) {
		for (char *d = destination + source_size - 1, *s = source + source_size - 1; s >= (char *) source; --d, --s) {
			*d = *s;
		}
	}
	else if (destination < source) {
		for (char *d = destination, *s = source; s < (char *) source + source_size; d++, s++) {
			*d = *s;
		}
	}

	error = dl_error_ok;
	l_cleanup:

	return error;
}
