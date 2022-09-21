/*	=========================================
	Práctica 0 Sietemas Operativos 15/09/2022

	Authors:
		-> Adrián Pardo Martinez
		-> Hugo Correa Blanco

	Grupo: 2.1

	-> Faltan los datos del comadno 'autores'
	-> Falta comprobar la implementacion de la lista
	-> Falta hacer el Makefile

	========================================= */
#include <time.h>					// Librería de tiempo del sistema
#include <stdio.h>					// Librería estándar de entrada/salida
#include <stdlib.h>					// Librería de conversión, memoria, procesos...
#include <string.h>					// Librería de tratamiento de "strings"
#include <unistd.h>					// Librería de funcionalidades del sistema
#include <errno.h>					// Librería de captador de errores
#include <sys/utsname.h>			// Obtiene informacñon del sistema [LINUX]

// Definiciones globales de la shell
#define COMMAND_LEN		512			// Longitud de cada parametro
#define COMMAND_BUFFER	4096		// Longitud máxima del comando introducido

// Definicion de tipos de la lista
typedef struct node *Lpos;
typedef struct node *List;
struct node {
	char comando[COMMAND_BUFFER];
	struct node *next;
};

// Definiciones globales
int argLen=0;						// Número de parametros del comadno introducido
char *args[COMMAND_LEN];			// Parámetros del comando introducido
char linea[COMMAND_BUFFER]=" ";		// String con el comando introducido
char get_input[COMMAND_BUFFER]=" ";	// Obtiene la líne introducida por el usuario, se crea para splitearla por \n y que no se sobreescriba en la misma direccion de memoria
List historicList;					// Lista del histórico de comandos

// Métodos de la implementacion de la lista
List createList();											// Devuelve una lista vacía
int isEmptyList(List l);									// Comprueba si la lista está vacía
Lpos firstElement(List l);									// Devuelve la primera posicion de la lista
Lpos nextElement(List l, Lpos p);							// Devuelve la siguiente posicion de la lsita si es válida
int insertElement(List l, char element[COMMAND_BUFFER]);	// Inserta un nuevo elemento al final de la lista
void getElement(List l, Lpos p, char *element);				// Devuelve el contenido de la lista en 'element'
void clearList(List l);										// Elimina todos los elementos de la lista
void deleteList(List l);									// Elimina la lista pasada por referencia

// Métodos del sistema
void printPrompt();
int TrocearCadena(char *line, char *tokens[]);
void getCmdLine();
int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]);

// Programas shell-in Build
int cmdAutores(const int lenArg, char *args[COMMAND_LEN]);
int cmdPid(const int lenArg, char *args[COMMAND_LEN]);
int cmdCarpeta(const int lenArg, char *args[COMMAND_LEN]);
int cmdFecha(const int lenArg, char *args[COMMAND_LEN]);
int cmdHist(const int lenArg, char *args[COMMAND_LEN]);
int cmdComando(const int lenArg, char *args[COMMAND_LEN]);
int cmdInfosys(const int lenArg, char *args[COMMAND_LEN]);
int cmdHelp(const int lenArg, char *args[COMMAND_LEN]);
int cmdExit(const int lenArg, char *args[COMMAND_LEN]);

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
	{"infosis", cmdInfosys},
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
	{"autores", "[-n|-l]\tMuestra los nombres y logins de los autores\n"},
	{"pid", "[-p]\tMuestra el pid del shell o de su proceso padre\n"},
	{"carpeta", "[dir]\tCambia (o muestra) el directorio actual del shell\n"},
	{"fecha", "[-d|-h]\tMuestra la fecha y o la hora actual\n"},
	{"hist", "[-c|-N]\tMuestra el historico de comandos, con -c lo borra\n"},
	{"comando", "[-N]\tRepite el comando N (del historico)\n"},
	{"infosis", "\tMuestra informacion de la maquina donde corre el shell\n"},
	{"ayuda", "[cmd]\tMuestra ayuda sobre los comandos\n"},
	{"fin", "\tTermina la ejecucion del shell\n"},
	{"salir", "\tTermina la ejecucion del shell\n"},
	{"bye", "\tTermina la ejecucion del shell\n"},
	{NULL, NULL}
};

/* == MAIN FUNCTION == */
int main(int argc, char const *argv[]){
	// Crear la lista del histórico
	historicList = createList();

	do{
		printPrompt();
		getCmdLine();
	}while(executeCommand(argLen, args)!=0);

	// Liberar la memoria de la lista
	deleteList(historicList);
	return 0;
}

// === DECLARACIONES PROPIAS DENTRO DE STRING.H ===
/*char *strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p != NULL) {
        memcpy(p, s, size);
    }
    return p;
}*/
char *strndup(const char *s, size_t n) {
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}

