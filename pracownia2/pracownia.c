#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

/*== CONSTANTS ==*/
#define N 5
#define MAX_THINK_TIME 20000
#define MAX_EAT_TIME  100000
#define MAX_WAIT_TIME 5000

/*== MACROS ==*/
#define wait sem_wait
#define timewait sem_timedwait
#define post sem_post
#define init sem_init
#define join pthread_join
#define create pthread_create
#define iter for(i=0;i<N;i++)


/*== GLOBALS ==*/
char *names[N] = { "Arystoteles", "Platon", "Kant", "Marks", "Nietzsche"};
sem_t forks[N];
pthread_t philosopherThread[N];



void think(int pid)
{
    fprintf(stdout, "%s is thinking...\n", names[pid]);
    usleep(rand() % MAX_THINK_TIME);
}


void eat(int pid)
{
    fprintf(stdout, "%s is eating...\n", names[pid]);
    usleep(rand() % MAX_EAT_TIME);
}



void *_2bOrNot2b(void *arg)
{
    int pid = *((int *)(arg));
    int fork;
    
    struct timespec x;
    x.tv_nsec = MAX_WAIT_TIME;

    while (1) // Filozofowie biesiadują w nieskończoność
    {
        think(pid);

        fork = pid;
        wait(&forks[fork]);
        fprintf(stdout, "\t%s pick fork %d\n", names[pid], fork);
        fork = (pid+1) % N;
        if(timewait(&forks[fork], &x) == -1) {
            post(&forks[pid]);            
            continue;
        }
        fprintf(stdout, "\t%s pick fork %d\n", names[pid], fork);

        eat(pid);

        fork = (pid+1) % N;
        post(&forks[fork]);
        fprintf(stdout, "\t%s released fork %d\n", names[pid], fork);
        fork = pid;
        post(&forks[fork]);
        fprintf(stdout, "\t%s released fork %d\n", names[pid], fork);


    }

   return NULL;
}

int main(int argc, char **argv)
{

    int i;
    iter
        if (init(&forks[i], 0, 1))
            perror("ERROR: semaphore cannot be initialized");

    int arg[N];
    iter
    {
        arg[i] = i;
        if (create(&philosopherThread[i], NULL, _2bOrNot2b, &arg[i]))
            perror("ERROR: thread cannot be initialized");
    }

    iter
        join(philosopherThread[i], NULL);

    fprintf(stdout, "Dinner is done\n");
    return EXIT_SUCCESS;
}

