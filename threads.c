#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

void *do_math(void *arg);
void *print_producer(void *arg);
void *print_consumer(void *arg);
void *produce(void *arg);
void *consume(void *arg);

int A;
int B;
const int sizeofbuffer = 4;
sem_t A_lock;
sem_t B_lock;
sem_t s_lock;
sem_t n_lock;
sem_t e_lock;
int buffer[3];
int n;
int e;

int main(int argc, char **argv) {
    int num_threads;
    pthread_t *threads;
    A = 0;
    B = 0;
    n= 0;
    e = sizeofbuffer;
    
    if (argc < 2) {
        printf("Usage: %s [NUM ITERATIONS]\n", argv[0]);
        return 0;
    }
    
    num_threads = atoi(argv[1]);
    threads = (pthread_t*) calloc(num_threads,sizeof(pthread_t));
    
    if (sem_init(&A_lock, 0, 1) < 0) {
        perror("Error initializing A_lock");
        return 0;
    }
    
    if (sem_init(&B_lock, 0, 1) < 0) {
        perror("Error initializing B_lock");
        return 0;
    }
    
    if (sem_init(&s_lock, 0, 1) < 0) {
        perror("Error initializing s_lock");
        return 0;
    }
    
    if (sem_init(&n_lock, 0, n) < 0) {
        perror("Error initializing n_lock");
        return 0;
    }
    
    if (sem_init(&e_lock, 0, e) < 0) {
        perror("Error initializing e_lock");
        return 0;
    }
    
    printf("Launching %d threads\n", num_threads);
    for (int i=0; i<num_threads; i++) {
        long thread_id = (long)i;
        if(pthread_create(&threads[i], NULL, &print_consumer, (void *)thread_id) !=0) {
            perror("Error");
        }
        i++;
        thread_id = (long)i;
        if(pthread_create(&threads[i], NULL, &print_producer, (void *)thread_id) !=0) {
            perror("Error");
        }
    }
    
    for (int i=0; i<num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    //here
    for (int i=0; i<num_threads; i++) {
        long thread_id = (long)i;
        if(pthread_create(&threads[i], NULL, &consume, (void *)thread_id) !=0) {
            perror("Error");
        }
        i++;
        thread_id = (long)i;
        if(pthread_create(&threads[i], NULL, &produce, (void *)thread_id) !=0) {
            perror("Error");
        }
    }
    
    for (int i=0; i<num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("A = %d\n", A);
    printf("B = %d\n", B);
    
    free(threads);
    
}
/*
void *do_math(void *arg) {
    int t_id = (int)(long)arg;
    int tmp;
    printf("Thread %d launched ...\n", t_id);
    for (int n=0; n < 100; n++) {
        sem_wait(&sum_lock);
        tmp = sum;
        usleep(5);
        tmp++;
        sum = tmp;
        sem_post(&sum_lock);
    }
}
*/

void *print_producer(void *arg) {
    int t_id = (int)(long)arg;
    int tmp;
    printf("Producer %d launched ...\n", t_id);
    for (int n=0; n < 100; n++) {
        sem_wait(&A_lock);
        tmp = A;
        usleep(5);
        tmp++;
        A = tmp;
        sem_post(&A_lock);
        sem_wait(&B_lock);
        tmp = B;
        usleep(5);
        tmp = tmp + 3;
        B = tmp;
        sem_post(&B_lock);
    }
}

void *print_consumer(void *arg) {
    int t_id = (int)(long)arg;
    int tmp;
    printf("Consumer %d launched ...\n", t_id);
    for (int n=0; n < 100; n++) {
        sem_wait(&B_lock);
        tmp = B;
        usleep(5);
        tmp = tmp + 3;
        B = tmp;
        sem_post(&B_lock);
        sem_wait(&A_lock);
        tmp = A;
        usleep(5);
        tmp++;
        A = tmp;
        sem_post(&A_lock);
    }
}

void *produce(void *arg) {
    int t_id = (int)(long)arg;
    for (int i=0; i < 20; i++) {
    int random;
    random = rand() % 100;
    printf("Producer %d: ", t_id);
    printf("produced %d\n", random);
        sem_wait(&e_lock);
        sem_wait(&s_lock);
        buffer[i] = random;
        sem_post(&s_lock);
        sem_post(&n_lock);
    }
}

void *consume(void *arg) {
    int t_id = (int)(long)arg;
    int take;
    for (int i=0; i <20; i++) {
        sem_wait(&n_lock);
        sem_wait(&s_lock);
        take = buffer[i];
        sem_post(&s_lock);
        sem_post(&e_lock);
        printf("Consumer %d: ", t_id);
        printf("consumed %d\n", take);
    }
}