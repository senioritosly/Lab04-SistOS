#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_PROCESOS 5
#define NUM_RECURSOS 3

int recursos_disponibles[NUM_RECURSOS] = {10, 5, 7};
int maximo[NUM_PROCESOS][NUM_RECURSOS] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
};


int asignado[NUM_PROCESOS][NUM_RECURSOS] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
};


int necesidad[NUM_PROCESOS][NUM_RECURSOS];
pthread_mutex_t mutex;


void inicializar_necesidades() {
    for (int i = 0; i < NUM_PROCESOS; ++i) {
        for (int j = 0; j < NUM_RECURSOS; ++j) {
            necesidad[i][j] = maximo[i][j] - asignado[i][j];
        }
    }
}


bool es_asignacion_segura(int proceso, int solicitud[]) {
    for (int j = 0; j < NUM_RECURSOS; ++j) {
        if (solicitud[j] > necesidad[proceso][j]) {
            return false;
        }
    }

    for (int j = 0; j < NUM_RECURSOS; ++j) {
        if (solicitud[j] > recursos_disponibles[j]) {
            return false;
        }
    }

    return true;
}


void asignar_recursos(int proceso, int solicitud[]) {
    for (int j = 0; j < NUM_RECURSOS; ++j) {
        asignado[proceso][j] += solicitud[j];
        recursos_disponibles[j] -= solicitud[j];
        necesidad[proceso][j] -= solicitud[j];
    }
}

void liberar_recursos(int proceso) {
    for (int j = 0; j < NUM_RECURSOS; ++j) {
        recursos_disponibles[j] += asignado[proceso][j];
        asignado[proceso][j] = 0;
        necesidad[proceso][j] = 0;
    }
}

void* solicitar_recursos(void* arg) {
    int proceso = *((int*)arg);

    int solicitud[NUM_RECURSOS];
    for (int j = 0; j < NUM_RECURSOS; ++j) {
        solicitud[j] = rand() % (maximo[proceso][j] + 1);
    }

    pthread_mutex_lock(&mutex);

    printf("Proceso %d solicita recursos: ", proceso);
    for (int j = 0; j < NUM_RECURSOS; ++j) {
        printf("%d ", solicitud[j]);
    }

    printf("\n");

    if (es_asignacion_segura(proceso, solicitud)) {
        asignar_recursos(proceso, solicitud);
        printf("Asignación segura para el proceso %d\n", proceso);
    } else {
        printf("Solicitud denegada para el proceso %d (podría conducir a un estado inseguro)\n", proceso);
    }

    pthread_mutex_unlock(&mutex);
    usleep(rand() % 2000000);

    pthread_mutex_lock(&mutex);
    liberar_recursos(proceso);
    printf("Recursos liberados por el proceso %d\n", proceso);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t procesos[NUM_PROCESOS];
    int ids[NUM_PROCESOS];

    pthread_mutex_init(&mutex, NULL);
    inicializar_necesidades();

    for (int i = 0; i < NUM_PROCESOS; ++i) {
        ids[i] = i;
        if (pthread_create(&procesos[i], NULL, solicitar_recursos, &ids[i]) != 0) {
            perror("Error al crear el hilo del proceso");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_PROCESOS; ++i) {
        pthread_join(procesos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}