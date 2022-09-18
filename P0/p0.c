/*	=========================================
	Práctica 0 Sietemas Operativos 15/09/2022

	Authors:
		-> Adrián Pardo Martinez
		-> 

	Grupo: 2.1
	========================================= */
#include <time.h>					// Librería de tiempo del sistema
#include <stdio.h>					// Librería estándar de entrada/salida
#include <stdlib.h>					// Librería de conversión, memoria, procesos...
#include <string.h>					// Librería de tratamiento de "strings"
#include <unistd.h>					// Librería de funcionalidades del sistema
#include <errno.h>					// Librería de captador de errores
#include <limits.h>					//
#include <sys/utsname.h>			// Obtiene informacñon del sistema [LINUX]

#define COMMAND_LEN		512
#define COMMAND_BUFFER	4096

// Definiciones globales
int argLen;						// Número de parametros del comadno introducido
char *args[COMMAND_LEN];		// Parámetros del comando introducido
char linea[COMMAND_BUFFER];		// String con el comando introducido
char get_input[COMMAND_BUFFER];	// Obtiene la líne introducida por el usuario, se crea para splitearla por \n y que no se sobreescriba en la misma direccion de memoria

// Métodos de la implementacion de la lista

// Métodos del sistema
void printPrompt();
int TrocearCadena(char *line, char *tokens[]);
void getCmdLine();
int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]);

// Programas shell-in Build
int cmdAutores(const int argLen, char *args[COMMAND_LEN]);
int cmdPid(const int argLen, char *args[COMMAND_LEN]);
int cmdCarpeta(const int argLen, char *args[COMMAND_LEN]);
int cmdFecha(const int argLen, char *args[COMMAND_LEN]);
int cmdHist(const int argLen, char *args[COMMAND_LEN]);
int cmdComando(const int argLen, char *args[COMMAND_LEN]);
int cmdInfosys(const int argLen, char *args[COMMAND_LEN]);
int cmdHelp(const int argLen, char *args[COMMAND_LEN]);
int cmdExit(const int argLen, char *args[COMMAND_LEN]);

// Tablas necesarias para la práctica
struct cmd_data{
	char *cmd_name;
	int (*cmd_func)(const int argLen, char *args[COMMAND_LEN]);
};
struct cmd_data cmd_table[] = {
	{"autores", cmdAutores},
	{"pid", cmdPid},
	{"carpeta", cmdCarpeta},
	{"fecha", cmdFecha},
	{"hist", cmdHist},
	{"comando", cmdComando},
	{"infosys", cmdInfosys},
	{"ayuda", cmdHelp},
	{"fin", cmdExit},
	{"salir", cmdExit},
	{"bye", cmdExit},
	{NULL, NULL}
};

// Tabla para la ayuda de los comandos
struct cmd_help_data{
	char *cmd_name;
	char *cmd_usage;
};
struct cmd_help_data cmd_help[] = {
	{"autores", "\t\t:: Devuelve el nombre y los logins de los autores\n\t\t[-l] Devuelve solo los logins de los autores\n\t\t[-n] Devuelve solo los nombres de los autores\n"},
	{"pid", "\t\t:: Muestra el PID del proceso que ejecuta la shell\n\t\t[-p] Muestra el PID del proceso de la shell\n"},
	{"carpeta", "\t\t:: Muestra el directorio actual\n\t\t[direct] Cambia el directorio de trabajo al especificado\n"},
	{"fecha", "\t\t:: Muestra tanto la fecha como la hora\n\t\t[-d] Muestra la fecha en formato DD/MM/YYYY\n\t\t[-h] Muestra la hora en formato hh:mm:ss\n"},
	{"hist", "\t\t:: Muestra toda la lista del historico\n\t\t[-c] Limpia la lista del historico\n\t\t[-N] Muestra los primeros N elementos\n"},
	{"comando", "\t\t[N] :: Repite el comando N del historico\n"},
	{"infosys", "\t\t:: Muestra por pantalla información de la máquina que ejecuta la shell\n"},
	{"ayuda", "\t\t:: Muestra por pantalla la ayuda de todos los comandos\n\t\t[cmd]	Muestra por pantalla la ayuda asociada al comando especificado\n"},
	{"fin", "\t\t:: Sale de la shell"},
	{"salir", "\t\t:: Sale de la shell"},
	{"bye", "\t\t:: Sale de la shell"},
	{NULL, NULL}
};

/* == MAIN FUNCTION == */
int main(int argc, char const *argv[]){
	// Inicializar la lista

	do{
		printPrompt();
		getCmdLine();

		// Captador del salto de linea sin comando introducido
		if(argLen==0) continue;

		for(int i=0; i<argLen; ++i){
			printf("[%s]", args[i]);
		}
		printf("\n");
		
	}while(executeCommand(argLen, args)!=0);

	// Liberar la memoria de la lista
	return 0;
}

// == SYSTEM METHODS ==
void printPrompt(){
	printf("[#]~$ ");
}

