#define _GNU_SOURCE
#include <grp.h>			// Aporta la definicion de datos de los grupos de linux
#include <pwd.h>			// Aporta la definicion de datos de la estructura passwd
#include <time.h>			// Librería de tiempo del sistema
#include <ctype.h>			// Incluye funciones para comprobar si es un numero
#include <errno.h>			// Librería de captador de errores
#include <fcntl.h>			// Librería para operaciones con archivos
#include <stdio.h>			// Librería estándar de entrada/salida
#include <dirent.h>			// Librería que importa las entradas de directorios
#include <stdlib.h>			// Librería de conversión, memoria, procesos...
#include <string.h>			// Librería de tratamiento de "strings"
#include <unistd.h>			// Librería de funcionalidades del sistema
#include <sys/shm.h>		// Librería con utilidades para la memoria compartida
#include <sys/mman.h>		// Librería para la declaración de gestiones de memoria
#include <sys/stat.h>		// Obtener información de los archivos
#include <sys/wait.h>		// Incluye funciones wait
#include <sys/types.h>		// Obtiene los tipos de datos del sistema
#include <sys/utsname.h>	// Obtiene informacñon del sistema [LINUX]

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica
#include "p3.h"				// Librería con las cabeceras de la práctica

extern struct cmd_data cmd_table[];
extern struct cmd_help_data cmd_help[];
extern char **environ;

// ===== Métodos semi-privados de la práctica =====
int print_environ_values(char *env[], char *env_type){
	register int i=0;
	for(i=0; environ[i]!=NULL; ++i)
		printf("%p->%s[%d]=(%p) %s\n", &env[i], env_type, i, env[i], env[i]);
	return SSUCC_EXIT;
}

// ==================== PRÁCTICA 3 ====================
// [+] Hacer
int cmdPriority(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdShowvar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdChangevar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [✔] Hecha
int cmdShowenv(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)
		return print_environ_values(envp, "main args3");
	else if(strcmp(args[1], "-environ")==0)
		return print_environ_values(environ, "environ");
	else if(strcmp(args[1], "-addr")==0){
		printf("environ:   %p (almacenado en %p)\n", &environ[0], &environ);
		printf("main arg3: %p (almacenado en %p)\n", &envp[0], &envp);
		return SSUCC_EXIT;
	}

	printf("[!] Error: %s\n", strerror(EINVAL));
	return FSUCC_EXIT;
}

// [+] Hacer
int cmdFork(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdExecute(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=1, j=1;
	register char bgp=0;
	char *specific_environ[PHARAM_LEN];
	t_proc *nwItem=NULL;

	// Si solo se llama a esta función
	if(lenArg==1)
		return report_error_exit(EFAULT);

	// Coger el nuevo entorno
	for(i=1; BuscarVariable(args[i], envp)!=-1; ++i)
		specific_environ[i-1] = args[i];

	// Comprobando la flag de prioridad

	// Comprobando la flag de segundo plano
	if(args[lenArg-1][0]=='&'){
		if((nwItem = (t_proc *)malloc(sizeof(t_proc)))==NULL){
			free(nwItem);
			return report_error_exit(ENOMEM);
		}

		// Crear el item
		nwItem->pid = getpid();
		nwItem->time = currentTime();
		nwItem->status = ACTIVE;
		nwItem->priority = 0;

		// Crear la línea pasada como parametro
		for(j=1; j<lenArg; ++j){
			strcpy(nwItem->line, args[j]);
			if(j!=lenArg-1)
				strcpy(nwItem->line, " ");
		}

		// Insertar en la lista de procesos el nuevo proceso
		if(!insertElement(historicList, nwItem))
			printf("[!] Error: %s\n", strerror(ENOMEM));

		args[]

		// Cambiar la flag a activo
		bgp=1;
		printf("[%s] => %d", nwItem->line, bgp);
	}

	// Ejecución del programa
	if(execvpe(args[i], args+i, (i==0)? envp : specific_environ)==-1)
		printf("[!] Error: %s\n", strerror(errno));

	return SSUCC_EXIT;
}

// [!] Hacer
int cmdListjobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [!] Hacer
int cmdDeljobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [!] Hacer
int cmdJob(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}