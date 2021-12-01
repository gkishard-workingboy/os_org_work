/*                                                    */
/*                   receiver.c                       */
/*                                                    */
/*  A signal receiver program for the Signals studio  */
/*  in CSE 422S at Washington University in St. Louis */
/*                                                    */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
// for pid_t and kill()
#include <sys/types.h>
// for atoi()
#include <stdlib.h>
#include <string.h>

enum args_t
{
	ARG_NAME,
	ARG_PID,
	ARG_NUM,
	EXPECTED_ARGC,
};

int main (int argc, char* argv[])
{

	pid_t target;
	int count, i;
	int ret;

	// Check the number of arguments
    if (argc != EXPECTED_ARGC)
    {
		printf("usage: %s <pid> <count>\n", argv[ARG_NAME]);
		return -1;
	}

	target = atoi(argv[ARG_PID]);
	count = atoi(argv[ARG_NUM]);

	if (count <= 0)
	{
		printf("unexpected zero or negative count: %d\n", count);
		return -2;
	}

	for (i = 0; i < count; ++i)
	{
		ret = kill(target, SIGUSR1);
		if (ret < 0)
		{
			perror("failed to kill target");
			return -3;
		}
	}

	ret = kill(target, SIGUSR2);
	if (ret < 0)
	{
		perror("failed to kill target");
		return -3;
	}
	
	printf("target killed.\n");
	
    return 0;
    
}
