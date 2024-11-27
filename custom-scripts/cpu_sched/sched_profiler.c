#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <linux/sched.h>
#include <errno.h>

#define SCHED_LOW_IDLE 7

char *global_buffer;
char *global_ptr;
sem_t buffer_sem;
int buffer_size;
int num_threads;
int *thread_counts;
pthread_t *threads;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int counter = 0;

typedef struct {
    int index;
    char c;
    int num_threads;
    int sched_policy
} Thread_args;

void* thread_function(void* arg) {
    Thread_args* data = (Thread_args*)arg;

    int newPolicy = data->sched_policy;
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(newPolicy);
    sched_setscheduler(0, newPolicy, &param);

    printf("Thread %i | policy: %d | priority: %d\n", data->index, newPolicy, param.sched_priority);


    pthread_mutex_lock(&mutex);
    counter++;
    if (counter == data->num_threads) {
        // All threads have arrived; signal all waiting threads
        printf("Thread %c: All threads have arrived. Broadcasting signal.\n", data->c);
        pthread_cond_broadcast(&cond);
        // sem_post(&buffer_sem);
    } else {
        // Wait until the broadcast signal is received
        printf("Thread %c: Waiting for other threads.\n", data->c);
        while (counter < num_threads) {
            pthread_cond_wait(&cond, &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);

    // All threads proceed from here
    int policy;
    pthread_getschedparam(threads[data->index], &policy, &param);
    printf("Thread %c: Proceeding after broadcast. POLICY: %i\n", data->c, policy);

    while (1)
    {
        sleep(1);
        sem_wait(&buffer_sem);

        if (global_ptr - global_buffer >= buffer_size)
        {
            sem_post(&buffer_sem);
            break;
        }
        *global_ptr = data->c;
        global_ptr++;
        thread_counts[data->index]++;
        sem_post(&buffer_sem);
    }
    printf("ACABOU [%c]\n", data->c);

    return NULL;
}


void summarize_buffer()
{
  char *summary = malloc(buffer_size);
  int summary_index = 0;

  for (int i = 0; i < num_threads; i++){
    thread_counts[i] = 0;
  }
  
  for (int i = 0; i < buffer_size; i++)
  {
    if (i == 0 || global_buffer[i] != global_buffer[i - 1])
    {
      summary[summary_index++] = global_buffer[i];
      thread_counts[global_buffer[i]-'A']++;
    }
  }
  summary[summary_index] = '\0';
  printf("Summarized Buffer: %s\n", summary);
  
  free(summary);
}

void print_thread_counts()
{
  for (int i = 0; i < num_threads; i++)
  {
    printf("Thread %c (%d) was scheduled %d times\n", 'A' + i, i, thread_counts[i]);
  }
}

int main(int argc, char *argv[]) {
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <buffer_size> <num_threads> <scheduling_policy>\n", argv[0]);
        return 1;
    }

    buffer_size = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    int first_policy = atoi(argv[3]);
    int second_policy = atoi(argv[4]);
    char thread_chars[num_threads];

    global_buffer = malloc(buffer_size);
    global_ptr = global_buffer;
    thread_counts = calloc(num_threads, sizeof(int));
    threads = calloc(num_threads, sizeof(pthread_t));

    sem_init(&buffer_sem, 0, 1);

    // Create threads
    for (long i = 0; i < num_threads; i++) {
        int scheduling_policy;
        thread_chars[i] = 'A' + i;
        Thread_args* data = (Thread_args*)malloc(sizeof(Thread_args));

        //   scheduling_policy = SCHED_FIFO;
        if (i%2 == 0) {
          scheduling_policy = first_policy;
        } else {
          scheduling_policy = second_policy;

        }

        data->index = i;
        data->c = thread_chars[i];
        data->num_threads = num_threads;
        data->sched_policy = scheduling_policy;

        if (pthread_create(&threads[i], NULL, thread_function, data) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // printf("Global Buffer: %s\n", global_buffer);
    summarize_buffer();
    print_thread_counts();

    // Clean up
    free(global_buffer);
    free(thread_counts);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    sem_destroy(&buffer_sem);

    printf("All threads have finished execution.\n");
    return 0;
}
