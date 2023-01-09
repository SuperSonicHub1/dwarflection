#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "procmap.h"

// https://www.geeksforgeeks.org/c-program-to-read-contents-of-whole-file/
// https://linux.die.net/man/3/getline
// https://linux.die.net/man/5/proc
// https://github.com/jswrenn/deflect/blob/b7c60ce053d5d2b29957e848ed902d702f461017/src/lib.rs#L161-L176
// 7ffc7b341000-7ffc7b343000 r-xp 00000000 00:00 0                          [vdso]
long get_relative_address(long ptr) {
	long start_address, end_address, offset, result;

	FILE* file;
	char* line;
	size_t len = 0;
	ssize_t read;
	
	file = fopen("/proc/self/maps", "r");
	if (file == NULL) {
		printf("Something is horribly wrong with Linux.");
		return -1;
	}

	while ((read = getline(&line, &len, file)) != -1) {
		char* line_ptr;
		// Read start
		start_address = strtol(line, &line_ptr, 16);
		// Skip dash
		line_ptr++;
		end_address = strtol(line_ptr, &line_ptr, 16);
		// Skip permissions
		line_ptr += 6;
		offset = strtol(line_ptr, NULL, 16);

		if (start_address <= ptr && ptr < end_address) {
			result = ptr - start_address + offset;
		}
    }

	free(line);
	return result;
}

int main() {
	int a = 5;
	printf("Int: %d\n", a);
	printf("Pointer: %p\n", &a);
	printf("Relative pointer: %lx\n", get_relative_address((long)&a));
}
