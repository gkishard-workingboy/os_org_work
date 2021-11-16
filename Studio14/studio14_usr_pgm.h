#ifndef STUDIO14_USR_PGM
#define STUDIO14_USR_PGM

enum ARGV_INDEX {
	PROGRAM_NAME,
	FILE_NAME,
	LEAST_ARGS,
};

enum ERROR_CODE {
	SUCCESS,
	INVALID_USAGE,
	FAILED_TO_OPEN_FILE,
	FAILED_TO_WRITE_LINE,
	FAILED_TO_CLOSE_FILE,
};

int usage(char* pgm_name);

int open_file_error(char* file_name);

int write_line_error(int line_no, char* line_content);

int close_file_error(char* file_name);

int main(int argc, char* argv[]);

#endif /* STUDIO14_USR_PGM */
