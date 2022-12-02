#ifndef SYS_MODULE_H
#define SYS_MODULE_H

#include <time.h>					// Librería de tiempo del sistema
#include <sys/types.h>				// Obtiene los tipos de datos del sistema

#include "List.h"

// Definiciones globales de la shell
#define PHARAM_LEN			512		// Longitud de cada parametro
#define COMMAND_BUFFER		4096	// Longitud máxima del comando introducido
#define MAX_PATH 			1024	// Longitud máxima para el path
#define MAX_NAME_LEN		70		// Longitud máxima para nombres auxiliares del programa 'char[]'
#define MAX_DATE_SIZE		12		// Longitud de la cadena de texto para la fecha actual
#define MAX_HOUR_SIZE		10		// Longitud de la cadena de texto para la hora actual
#define TAMANO				2048	// Longitud máxima de cadena para llamadas recursivas

// Codigos de salida de la shell
#define SCSS_EXIT			 0		// Salida de programa
#define SSUCC_EXIT			 1		// Salida exitosa de comando
#define FSUCC_EXIT			-1		// Salida erronea de comando
#define FFAIL_EXIT			 1		// Salida forzada de programa

// Tabla de programas de la shell
struct cmd_data{
	char *cmd_name;
	int (*cmd_func)(const int argLen, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
};

// Tabla para la ayuda de los comandos
struct cmd_help_data{
	char *cmd_name;
	char *cmd_pharams;
	char *cmd_usage;
};

// Tipo de asignación de la memoria
enum asign_type {NOT_DEFINED, MALLOC_MEM, SHARED_MEM, MAPPED_MEM};
typedef enum asign_type t_asign;

// Tipos de información almacenada en los items de la lista de memoria
typedef struct{
	void *dir;				// Dirección de memoria
	size_t size;			// Tamaño de la reserva
	struct tm time;			// Tiempo de asignación
	t_asign type;			// Tipo de asignación
	key_t key;				// Clave utilizada en share
	char *file_name;		// Nombre del archivo utilizado en mmap
	int file_descriptor;	// Redireccionador utilizado en mmap
} t_mem;

// Tipos de información almacenada en los items de la lista de procesos

// ==== Métodos del sistema ====
void freed_list_memory(List historicList, List memoryList, List processList);															// Liverar la memoria de las listas
/*[!]*/int executeCommand(const int numTrozos, char *tokens[PHARAM_LEN], List historicList, List memoryList, List processList);			// Método que ejecuta el comando introducido por el usuario
void controled_exit(List historicList, List memoryList, List processList, int exitCode);												// Crea una salida controlada de la terminal
int TrocearCadena(char *line, char *tokens[]);																							// Sepra el comando introducido en parametros, usando como delimitador espacios, saltos de línea y tabuladores
void check_init_lists(List historicList, List memoryList, List processList);															// Comprobar que las listas están inicializadas
int report_error_exit(int errorCode);																									// Reporta un mensaje de error por pantalla
void printPrompt(int exitStatus);																										// Imprime por pantalla el propmt del usuario
void getCmdLine(char linea[COMMAND_BUFFER], int *argLen, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);	// Línea donde el usuario introduce el comando, se comprueba su validez, se guarda en el histórico y se ejecuta

// ================= Métodos globales de las prácticas =================
// Métodos de las prácticas que pueden ser usadas en cualquier otra
// Devuelven tipos concretos y hacen funciones auxiliares específicas

// ==== P0 ====
struct tm currentTime();										// Devuelve una estructura con la fecha actual
char *currentDirectory();										// Devuelve un string con el directorio actual, NULL en caso de que no exista
char *currentDate(char *format);								// Devuelve string con la fecha actual
char *currentHour(char *format);								// Devuelve un puntero a string con la hora actual

// ==== P1 ====
int isDir(const char *path);									// Comprueba si el path es un directorio válido
char LetraTF(mode_t m);											// Codigo de ejemplo p1
char * ConvierteModo2(mode_t m);								// Codigo de ejemplo p1

// ==== P2 ====
void * ObtenerMemoriaShmget(key_t clave, size_t tam, t_mem *item, List memoryList);	// Codigo de ejemplo p2
void * MapearFichero(char * fichero, int protection, List memoryList);				// Codigo de ejemplo p2
void Do_pmap();																		// Codigo de ejemplo p2
char *t_asigntoa(t_asign asign);													// Devuelve un string con el tipo de reserva de memoria hecha

void do_DeallocateDelkey(char *args);							// Codigo de ejemplo p2
void Recursiva(int n);											// Codigo de ejemplo p2
int LeerFichero(char *fich, void *p, int n);					// Codigo de ejemplo p2
int EscribirFichero(char *fich, void *p, int n);				// Codigo de ejemplo p2
void LlenarMemoria(void *p, size_t cont, unsigned char byte);	// Codigo de ejemplo p2
void freeMemoryListItem(void *data);							// Libera memoria de la lista de memoria

// ==== P3 ====
/*[!]*/void freeProcessListItem(void *data);							// Libera memoria de la lista de procesos

#endif //SYS_MODULE_H