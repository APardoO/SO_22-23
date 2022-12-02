/*	==============================================================
	Práctica 3 Sietemas Operativos
	@author:
		-> Adrián Pardo Martinez	[adrian.pardo.martinez@udc.es]
		-> Hugo Correa Blanco		[hugo.correa.blanco@udc.es]
	Grupo: 2.1
	============================================================== */
//#include <grp.h>			// Aporta la definicion de datos de los grupos de linux
//#include <pwd.h>			// Aporta la definicion de datos de la estructura passwd
//#include <time.h>			// Librería de tiempo del sistema
//#include <ctype.h>			// Incluye funciones para comprobar si es un numero
//#include <errno.h>			// Librería de captador de errores
//#include <fcntl.h>			// Librería para operaciones con archivos
//#include <stdio.h>			// Librería estándar de entrada/salida
//#include <dirent.h>			// Librería que importa las entradas de directorios
//#include <stdlib.h>			// Librería de conversión, memoria, procesos...
//#include <string.h>			// Librería de tratamiento de "strings"
//#include <unistd.h>			// Librería de funcionalidades del sistema
//#include <sys/shm.h>		// Librería con utilidades para la memoria compartida
//#include <sys/mman.h>		// Librería para la declaración de gestiones de memoria
#include <sys/stat.h>		// Obtener información de los archivos
//#include <sys/wait.h>		// Incluye funciones wait
//#include <sys/types.h>		// Obtiene los tipos de datos del sistema
//#include <sys/utsname.h>	// Obtiene informacñon del sistema [LINUX]

//#include "List.h"			// Librería con las funcionalidades de la lista
//#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

#include "p0.h"				// Librería con las cabeceras de la práctica 0
#include "p1.h"				// Librería con las cabeceras de la práctica 1
#include "p2.h"				// Librería con las cabeceras de la práctica 2
#include "p3.h"				// Librería con las cabeceras de la práctica 3

// ==== Definiciones de variables para la shell ====
// Tabla de programas de la shell
struct cmd_data cmd_table[] = {
	// P0
	{"autores", cmdAutores},
	{"pid", cmdPid},
	{"carpeta", cmdCarpeta},
	{"fecha", cmdFecha},
	{"hist", cmdHist},
	{"comando", cmdComando},
	{"infosis", cmdInfosys},
	{"ayuda", cmdHelp},
	{"salir", cmdExit},
	{"fin", cmdExit},
	{"bye", cmdExit},

	// P1
	{"create", cmdCreate},
	{"stat", cmdStat},
	{"list", cmdList},
	{"delete", cmdDelete},
	{"deltree", cmdDeltree},

	// P2
	{"allocate", cmdAllocate},
	{"deallocate", cmdDeallocate},
	{"i-o", cmdIo},
	{"e-s", cmdIo},		// NO contemplada por la documentacion pero sí por la shell de referencia
	{"memdump", cmdMemdump},
	{"memfill", cmdMemfill},
	{"memory", cmdMemory},
	{"recurse", cmdRecurse},

	// P3
	{"priority", cmdPriority},
	{"showvar", cmdShowvar},
	{"changevar", cmdChangevar},
	{"showenv", cmdShowenv},
	{"fork", cmdFork},
	{"execute", cmdExecute},
	{"listjobs", cmdListjobs},
	{"deljobs", cmdDeljobs},
	{"job", cmdJob},

	{NULL, NULL}
};
// Tabla para la ayuda de los comandos
struct cmd_help_data cmd_help[] = {
	// P0
	{"autores", "[-n|-l]", "\tMuestra los nombres y/o logins de los autores\n"},
	{"pid", "[-p]", "\tMuestra el pid del shell o de su proceso padre\n"},
	{"carpeta", "[dir]", "\tCambia (o muestra) el directorio actual del shell\n"},
	{"fecha", "[-d|-h]", "\tMuestra la fecha y/o la hora actual\n"},
	{"hist", "[-c|-N]", "\tMuestra el historico de comandos, con -c lo borra\n"},
	{"comando", "[-N]", "\tRepite el comando N (del historico)\n"},
	{"infosis", NULL, "\tMuestra informacion de la maquina donde corre el shell\n"},
	{"ayuda", "[cmd]", "\tMuestra ayuda sobre los comandos\n"},
	{"fin", NULL, "\tTermina la ejecucion del shell\n"},
	{"salir", NULL, "\tTermina la ejecucion del shell\n"},
	{"bye", NULL, "\tTermina la ejecucion del shell\n"},

