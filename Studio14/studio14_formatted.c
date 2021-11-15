#include "studio14_usr_pgm.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

int usage(char* pgm_name) {
	printf("usage: %s file_name [string(s)]\n", pgm_name);
	return INVALID_USAGE;
}

int open_file_error(char* file_name) {
	printf("error: failed to open file %s due to the following error\n",
	 file_name);
	printf("	%s\n", strerror(errno));
	return FAILED_TO_OPEN_FILE;
}

int write_line_error(int line_no, char* line_content) {
	printf("error: failed to write line %d, %s, aborting.\n",
	 line_no, line_content);
	return FAILED_TO_WRITE_LINE;
}

int close_file_error(char* file_name) {
	printf("error: failed to close file %s due to the following error\n",
	 file_name);
	printf("	%s\n", strerror(errno));
	return FAILED_TO_CLOSE_FILE;
}

int main(int argc, char* argv[]) {
	// guard invalid number of arguments.
	if (argc < LEAST_ARGS) {
		return usage(argv[PROGRAM_NAME]);
	}
	// try to open file for appending
	FILE* fp = NULL;
	fp = fopen(argv[FILE_NAME], "a");
	// fopen returns NULL upon failure and sets errno.
	if (fp == NULL) {
		// failed to open file
		return open_file_error(argv[FILE_NAME]);
	}
	// write to the file opened
	int ret_val;
	char* tmp;
	for (int index = LEAST_ARGS; index < argc; ++index) {
		tmp = argv[index];
		ret_val = fprintf(fp, "%d %s\n", strlen(tmp), tmp);
		// fprintf returns a negative value upon failure.
		if (ret_val < SUCCESS) {
			// failed to write line
			write_line_error(index, tmp);
		}
	}
	// close the file
	ret_val = fclose(fp);
	// fclose returns a non-zero value upon failure and sets errno.
	if (ret_val != SUCCESS) {
		// failed to close file
		return close_file_error(argv[FILE_NAME]);
	}
	return SUCCESS;
}