// === IMPLEMENTACION DE LA LISTA DEL HISTORICO ===
// Crea una lista vacía
List createList(){
	struct node *l = (struct node *)malloc(sizeof(struct node));
	l->next=NULL;
	return l;
}

// Comprueba si la lista está vacía
int isEmptyList(List l){
	return (l->next==NULL)? 1 : 0;
}

// Retorna el primer elemento de la lista
Lpos firstElement(List l){
	return l->next;
}

// Retorna el siguiente elemento si es válido
Lpos nextElement(List l, Lpos p){
	return (p->next==NULL)? NULL : p->next;
}

// Inserta un elemento en la lista
int insertElement(List l, char element[COMMAND_BUFFER]){
	struct node *nwPos = (struct node *)malloc(sizeof(struct node));
	struct node *auxPos;

	if(nwPos==NULL) return 0;

	// Compia de un duplicado del elemento a introducir
	strcpy(nwPos->comando, strndup(element, COMMAND_BUFFER));

	//strcpy(nwPos->comando, element);
	nwPos->next=NULL;

	if(l->next==NULL)
		l->next=nwPos;
	else{
		for(auxPos=l->next; auxPos->next!=NULL; auxPos=auxPos->next);
		auxPos->next=nwPos;
	}

	return 1;
}

// Retorna el elemento de la posicion en element
void getElement(List l, Lpos p, char *element){
	if(p==NULL || l->next==NULL) return;
	strcpy(element, p->comando);
}

// Elimina todos los elementos de la lista
void clearList(List l){
	struct node *auxPos;

	while(l->next!=NULL){
		auxPos = l->next;
		l->next = auxPos->next;
		auxPos->next=NULL;
		free(auxPos);
	}
}

// Elimina la lista
void deleteList(List l){
	clearList(l);
	l->next=NULL;
	free(l);
}

// == SYSTEM METHODS ==
// Imprime por pantalla el propmt del usuario
void printPrompt(){
	printf("[#]~$ ");
}

// Sepra la línea en partes usando como delimitador los espacios(' '), saltos de línea('\n') y tabuladores('\t')
int TrocearCadena(char *line, char *tokens[]){
	int i = 1;
	if ((tokens[0]=strtok(line," \n\t"))==NULL)
		return 0;
	while((tokens[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}

// Línea donde el usuario introduce el comando, se guarda en el histórico y se comprueba si es válido
void getCmdLine(){
	// Comprobar que la lista está inicializada
	if(isEmptyList(historicList)==1)
		historicList = createList();

	// Obteniendo el string de los comandos
	if(fgets(get_input, COMMAND_BUFFER, stdin)==NULL){
		printf("\n[!] Fin...\n");
		exit(0);
	}

	// Comprobación del comando introducido
	if(strcmp(get_input, "\n")!=0 && get_input!=NULL){
		char *token=strtok(get_input, "\n");
		strcpy(linea, token);

		// Insertar en la lista
		if(insertElement(historicList, linea)==0)
			printf("[!] Error: %s\n", strerror(12));

		// Separar en trozos la cadena de texto introducida
		argLen = TrocearCadena(linea, args);

	// En caso de no ser válido, la línea se vacía y el número de argumentos se pone a 0
	}else{
		strcpy(get_input, " ");
		strcpy(linea, " ");
		argLen = 0;
	}
}

// Método que ejecuta el comando introducido por el usuario
int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]){
	int i=0;

	// Captador del salto de linea sin comando introducido
	if(argLen==0) return 1;

	// Obtenemos la posicion en la tabla de comandos la posicion del comando a ejecutar
	while (cmd_table[i].cmd_name != NULL && strcmp(cmd_table[i].cmd_name, tokens[0])!=0)
		++i;

	// Comprobamos de que no sea una posicion erronea
	if(cmd_table[i].cmd_name != NULL)

		// En caso de que no se haya implementado la funcion de dicho programa
		if(cmd_table[i].cmd_func == NULL)
			printf("[!] Error: %s\n", strerror(38));
		
		else
			return cmd_table[i].cmd_func(numTrozos, tokens);

	// En caso de que no se encuentre el comando en la lista
	else
		printf("[!] Error: %s\n", strerror(38));

	return 1;
}

// == PROGRAMAS SHELL-IN BUILD ==
int cmdAutores(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1){
		printf("Adrian Pardo Martinez: adrian.pardo.martinez\n");
		printf("Hugo Correa Blanco: hugo.correa.blanco\n");
		return 1;
	}

	if(strcmp("-l", args[1])==0)
		printf("adrian.pardo.martinez\nhugo.correa.blanco\n");
	
	else if(strcmp("-n", args[1])==0)
		printf("Adrian Pardo Martinez\nHugo Correa Blanco\n");
	
	else
		printf("[!] Error: %s\n", strerror(22));

	return 1;
}

