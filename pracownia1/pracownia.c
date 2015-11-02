#include <stdio.h>
#include <pthread.h>
#include <string.h>

pthread_t tid[2];

void* drinkBeerMyThread(void *arg)
{
    int i;
    char* thread_name;

    thread_name = pthread_equal(pthread_self(),tid[0]) ? "first" : "second"; 

    i = 100;
    while(i--)
        printf("%s has %d bottles of beer on the wall, %d bottles of beer.\nTake one down and pass it around, %d bottles of beer on the wall.\n\n", thread_name, i, i, i-1);

    printf("\nNo bottles left! in thread %s \n", thread_name);

    return NULL;
}

int main(void)
{
    int i = 0;
    int error;

    while(i < 2)
    {
        error = pthread_create(&(tid[i]), NULL, &drinkBeerMyThread, NULL);
        if (!error)
            printf("\nCan't create thread :[%s]", strerror(error));
        else
            printf("\nThread %d, created successfully\n", i);

        i++;
    }

    sleep(2); // give some time for get things done
    return 0;
}
