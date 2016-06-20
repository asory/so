#include <stdio.h>
 #include <string.h> 
 #include <stdlib.h> 
 #include <unistd.h> 
 #include <signal.h> 
 #include <sys/wait.h> 
 #include <memoriaI.h> 
 #include <semaforoI.h> 
 #define COCHES 30 
 #define MAX_T_LANZAR 3 
 #define PUENTE "puente" 
 #define MUTEXN "mutex_n" 
 #define MUTEXS "mutex_s" 
 #define COCHESNORTE "coches_norte" 
 #define COCHESSUR "coches_sur" 
 
  pid_t pids[COCHES];
  
void liberarecursos ();
void finalizarprocesos ();
void controlador (int senhal);

int main (int argc, char *argv[]) {
	pid_t pid_hijo;
	int i;
	srand((int)getpid());
	
	// Creación de semáforos y segmentos de memoria compartida. 
	crear_sem(PUENTE, 1);
	crear_sem(MUTEXN, 1);
	crear_sem(MUTEXS, 1);
	crear_var(COCHESNORTE, 0);
	crear_var(COCHESSUR, 0);
	
	// Manejo de Ctrol+C. 
	if (signal(SIGINT, controlador) == SIG_ERR) {
		fprintf(stderr, "Abrupt termination.\n");
		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < COCHES; i++) {
		// Se lanzan los coches... 
		switch (pid_hijo = fork()) {
			case 0: 
			              if ((i % 2) == 0)
			              // Coche Norte. 
			execl("./exec/coche", "coche", "N", PUENTE, MUTEXN, COCHESNORTE , NULL); else // Coche Sur. 
			execl("./exec/coche", "coche", "S", PUENTE, MUTEXS, COCHESSUR, NULL);
			break;
		}
		// Fin switch. 
		sleep(rand() % MAX_T_LANZAR);
	}
	// Fin for. 
	for (i = 0; i < COCHES; i++) waitpid(pids[i], 0, 0);
	liberarecursos();
	return EXIT_SUCCESS;
}

void liberarecursos () {
	destruir_sem(PUENTE);
	destruir_sem(MUTEXN);
	destruir_sem(MUTEXS);
	destruir_var(COCHESNORTE);
	destruir_var(COCHESSUR);
}

void finalizarprocesos () {
	int i;
	printf ("\n--- Finalizando procesos --- \n");
	for (i = 0; i < COCHES; i++) 
	    if (pids[i]) {
		printf ("Finalizando proceso [%d]... ", pids[i]);
		kill(pids[i], SIGINT);
		printf ("<Ok>\n");
	}
}

void controlador (int senhal) {
	finalizarprocesos();
	liberarecursos();
	printf ("\nFin del programa (Ctrol + C)\n");
	exit(EXIT_SUCCESS);
}