// Splitea el comando por delimitadores
int TrocearCadena(char *line, char *tokens[]){
	int i = 1;
	if ((tokens[0]=strtok(line," \n\t"))==NULL)
		return 0;
	while((tokens[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}

void getCmdLine(){
	// Comprobar que la lista está inicializada

	// Obteniendo el string de los comandos
	if(fgets(get_input, COMMAND_BUFFER, stdin)==NULL){
		printf("\n[!] End...\n");
		exit(0);
	}
	char *token = strtok(get_input, "\n");

	// Comprobación del comando introducido
	if(token!=NULL){
		strcpy(linea, token);

		// Insertar en la lista
	}else{
		strcpy(linea, "");
		argLen = 0;
	}

	// Spliteo del comando introducido
	argLen = TrocearCadena(linea, args);
}

int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]){
	int i=0;

	// Obtenemos la posicion en la tabla de comandos la posicion del comando a ejecutar
	while (cmd_table[i].cmd_name != NULL && strcmp(cmd_table[i].cmd_name, tokens[0])!=0)
		++i;

	// Comprobamos de que no sea una posicion erronea
	if(cmd_table[i].cmd_name != NULL)

		// En caso de que no se haya implementado la funcion de dicho programa
		if(cmd_table[i].cmd_func == NULL)
			printf("[!] Error: %s function not found\n\n", tokens[0]);
		
		else
			return cmd_table[i].cmd_func(numTrozos, tokens);

	// En caso de que no se encuentre el comando en la lista
	else{
		printf("[!] Error: %s command not found\n\n", tokens[0]);
	}

	return 1;
}

// == PROGRAMAS SHELL-IN BUILD ==
// Done
int cmdAutores(const int argLen, char *args[COMMAND_LEN]){
	if(argLen==1){
		printf("Nombres:%s\nLogins:%s\n\n", "\n\tAdrian Pardo Martinez\n\t[NAME]", "\n\tadrian.pardo.martinez\n\t[LOGIN]");
		return 1;
	}

	if(strcmp("-l", args[1])==0){
		printf("Logins:%s\n\n", "\n\tadrian.pardo.martinez\n\t[LOGIN]");
		return 1;
	}

	if(strcmp("-n", args[1])==0){
		printf("Nombres:%s\n\n", "\n\tAdrian Pardo Martinez\n\t[NAME]");
		return 1;
	}

	printf("[!] Error: parametros incorrectos\n");
	return 1;
}

// Done
int cmdPid(const int argLen, char *args[COMMAND_LEN]){
	if(argLen==1){
		printf("%d\n\n", getppid());
		return 1;
	}

	if(strcmp(args[1], "-p")==0){
		printf("%d\n\n", getpid());
		return 1;
	}

	printf("[!] Error: parametros incorrectos\n");
	return 1;
}

// Done
int cmdCarpeta(const int argLen, char *args[COMMAND_LEN]){
	printf("argLen = %d\n", argLen);
	if(argLen==1){
		char path[COMMAND_BUFFER];
		if(getcwd(path, COMMAND_BUFFER)==NULL)
			printf("Error: No se pudo encontrar el directorio actual\n\n");
		else
			printf("%s\n\n", path);
		return 1;
	}

	if(chdir(args[1])!=0)
		printf("Error: Path not found\n\n");

	return 1;
}

// Done
int cmdFecha(const int argLen, char *args[COMMAND_LEN]){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);
	char datosFecha[70]="";
	char datosHora[70]="";
	char *formato_fecha="%d-%m-%Y";
	char *formato_hora="%H:%M:%S";

	int bytesFecha=0, bytesHora=0;
	bytesFecha = strftime(datosFecha, sizeof datosFecha, formato_fecha, &tiempoLocal);
	bytesHora = strftime(datosHora, sizeof datosHora, formato_hora, &tiempoLocal);

	if(argLen==1){
		if(bytesFecha==0 || bytesHora==0){
			printf("[!] Error de formato fecha\n");
			return 1;
		}

		printf("%s %s\n\n", datosFecha, datosHora);
		return 1;
	}

	if(strcmp("-d", args[1])==0){
		if(bytesFecha==0){
			printf("[!] Error de formato fecha\n");
			return 1;
		}

		printf("%s\n\n", datosFecha);
		return 1;
	}

	if(strcmp("-h", args[1])==0){
		if(bytesHora==0){
			printf("[!] Error de formato fecha\n");
			return 1;
		}

		printf("%s\n\n", datosHora);
		return 1;
	}

	return 1;
}

int cmdHist(const int argLen, char *args[COMMAND_LEN]){
	// Code
	return 1;
}

int cmdComando(const int argLen, char *args[COMMAND_LEN]){
	// Code
	return 1;
}

// Done
int cmdInfosys(const int argLen, char *args[COMMAND_LEN]){
	struct utsname systemData;
	
	if(uname(&systemData)==-1){
		printf("[!] Error: No se pudo obtener informacion del sistema\n\n");
		return 1;
	}

	printf("System name:\t%s\nNode name:\t%s\nRelease:\t%s\nVersion:\t%s\nMachine:\t%s\n\n", systemData.sysname, systemData.nodename, systemData.release, systemData.version, systemData.machine);
	return 1;
}

// Done
int cmdHelp(const int argLen, char *args[COMMAND_LEN]){
	int i=0;
	if(argLen==2){
		while(cmd_help[i].cmd_name != NULL && strcmp(cmd_help[i].cmd_name, args[1])!=0)
			++i;

		if(cmd_help[i].cmd_name == NULL)
			perror("[!] Comando no encontrado");
		else{
			printf("%s\n", cmd_help[i].cmd_usage);
		}
	}else{
		while(cmd_help[i].cmd_name!=NULL){
			printf("%s%s\n", cmd_help[i].cmd_name, cmd_help[i].cmd_usage);
			++i;
		}
	}
	return 1;
}

// Done
int cmdExit(const int argLen, char *args[COMMAND_LEN]){
	return 0;
}
