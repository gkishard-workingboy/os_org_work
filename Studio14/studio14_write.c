#include "studio14_usr_pgm.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


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
	printf("error: failed to write line %d, %s due to the following error\n",
	 line_no, line_content);
	printf("	%s\n", strerror(errno));
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
	// try to open file for writing
	int fd = -1;
	fd = open(argv[FILE_NAME], O_TRUNC|O_WRONLY);
	// open returns -1 upon failure and sets errno.
	if (fd == -1) {
		// failed to open file
		return open_file_error(argv[FILE_NAME]);
	}
	// write to the file opened
	int ret_val;
	char* endl = "\n";
	char* tmp;
	for (int index = LEAST_ARGS; index < argc; ++index) {
		tmp = argv[index];
		ret_val = write(fd, tmp, strlen(tmp));
		// write returns -1 upon failure and sets errno.
		if (ret_val == -1) {
			// failed to write line
			write_line_error(index, argv[index]);
		}
		// write end line character.
		tmp = endl;
		ret_val = write(fd, tmp, strlen(tmp));
		// write returns -1 upon failure and sets errno.
		if (ret_val == -1) {
			// failed to write line
			write_line_error(index, endl);
		}
	}
	// close the file
	ret_val = close(fd);
	// fclose returns a non-zero value upon failure and sets errno.
	if (ret_val == -1) {
		// failed to close file
		return close_file_error(argv[FILE_NAME]);
	}
	return SUCCESS;
}