int cmdPid(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1){
		printf("Pid de shell: %d\n", getppid());
		return 1;
	}

	if(strcmp(args[1], "-p")==0)
		printf("Pid del padre del shell: %d\n", getpid());
	else
		printf("[!] Error: %s\n", strerror(22));
	
	return 1;
}

int cmdCarpeta(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1){
		char path[COMMAND_BUFFER];
		if(getcwd(path, COMMAND_BUFFER)==NULL)
			printf("[!] Error: %s\n", strerror(20));
		else
			printf("%s\n", path);
		return 1;
	}

	if(chdir(args[1])!=0)
		printf("[!] Error: %s\n", strerror(2));
	return 1;
}

// ====== CODIGO OFUSCADO ======
int cmdFecha(const int lenArg, char *args[COMMAND_LEN]){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);
	char datosFecha[70]="";
	char datosHora[70]="";

	int bytesFecha=0, bytesHora=0;
	bytesFecha = strftime(datosFecha, sizeof datosFecha, "%d/%m/%Y", &tiempoLocal);
	bytesHora = strftime(datosHora, sizeof datosHora, "%H:%M:%S", &tiempoLocal);

	if(bytesFecha==0 || bytesHora==0){
		printf("[!] Error: %s\n", strerror(8));
		return 1;
	}

	if(lenArg==1){
		printf("%s\n%s\n", datosHora, datosFecha);
		return 1;
	}

	if(strcmp("-d", args[1])==0)
		printf("%s\n", datosFecha);

	else if(strcmp("-h", args[1])==0)
		printf("%s\n", datosHora);

	else
		printf("[!] Error: %s\n", strerror(22));
	return 1;
}

static void printNcommands(int n){
	char comm[COMMAND_BUFFER] = "";
	Lpos auxPos;
	int iter=0;

	// Buscar una forma de aliviar código
	if(n>0){
		for(auxPos=firstElement(historicList); auxPos!=NULL && iter<n; ++iter, auxPos=nextElement(historicList, auxPos)){
			getElement(historicList, auxPos, comm);
			printf("%d->%s\n", iter, comm);
		}

	}else{
		for(auxPos=firstElement(historicList); auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos)){
			getElement(historicList, auxPos, comm);
			printf("%d->%s\n", iter, comm);
		}
	}
}

int cmdHist(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1){
		printNcommands(-1);
		return 1;
	}

	if(strcmp(args[1], "-c")==0){
		clearList(historicList);
		return 1;
	}

	char *num;
	int n=0;
	if ((num=strtok(args[1],"-"))==NULL){
		printf("[!] Error: %s\n", strerror(22));
		return 1;
	}

	if((n=atoi(num))!=0)
		printNcommands(n);
	else
		printf("[!] Error: %s\n", strerror(22));
	
	return 1;
}

// ====== CODIGO OFUSCADO ======
int cmdComando(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg>1){
		int iter=0, nCommand = atoi(args[1]);
		
		if(nCommand<=0){
			printf("[!] Error: %s\n", strerror(22));
			return 1;
		}

		Lpos auxPos;
		char comando[COMMAND_BUFFER] = "";
		for(auxPos=firstElement(historicList); iter<nCommand && auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos));

		// Comprobar la salida del bucle
		if(auxPos==NULL){
			printf("[!] Error: %s\n", strerror(95));
			return 1;
		}

		getElement(historicList, auxPos, comando);
		strcpy(linea, comando);

		printf("Ejecutando hist (%d): %s\n", nCommand, linea);

		// Separar en trozos la cadena de texto introducida
		argLen = TrocearCadena(linea, args);
		return executeCommand(argLen, args);
	}

	printNcommands(-1);
	return 1;
}

int cmdInfosys(const int lenArg, char *args[COMMAND_LEN]){
	struct utsname systemData;
	if(uname(&systemData)==-1)
		printf("[!] Error: %s\n", strerror(61));
	else
		printf("%s (%s), OS: %s-%s-%s\n", systemData.nodename, systemData.machine, systemData.sysname, systemData.release, systemData.version);

	return 1;
}

int cmdHelp(const int lenArg, char *args[COMMAND_LEN]){
	int i=0;

	if(lenArg==2){
		while(cmd_help[i].cmd_name != NULL && strcmp(cmd_help[i].cmd_name, args[1])!=0)
			++i;

		if(cmd_help[i].cmd_name == NULL)
			printf("[!] Error: %s\n", strerror(38));
		else
			printf("%s %s", cmd_help[i].cmd_name, cmd_help[i].cmd_usage);
	}else{
		printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n");
		for(i=0; cmd_help[i].cmd_name!=NULL; ++i)
			printf("%s ", cmd_help[i].cmd_name);

		printf("\n");
	}

	return 1;
}

int cmdExit(const int lenArg, char *args[COMMAND_LEN]){
	return 0;
}
