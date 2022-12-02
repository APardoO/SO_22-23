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

// ===== Métodos semi-privados de la práctica =====

// ==================== PRÁCTICA 3 ====================
// [+] Hacer
int cmdPriority(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdShowvar(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdChangevar(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdShowenv(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdFork(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdExecute(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [!] Hacer
int cmdListjobs(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [!] Hacer
int cmdDeljobs(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [!] Hacer
int cmdJob(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}