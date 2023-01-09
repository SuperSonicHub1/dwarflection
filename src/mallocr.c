#include <stdlib.h>
#include <errno.h>
#include "monads.h"
#include "mallocr.h"

extern int errno;

// https://pubs.opengroup.org/onlinepubs/009695399/functions/malloc.html
struct malloc_result mallocr(size_t size) {
	struct malloc_result result;

	void* allocation = malloc(size);
	if (size != 0 && allocation == NULL) ERR(result, errno);
	else OK(result, allocation);

	return result;
}

// https://pubs.opengroup.org/onlinepubs/009695399/functions/realloc.html
struct realloc_result reallocr(void *ptr, size_t size) {
	struct realloc_result result;

	void* allocation = realloc(ptr, size);
	if (allocation == NULL && (size != 0 || errno == ENOMEM)) ERR(result, errno);
	else OK(result, allocation);

	return result;
}
