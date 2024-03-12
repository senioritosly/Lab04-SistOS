#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_FILOSOFOS 5

pthread_mutex_t tenedores[NUM_FILOSOFOS];
sem_t mutex;


void *filosofo(void *arg) {
    int id = *((int*)arg);
    int tenedor_izq = id;
    int tenedor_der = (id + 1) % NUM_FILOSOFOS;

    while (1) {

        printf("Filósofo %d pensando\n", id);
        usleep(rand() % 3000000);

        sem_wait(&mutex);

        pthread_mutex_lock(&tenedores[tenedor_izq]);
        printf("Filósofo %d tomó el tenedor izquierdo (%d)\n", id, tenedor_izq);
        pthread_mutex_lock(&tenedores[tenedor_der]);
        printf("Filósofo %d tomó el tenedor derecho (%d)\n", id, tenedor_der);

        sem_post(&mutex);

        printf("Filósofo %d comiendo\n", id);
        usleep(rand() % 3000000);

        pthread_mutex_unlock(&tenedores[tenedor_izq]);
        printf("Filósofo %d soltó el tenedor izquierdo (%d)\n", id, tenedor_izq);
        pthread_mutex_unlock(&tenedores[tenedor_der]);
        printf("Filósofo %d soltó el tenedor derecho (%d)\n", id, tenedor_der);
    }
}

int main() {

    pthread_t filosofos[NUM_FILOSOFOS];
    int ids[NUM_FILOSOFOS];

    sem_init(&mutex, 0, 1);

    for (int i = 0; i < NUM_FILOSOFOS; ++i) {
        pthread_mutex_init(&tenedores[i], NULL);
    }

    for (int i = 0; i < NUM_FILOSOFOS; ++i) {
        ids[i] = i;
        if (pthread_create(&filosofos[i], NULL, filosofo, &ids[i]) != 0) {
            perror("Error al crear el hilo del filósofo");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_FILOSOFOS; ++i) {
        pthread_join(filosofos[i], NULL);
    }

    sem_destroy(&mutex);

    for (int i = 0; i < NUM_FILOSOFOS; ++i) {
        pthread_mutex_destroy(&tenedores[i]);
    }

    return 0;
}