/******************************************************************************
* 
* receiver.c
*
* A signal receiver program for the Signals studio
* in CSE 422S at Washington University in St. Louis
* 
* Written by Marion Sudvarg
* Last modified January 18, 2021
* 
******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t done = 0;
volatile int count = 0;

void sig_handler( int signum ){

    if (signum == SIGUSR1) {
        count++;
    }

    if (signum == SIGUSR2) {
        done = 1;
    }
}

int main (int argc, char* argv[]){

    struct sigaction ss;

    ss.sa_handler = sig_handler;
    ss.sa_flags = SA_RESTART;

    sigaction( SIGUSR1, &ss, NULL );
    sigaction( SIGUSR2, &ss, NULL );

    printf("pid: %d\n", getpid());

    while (!done) {}

    printf("SIGUSR1 received %d times\n", count);

    return 0;
}

