#ifndef STUDIO14_READER
#define STUDIO14_READER

enum ARGV_INDEX {
	PROGRAM_NAME,
	FILE_NAME,
	NUM_ARGS,
};

enum ERROR_CODE {
	SUCCESS,
	INVALID_USAGE,
	FAILED_TO_OPEN_FILE,
	ILL_FORMATTED_LINE,
	UNEXPECTED_EOF,
	FAILED_TO_CLOSE_FILE,
};

int usage(char* pgm_name);

int open_file_error(char* file_name);

int close_file_error(char* file_name);

int main(int argc, char* argv[]);


#endif /* STUDIO14_READER */
