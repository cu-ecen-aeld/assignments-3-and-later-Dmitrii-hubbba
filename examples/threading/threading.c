#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    struct thread_data* args = (struct thread_data*) thread_param;
    args->thread_complete_success = false;

    usleep(args->wait_to_obtain * 1000);

    pthread_mutex_lock((args->mutex));

    usleep(args->wait_to_release * 1000);

    pthread_mutex_unlock(args->mutex);

    args->thread_complete_success = true;

/*
    struct thread_data* data = (struct thread_data *) thread_param;
	data->thread_complete_success = false;
	usleep(data->wait_to_obtain * 1000);
	pthread_mutex_lock(data->mutex);
	usleep(data->wait_to_release * 1000);
	pthread_mutex_unlock(data->mutex);
	data->thread_complete_success = true;
*/
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    int iret;

   // *mutex = PTHREAD_MUTEX_INITIALIZER;

    struct thread_data *data = malloc(sizeof(struct thread_data));
    //data->wait_to_obtain  = (int*)malloc(sizeof(int*));
    //data->wait_to_release = (int*)malloc(sizeof(int));//wait_to_release_ms;
 //  data->mutex           = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));//mutex;

   data->wait_to_obtain  = wait_to_obtain_ms;
   data->wait_to_release = wait_to_release_ms;
   data->mutex = mutex;

 // pthread_mutex_init(data->mutex, NULL);

   //pthread_mutex_t new_mutex = PTHREAD_MUTEX_INITIALIZER;

   //pthread_t main_thread;
   iret = pthread_create(thread, NULL, threadfunc, (void*)data);
   //iret =  pthread_join(*thread, NULL);
   //DEBUG_LOG("PISS %d", iret);
   if (iret) {
    free(data);
    return false;
  }

  // bool result = (iret == 0) ? true : false;

   return true;
}

