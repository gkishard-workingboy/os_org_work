/******************************************************************************
* 
* print500000.c
*
* Prints numbers 1 to 500000 in order.
* 
* A simple program serving as a starting point
* for exploring several signal-handling concepts.
*
* Modified January 18, 2021 by Marion Sudvarg
* from a similar program used in previous semesters
* of CSE 422S at Washington University in St. Louis
* 
******************************************************************************/

#include <stdio.h>
#include <signal.h> 
// for write()
#include <unistd.h>
// for strlen()
#include <string.h>

#define size 500000

volatile int i;
volatile sig_atomic_t should_terminate;

int called [size];

static void handler(int sig, siginfo_t *si, void *unused)
{
        char* message = "Caught SIGINT.\n";
        int message_len = strlen(message);
        write(STDOUT_FILENO, message, message_len);
        
        called[i] = 1;
        // should_terminate = 1;
        
}

int main (int argc, char* argv[])
{

        /// ref: man 2 mprotect
        struct sigaction sa;

        should_terminate = 0;

        sa.sa_sigaction = handler;
        sa.sa_flags = SA_RESTART;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGINT, &sa, NULL) == -1)
                perror("sigaction");

        for(i = 0; i < size; i++){
                printf("i: %d\n", i);
                if (should_terminate)
                        break;
        }


        for(i = 0; i < size; i++){
                if( called[i] == 1 )
                        printf("%d was possibly interrupted\n", i);
        }

        return 0;
}
