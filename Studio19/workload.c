#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


#define SUCCESS 0
#define FAILURE -1

enum args
{
	PROGRAM_NAME,
	CPU_NO,
	EXPECTED_ARGC
};

int main(int argc, char* argv[])
{
	int ret;
	int cpuno;
	cpu_set_t set;

	// validate input.
	if (argc != EXPECTED_ARGC) {
		fprintf(stderr, "usage: %s cpuno\n", argv[PROGRAM_NAME]);
		return FAILURE;
	}
	cpuno = atoi(argv[CPU_NO]);

	// set mask.
	CPU_ZERO(&set);
	CPU_SET(cpuno, &set);

	// set affinity.
	ret = FAILURE;
	sched_setaffinity(getpid(), sizeof(set), &set) ;
	if (ret < SUCCESS) perror("failed to set affinity");
	
	while (1)
	{
	}
	
	return SUCCESS;
}
