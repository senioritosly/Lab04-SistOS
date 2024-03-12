#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t recurso1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recurso2_mutex = PTHREAD_MUTEX_INITIALIZER;

void* proceso1(void* arg) {

    while (1) {

        // Intenta adquirir recurso1
        pthread_mutex_lock(&recurso1_mutex);
        printf("Proceso 1 adquirió recurso1\n");
        printf("Intentando adquirir recurso2...\n");

        usleep(100000);

        pthread_mutex_lock(&recurso2_mutex);
        printf("Proceso 1 adquirió recurso2\n");
        printf("Intentando adquirir recurso1...\n");

        pthread_mutex_unlock(&recurso2_mutex);
        pthread_mutex_unlock(&recurso1_mutex);
        usleep(100000);

    }

    return NULL;

}



void* proceso2(void* arg) {

    while (1) {

        pthread_mutex_lock(&recurso2_mutex);
        printf("Proceso 2 adquirió recurso2\n");
        printf("Intentando adquirir recurso1...\n");
        usleep(100000);


        pthread_mutex_lock(&recurso1_mutex);
        printf("Proceso 2 adquirió recurso1\n");
        printf("Intentando adquirir recurso2...\n");

        pthread_mutex_unlock(&recurso1_mutex);
        pthread_mutex_unlock(&recurso2_mutex);
        usleep(100000);

    }

    return NULL;

}



int main() {
    pthread_t hilo_proceso1, hilo_proceso2;
    if (pthread_create(&hilo_proceso1, NULL, proceso1, NULL) != 0 ||
        pthread_create(&hilo_proceso2, NULL, proceso2, NULL) != 0) {
        perror("Error al crear los hilos");
        exit(EXIT_FAILURE);
    }

    pthread_join(hilo_proceso1, NULL);
    pthread_join(hilo_proceso2, NULL);

    return 0;
}
