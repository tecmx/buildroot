#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <linux/sched.h>

#define SCHED_LOW_IDLE 7

char *global_buffer;
char *global_ptr;
sem_t buffer_sem;
int buffer_size;
int num_threads;
int *thread_counts;

void *thread_func(void *arg)
{
  char c = *(char *)arg;
  sprintf
  display_thread_sched_attr()
  while (1)
  {
    sem_wait(&buffer_sem);
    
    if (global_ptr - global_buffer >= buffer_size)
    {
      sem_post(&buffer_sem);
      break;
    }
    *global_ptr = c;
    global_ptr++;
    thread_counts[c - 'A']++;
    sem_post(&buffer_sem);
    // sched_yield();
  }
  return NULL;
}

void summarize_buffer()
{
  char *summary = malloc(buffer_size);
  int summary_index = 0;
  for (int i = 0; i < buffer_size; i++)
  {
    if (i == 0 || global_buffer[i] != global_buffer[i - 1])
    {
      summary[summary_index++] = global_buffer[i];
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
    printf("Thread %c was scheduled %d times\n", 'A' + i, thread_counts[i]);
  }
}

display_thread_sched_attr(char *msg)
{
    int policy, s;
    struct sched_param param;

    s = pthread_getschedparam(pthread_self(), &policy, &param);
    if (s != 0)
        handle_error_en(s, "pthread_getschedparam");

    printf("%s\n", msg);
    display_sched_attr(policy, &param);
}

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    fprintf(stderr, "Usage: %s <buffer_size> <num_threads> <scheduling_policy>\n", argv[0]);
    return 1;
  }

  buffer_size = atoi(argv[1]);
  num_threads = atoi(argv[2]);
  int scheduling_policy = atoi(argv[3]);
  pthread_attr_t attr;
  struct sched_param param;

  global_buffer = malloc(buffer_size);
  global_ptr = global_buffer;
  thread_counts = calloc(num_threads, sizeof(int));

  sem_init(&buffer_sem, 0, 0);

  pthread_t threads[num_threads];
  char thread_chars[num_threads];
  for (int i = 0; i < num_threads; i++)
  {
    thread_chars[i] = 'A' + i;
    // Inicializa os atributos da thread
    pthread_attr_init(&attr);

    // Define a política de escalonamento para SCHED_FIFO
    scheduling_policy = i % 2 ? SCHED_RR : SCHED_LOW_IDLE;
    printf("Thread[%c]: %d\n", 'A' + i, scheduling_policy);
    pthread_attr_setschedpolicy(&attr, scheduling_policy);

    // Define a prioridade da thread
    param.sched_priority = sched_get_priority_max(scheduling_policy);
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&threads[i], &attr, thread_func, &thread_chars[i]);
    // pthread_setschedparam(threads[i], scheduling_policy, &param);
  }

  for (int i = 0; i <= num_threads; i++)
  {
    sem_post(&buffer_sem);
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  printf("Global Buffer: %s\n", global_buffer);
  summarize_buffer();
  print_thread_counts();

  free(global_buffer);
  free(thread_counts);
  sem_destroy(&buffer_sem);

  return 0;
}