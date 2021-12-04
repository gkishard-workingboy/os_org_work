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
// for getrlimit()
#include <time.h>
#include <sys/resource.h>

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
	struct rlimit rlim;
	union sigval sval;

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
	
	ret = getrlimit(RLIMIT_SIGPENDING, &rlim);
	if (ret < 0)
	{
		perror("failed to get limit");
		return -4;
	} else
	{
		printf("soft limit: %lu.\n", rlim.rlim_cur);
	}
	
	if (count >= rlim.rlim_cur) {
		count = rlim.rlim_cur;
		printf("count too large, limiting number of signals to soft limit.\n");
	}
	
	sval.sival_int = 0;
	for (i = 0; i < count; ++i)
	{
		ret = sigqueue(target, SIGRTMIN, sval);
		if (ret < 0)
		{
			perror("failed to send signal to target");
			return -3;
		}
	}

	sval.sival_int = 1;
	ret = sigqueue(target, SIGRTMIN, sval);
	if (ret < 0)
	{
		perror("failed to send end signal to target");
		return -3;
	}
	
	printf("target killed.\n");
	
	return 0;
	
}
