#include <stdio.h>
#include <unistd.h>

int main() {
	pid_t nr;
	nr = getpid();
	printf("PID: %d\n", nr);

        while (1) {
		// spin indefinitely
	}

	return 0;
}