	// P1
	{"create", "[-f] [name]", "\tCrea un directorio o un fichero (-f)\n"},
	{"stat", "[-long][-link][-acc] name1 name2", " ..\tlista ficheros;\n\t-long: listado largo\n\t-acc: acesstime\n\t-link: si es enlace simbolico, el path contenido\n"},
	{"list", "[-reca] [-recb] [-hid][-long][-link][-acc] n1 n2", " ..\tlista contenidos de directorios\n\t-hid: incluye los ficheros ocultos\n\t-recb: recursivo (antes)\n\t-reca: recursivo (despues)\n\tresto parametros como stat\n"},
	{"delete", "[name1 name2 ..]", "\tBorra ficheros o directorios vacios\n"},
	{"deltree", "[name1 name2 ..]", "\tBorra ficheros o directorios no vacios recursivamente\n"},
	
	// P2
	{"allocate", "[-malloc|-shared|-createshared|-mmap]", "... Asigna un bloque de memoria\n\t-malloc tam: asigna un bloque malloc de tamano tam\n\t-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n\t-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n\t-mmap fich perm: mapea el fichero fich, perm son los permisos\n"},
	{"deallocate", "[-malloc|-shared|-delkey|-mmap|addr]", "..\tDesasigna un bloque de memoria\n\t-malloc tam: desasigna el bloque malloc de tamano tam\n\t-shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n\t-delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n\t-mmap fich: desmapea el fichero mapeado fich\n\taddr: desasigna el bloque de memoria en la direccion addr\n"},
	{"i-o", "[read|write] [-o] fiche addr cont", " \n\tread fich addr cont: Lee cont bytes desde fich a addr\n\twrite [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n\t\taddr es una direccion de memoria\n"},
	{"e-s", "[read|write] [-o] fiche addr cont", " \n\tread fich addr cont: Lee cont bytes desde fich a addr\n\twrite [-o] fich addr cont: Escribe cont bytes desde addr a fich. -o para sobreescribir\n\t\taddr es una direccion de memoria\n"},
	{"memdump", "addr cont", " \tVuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n"},
	{"memfill", "addr cont byte", " \tLlena la memoria a partir de addr con byte\n"},
	{"memory", "[-blocks|-funcs|-vars|-all|-pmap]", " ..\tMuestra muestra detalles de la memoria del proceso\n\t\t-blocks: los bloques de memoria asignados\n\t\t-funcs: las direcciones de las funciones\n\t\t-vars: las direcciones de las variables\n\t\t:-all: todo\n\t\t-pmap: muestra la salida del comando pmap(o similar)\n"},
	{"recurse", "[n]", "\tInvoca a la funcion recursiva n veces\n"},

	// P3
	{"priority", "[pid] [valor]", "\tMuestra o cambia la prioridad del proceso pid a valor\n"},
	{"showvar", "var1", "\tMuestra el valor y las direcciones de la variable de entorno var\n"},
	{"changevar", "[-a|-e|-p] var valor", "\tCambia el valor de una variable de entorno\n\t-a: accede por el tercer arg de main\n\t-e: accede mediante environ\n\t-p: accede mediante putenv\n"},
	{"showenv", "[-environ|-addr]", "\tMuestra el entorno del proceso\n"},
	{"fork", NULL, "\tHace una llamada fork para crear un proceso\n"},
	{"execute", "prog args", "....\tEjecuta, sin crear proceso,prog con argumentos\n"},
	{"listjobs", NULL, "\tLista los procesos en segundo plano\n"},
	{"deljobs", "[-term][-sig]", "\tElimina los procesos de la lista procesos en sp\n\t-term: los terminados\n\t\t-sig: los terminados por senal\n"},
	{"job", "[-fg] pid", "\tMuestra informacion del proceso pid.\n\t\t-fg: lo pasa a primer plano\n"},

	{NULL, NULL, NULL}
};

/* ==== MAIN FUNCTION ==== */
int main(int argc, char const *argv[]){
	int argLen=0;							// Número de parametros del comadno introducido
	int exitCode=SSUCC_EXIT;				// Código de salida de los comandos
	char *args[PHARAM_LEN];					// Parámetros del comando introducido
	char linea[COMMAND_BUFFER];				// String con el comando introducido
	
	List historicList = NULL;				// Lista del histórico de comandos
	List memoryList = NULL;					// Lista para la memoria reservada
	List processList = NULL;				// Lista con los procesos generados

	// Crear la lista del histórico y de la memoria
	createList(&historicList);
	createList(&memoryList);
	createList(&processList);

	do{
		printPrompt(exitCode);
		getCmdLine(linea, &argLen, args, historicList, memoryList, processList);
		exitCode = executeCommand(argLen, args, historicList, memoryList, processList);
	}while(exitCode != SCSS_EXIT);

	// Liberar la memoria de las lista
	freed_list_memory(historicList, memoryList, processList);
	return SCSS_EXIT;
}