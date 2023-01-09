#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

union bithack {	
	float fl;
	int in;
};

struct point {
	int x, y;
};

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
			printf("%s", line);
			result = ptr - start_address + offset;
		}
    }

	free(line);
	return result;
}

int main() {
	// https://stackoverflow.com/questions/1528298/get-path-of-executable
	char resolved_path[PATH_MAX];
	int length = readlink("/proc/self/exe", resolved_path, PATH_MAX);
	resolved_path[length] = 0;

	union bithack my_bithack;
	my_bithack.fl = 2.;

	struct point my_point;
	my_point.x = 42;
	my_point.y = 69;

	// printf("Path of this program: %s\n", resolved_path);
	// printf("Float: %f, Int: %d\n", my_bithack.fl, my_bithack.in);
	// printf("x: %d, y: %d\n", my_point.x, my_point.y);
	printf("Pointer to point: %p\n", &my_point);
	printf("Pointer to point relative to start of stack: %lx\n", get_relative_address((long)&my_point));
	printf("Pointer to `main`: %p\n", &main);
	printf("Pointer to point relative to start of stack: %lx\n", get_relative_address((long)&main));
}
