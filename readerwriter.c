/**
Este programa trata de simular el problema de los lectores-escritores. El 
problema de los lectores-escritores busca modelar un problema de exclusion 
mutua categorica.
Este problema establece que mientras un escritor este escribiendo nadie mas 
puede ni leer, ni escribir. De otro lado, si hay alguien esta leyendo, pueden
llegar mas lectores a leer pero mientras alguien esta leyendo un escritor no 
puede escribir.
*/

#include "readerwriter.h" // archivo de cabecera con funciones auxiliares
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

#define SEMAPHORE10 "/mysemaphore1000"
#define SEMAPHORE20 "/mysemaphore2000"
#define MAXPERSONS 10

sem_t *semaforo1;
sem_t *semaforo2;

int contador;
int numreaders;
int numwriters;
int contador2;

void *reading(void*);
void *writing(void*);
void INThandler(int);

int main() {
	semaforo1 = sem_open(SEMAPHORE10, O_CREAT, S_IRUSR | S_IWUSR, 1);
	semaforo2 = sem_open(SEMAPHORE20, O_CREAT, S_IRUSR | S_IWUSR, 1);
	pthread_t personas[MAXPERSONS];
	long tiempo = 0.0;
	int ids[MAXPERSONS];
	int i;
	contador = 0;
	numreaders = 0;
	numwriters = 0;
	signal(SIGINT, INThandler);
	// Inicializo el generador de numeros aleatorios
	srand(time(NULL));
	// Se crearan MAXPERSONS 
	for (i = 0; i <  MAXPERSONS; i++) {
		ids[i] = i + 1;
		//printf("La persona %d llego en el tiempo %li\n",i,tiempo);
		// aleatoriamente se decide si se crea un lector o un escritor
		if (readerorwriter(.7) == READER) {
			pthread_create(&personas[i],NULL,reading,(void*)ids[i]);
			numreaders++;
		} else {
			pthread_create(&personas[i],NULL,writing,(void*)ids[i]);
			numwriters++;
		}
		if (i < MAXPERSONS - 1) {
			printf("Tiempo actual %li ms. ",tiempo);
			long _tiempo = __waitt__;
			printf("La proxima persona llegara en %li ms\n", _tiempo);
			__wait__((int)_tiempo);
			tiempo += _tiempo;
		} else {
#ifdef DEBUG
			printf("La ultima persona\n");
#endif
		}
	}

	for (i = 0; i < MAXPERSONS; i++) {
		pthread_join(personas[i], NULL);
	}
	sem_close(semaforo1);
	sem_unlink(SEMAPHORE10);
    sem_close(semaforo2);
	sem_unlink(SEMAPHORE20);
	
	return 0;
}

/**
Funcion que modela el comportamiento lectura.
*/
void *reading(void* data) {

	int id = (int) data;
	long _tiempo;
	_tiempo = __waitt__;

	sem_wait(semaforo1);
	contador++;
	if (contador == 1){
		sem_wait(semaforo2);
	}

	sem_post(semaforo1);
	
	
	printf("El lector %d estara leyendo por %li ms. Hay %d lectores en el salon\n",id,_tiempo,contador);
	__reading__(_tiempo);



	sem_wait(semaforo1);
	contador--;
	printf("El lector %d termina de leer, ahora hay %d lectores leyendo\n",id, contador);
	
	if(contador == 0){
		sem_post(semaforo2);
}
	sem_post(semaforo1);

	
	return NULL;
}

/**
Funcion que modela el comportamiento escritura.
*/
void *writing(void* data) {
	int id = (int) data;
	long _tiempo = __waitt__;
	
	printf("La persona %d quien es escritor, intentara escribir\n",id);
	sem_wait(semaforo2);

	
	
	printf("El escritor %d esta escribiendo en el salon con %d lectores y escribira por %li ms\n",id,contador,_tiempo);
	__writing__(_tiempo);
	printf("El escritor %d termina de escribir\n",id);

	sem_post(semaforo2);
	return NULL;
}

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y') {
 		sem_close(semaforo1); sem_unlink(SEMAPHORE10);
 		sem_close(semaforo2); sem_unlink(SEMAPHORE20);
		exit(0);
     } else
          	signal(SIGINT, INThandler);
     getchar(); // Get new line character
}

