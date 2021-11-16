#include "studio14_reader.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int usage(char* pgm_name) {
	printf("usage: %s file_name\n", pgm_name);
	return INVALID_USAGE;
}

int open_file_error(char* file_name) {
	printf("error: failed to open file %s due to the following error\n",
	 file_name);
	printf("	%s\n", strerror(errno));
	return FAILED_TO_OPEN_FILE;
}

int close_file_error(char* file_name) {
	printf("error: failed to close file %s due to the following error\n",
	 file_name);
	printf("	%s\n", strerror(errno));
	return FAILED_TO_CLOSE_FILE;
}

int main(int argc, char* argv[]) {
	// guard invalid number of arguments.
	if (argc != NUM_ARGS) {
		return usage(argv[PROGRAM_NAME]);
	}
	// try to open file for appending
	FILE* fp = NULL;
	fp = fopen(argv[FILE_NAME], "r");
	// fopen returns NULL upon failure and sets errno.
	if (fp == NULL) {
		// failed to open file
		return open_file_error(argv[FILE_NAME]);
	}
	// read from file
	char char_len = '\0';
	unsigned int current_len = 0;
	unsigned int max_len = 0;
	char* line_buf = 0;
	// gets the line width for the first line.
	char_len = fgetc(fp);
	// 255 is the end of file
	while (char_len != 255) {
		// convert the current length from char to unsigned int.
		// 2 is added to account for the end line character.
		current_len = char_len - '0' + 2;
		// check len
		if (current_len > 11) {
			printf("error: ill formatted line, unknown length %c.\n", char_len);
			return ILL_FORMATTED_LINE;
		}
		if (current_len > max_len) {
			// update max len
			max_len = current_len;
			// allocate a larger buffer
			if (line_buf) {
				// initialized, call realloc.
				line_buf = (char*)realloc(line_buf, max_len);
			}
			else {
				// uninitialized, call malloc.
				line_buf = (char*)malloc(max_len);
			}
		}
		// skip the space
		char_len = fgetc(fp);
		if (char_len != ' ') {
			printf("error: ill formatted line, expected space, was %c.\n", char_len);
			return ILL_FORMATTED_LINE;
		}
		// read in content including the end line character.
		line_buf = fgets(line_buf, current_len, fp);
		if (line_buf == NULL) {
			printf("error: unexpected end of file.\n");
			return UNEXPECTED_EOF;
		
		}
		// print the contents, with a maximum of current_len characters.
		printf("%.*s", current_len, line_buf);
		// gets the line width for next line.
		char_len = fgetc(fp);
	}
	// close the file
	int ret_val = fclose(fp);
	// fclose returns a non-zero value upon failure and sets errno.
	if (ret_val != SUCCESS) {
		// failed to close file
		return close_file_error(argv[FILE_NAME]);
	}
	return SUCCESS;
}
