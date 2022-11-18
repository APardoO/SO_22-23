/*	=========================================
	Práctica 2 Sietemas Operativos
	Authors:
		-> Adrián Pardo Martinez
		-> Hugo Correa Blanco
	Grupo: 2.1
	========================================= */
#include <pwd.h>					// Aporta la definicion de datos de la estructura passwd
#include <grp.h>					// Aporta la definicion de datos de los grupos de linux
#include <time.h>					// Librería de tiempo del sistema
#include <stdio.h>					// Librería estándar de entrada/salida
#include <errno.h>					// Librería de captador de errores
#include <stdlib.h>					// Librería de conversión, memoria, procesos...
#include <string.h>					// Librería de tratamiento de "strings"
#include <unistd.h>					// Librería de funcionalidades del sistema
#include <dirent.h>					// Librería que importa las entradas de directorios
#include <signal.h>					// Librería que define macros para captar salidas anómalas
#include <sys/shm.h>				// Librería con utilidades para la memoria compartida
#include <sys/stat.h>				// Obtener información de los archivos
#include <sys/types.h>				// Obtiene los tipos de datos del sistema
#include <sys/utsname.h>			// Obtiene informacñon del sistema [LINUX]
#include <fcntl.h>					// ?
#include <sys/wait.h>				//Incluye funciones wait
#include <ctype.h>				//Incluye funciones para comprobar si es un numero

#include "List.h"					// Librería con las funcionalidades de la lista

// Definiciones globales de la shell
#define PHARAM_LEN		512			// Longitud de cada parametro
#define COMMAND_BUFFER	4096		// Longitud máxima del comando introducido
#define MAX_PATH 		1024		// Longitud máxima para el path
#define MAX_NAME_LEN	70			// Longitud máxima para nombres auxiliares del programa 'char[]'
#define MAX_DATE_SIZE	12			// Longitud de la cadena de texto para la fecha actual
#define MAX_HOUR_SIZE	10			// Longitud de la cadena de texto para la hora actual
#define TAMANO 			2048		// ? <- P2

// Definiciones globales
int argLen=0;						// Número de parametros del comadno introducido
char *args[PHARAM_LEN];				// Parámetros del comando introducido
char linea[COMMAND_BUFFER];			// String con el comando introducido
List historicList;					// Lista del histórico de comandos
List memoryList;					// Lista para la memoria reservada

// Métodos del sistema
void printPrompt();
int TrocearCadena(char *line, char *tokens[]);
void getCmdLine();
int executeCommand(const int numTrozos, char *tokens[PHARAM_LEN]);
void sighandler(int signum);

// ===== Programas propios de la shell =====
// P0
int cmdAutores(const int lenArg, char *args[PHARAM_LEN]);
int cmdPid(const int lenArg, char *args[PHARAM_LEN]);
int cmdCarpeta(const int lenArg, char *args[PHARAM_LEN]);
int cmdFecha(const int lenArg, char *args[PHARAM_LEN]);
int cmdHist(const int lenArg, char *args[PHARAM_LEN]);
int cmdComando(const int lenArg, char *args[PHARAM_LEN]);
int cmdInfosys(const int lenArg, char *args[PHARAM_LEN]);
int cmdHelp(const int lenArg, char *args[PHARAM_LEN]);
int cmdExit(const int lenArg, char *args[PHARAM_LEN]);
// P1
int cmdCreate(const int lenArg, char *args[PHARAM_LEN]);
int cmdStat(const int lenArg, char *args[PHARAM_LEN]);
int cmdList(const int lenArg, char *args[PHARAM_LEN]);
int cmdDelete(const int lenArg, char *args[PHARAM_LEN]);
int cmdDeltree(const int lenArg, char *args[PHARAM_LEN]);
// P2
int cmdAllocate(const int lenArg, char *args[PHARAM_LEN]);
int cmdDeallocate(const int lenArg, char *args[PHARAM_LEN]);
int cmdIo(const int lenArg, char *args[PHARAM_LEN]);
int cmdMemdump(const int lenArg, char *args[PHARAM_LEN]);
int cmdMemfill(const int lenArg, char *args[PHARAM_LEN]);
int cmdMemory(const int lenArg, char *args[PHARAM_LEN]);
int cmdRecurse(const int lenArg, char *args[PHARAM_LEN]);

// Tabla de programas de la shell
struct cmd_data{
	char *cmd_name;
	int (*cmd_func)(const int argLen, char *args[PHARAM_LEN]);
};
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
	{"fin", cmdExit},
	{"salir", cmdExit},
	{"bye", cmdExit},

	// P1
	{"create", cmdCreate},
	{"stat", cmdStat},
	{"list", cmdList},
	{"delete", cmdDelete},
	{"deltree", cmdDeltree},

	// P2
	{"allocate", cmdAllocate},		// Pardo
	{"deallocate", cmdDeallocate},	// Pardo
	{"i-o", cmdIo},
	{"memdump", NULL},
	{"memfill", NULL},
	{"memory", cmdMemory},
	{"recurse", cmdRecurse},		// Done

	{NULL, NULL}
};

// Tabla para la ayuda de los comandos
struct cmd_help_data{
	char *cmd_name;
	char *cmd_pharams;
	char *cmd_usage;
};
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
	{"memdump", "addr cont", " \tVuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n"},
	{"memfill", "addr cont byte", " \tLlena la memoria a partir de addr con byte\n"},
	{"memory", "[-blocks|-funcs|-vars|-all|-pmap]", " ..\tMuestra muestra detalles de la memoria del proceso\n\t\t-blocks: los bloques de memoria asignados\n\t\t-funcs: las direcciones de las funciones\n\t\t-vars: las direcciones de las variables\n\t\t:-all: todo\n\t\t-pmap: muestra la salida del comando pmap(o similar)\n"},
	{"recurse", "[n]", "\tInvoca a la funcion recursiva n veces\n"},

	{NULL, NULL}
};

// Tipo de asignación en la memoria
enum asign_type {NOT_DEFINED, MALLOC_MEM, SHARED_MEM, MAPPED_MEM};
typedef enum asign_type t_asign;

// Tipos de información almacenada en los items de la lista de memoria
// Otro tipo de información
typedef struct{
	key_t key;				// Clave utilizada en share
	char *file_name;		// Nombre del archivo utilizado en mmap
	int file_descriptor;	// Redireccionadorutilizado en mmap
} t_oinfo;
// Estructura básica
struct mem_table_data{
	void *dir;				// Dirección de memoria
	size_t size;			// Tamaño de la reserva
	struct tm time;			// Tiempo de asignación
	t_asign type;			// Tipo de asignación
	t_oinfo data;			// Otra información
};
typedef struct mem_table_data memory_item;


// ===== Métodos estáticos de la shell =====
// P0
static int currentDirectory();
static char *currentDate();
static char *currentHour();
static int checkZeroPharam(char arg[PHARAM_LEN]);
static void printNcommands(int n);
// P1
static void print_file_info(const char *name, const char *allPath, const struct stat *std, int longp, int accp, int linkp);
static void print_dir_data(const char *name, int hidp, int longp, int accp, int linkp);
static void process_dir_data(const char *name, int recap, int recbp, int hidp, int longp, int accp, int linkp);
static void list_fd_data(const char *name, const struct stat *std, int recap, int recbp, int hidp, int longp, int accp, int linkp);
// P2
static void freeMemoryListItem(void *data);
static char *t_asigntoa(t_asign asign);
static void print_memList(t_asign asign);


/* == MAIN FUNCTION == */
int main(int argc, char const *argv[]){
	// Mandamos la señal para captar el trap del ctrl_c
	signal(SIGINT, sighandler);

	// Crear la lista del histórico
	createList(&historicList);
	createList(&memoryList);

	do{
		printPrompt();
		getCmdLine();
	}while(executeCommand(argLen, args)!=0);

	// Liberar la memoria de la lista
	deleteList(historicList, free);
	deleteList(memoryList, freeMemoryListItem);
	return 0;
}

// ==================== PRÁCTICA 0 ====================

// == SYSTEM METHODS ==
// Imprime por pantalla el propmt del usuario
void printPrompt(){
	printf("[#]~$ ");
	//printf("-> ");
}
// Sepra el comando introducido en parametros, usando como delimitador espacios, saltos de línea y tabuladores
int TrocearCadena(char *line, char *tokens[]){
	register int i = 1;
	if ((tokens[0]=strtok(line," \n\t"))==NULL)
		return 0;
	while((tokens[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}
// Línea donde el usuario introduce el comando, se comprueba su validez, se guarda en el histórico y se ejecuta
void getCmdLine(){
	char get_input[COMMAND_BUFFER];

	// Comprobar que la lista está inicializada
	if(historicList==NULL)
		createList(&historicList);

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
		if(insertElement(historicList, strndup(linea, COMMAND_BUFFER))==0)
			printf("[!] Error: %s\n", strerror(ENOMEM));

		// Separar en trozos la cadena de texto introducida
		argLen = TrocearCadena(linea, args);

	// En caso de no ser válido, la línea se vacía y el número de argumentos se pone a 0
	}else{
		strcpy(linea, " ");
		argLen = 0;
	}
}
// Método que ejecuta el comando introducido por el usuario
int executeCommand(const int numTrozos, char *tokens[PHARAM_LEN]){
	register int i=0;

	// Captador del salto de linea; sin comando introducido
	if(argLen==0) return 1;

	// Obtenemos la posicion en la tabla de comandos del comando a ejecutar
	while (cmd_table[i].cmd_name != NULL && strcmp(cmd_table[i].cmd_name, tokens[0])!=0)
		++i;

	// Comprobamos de que no sea una posicion erronea
	if(cmd_table[i].cmd_name != NULL)

		// En caso de que no se haya implementado la funcion de dicho programa
		if(cmd_table[i].cmd_func == NULL)
			printf("[!] Error: %s\n", strerror(ENOSYS));
		
		else
			return cmd_table[i].cmd_func(numTrozos, tokens);

	// En caso de que no se encuentre el comando en la lista
	else
		printf("[!] Error: %s\n", strerror(ENOSYS));

	return 1;
}
// Método que redirecciona la saliza forzosa del programa para liberar la memoria dinámica del histórico
void sighandler(int signum){
	// Memoria reservada para el histórico
	deleteList(historicList, free);

	// Memoria reservada para la lista de memoria
	deleteList(memoryList, freeMemoryListItem);

	// Salida forzosa del programa
	exit(1);
}

// ================ COMANDOS PRÁCTICA 0 ===============

int cmdAutores(const int lenArg, char *args[PHARAM_LEN]){
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
		printf("[!] Error: %s\n", strerror(EINVAL));

	return 1;
}

int cmdPid(const int lenArg, char *args[PHARAM_LEN]){
	if(lenArg==1){
		printf("Pid de shell: %d\n", getppid());
		return 1;
	}

	if(strcmp(args[1], "-p")==0)
		printf("Pid del padre del shell: %d\n", getpid());
	else
		printf("[!] Error: %s\n", strerror(EINVAL));
	
	return 1;
}

// Muestra por pantalla el directorio actual
static int currentDirectory(){
	char path[COMMAND_BUFFER];
	if(getcwd(path, COMMAND_BUFFER)==NULL)
		printf("[!] Error: %s\n", strerror(ENOTDIR));
	else
		printf("%s\n", path);
	return 1;
}
int cmdCarpeta(const int lenArg, char *args[PHARAM_LEN]){
	if(lenArg==1)
		return currentDirectory();

	if(chdir(args[1])!=0)
		printf("[!] Error: %s\n", strerror(ENOENT));
	return 1;
}

// Devuelve un puntero a string con la fecha actual
static char *currentDate(){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);
	static char current_date[MAX_DATE_SIZE];

	sprintf(current_date, "%02d/%02d/%04d", tiempoLocal.tm_mday, tiempoLocal.tm_mon+1, tiempoLocal.tm_year+1900);
	return (current_date);
}
// Devuelve un puntero a string con la hora actual
static char *currentHour(){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);
	static char current_hour[MAX_HOUR_SIZE];

	sprintf(current_hour, "%02d:%02d:%02d", tiempoLocal.tm_hour, tiempoLocal.tm_min, tiempoLocal.tm_sec);
	return (current_hour);
}
int cmdFecha(const int lenArg, char *args[PHARAM_LEN]){
	if(lenArg==1){
		printf("%s\n%s\n", currentHour(), currentDate());
		return 1;
	}

	if(strcmp(args[1], "-d")==0)
		printf("%s\n", currentDate());
	else if(strcmp(args[1], "-h")==0)
		printf("%s\n", currentHour());
	else
		printf("[!] Error: %s\n", strerror(EINVAL));

	return 1;
}

// Comprueba el parametro introducido '-N'
static int checkZeroPharam(char arg[PHARAM_LEN]){
	int n=-1;
	arg[0]='0';
	return ((n=atoi(arg))<=0 && arg[1]!='0')? -1 : n;
}
// Muestra por pantalla los n primeros comandos del historicos
static void printNcommands(int n){
	Lpos auxPos;
	register int iter=0;

	if(n>=0)
		for(auxPos=firstElement(historicList); auxPos!=NULL && iter<n; ++iter, auxPos=nextElement(historicList, auxPos))
			printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));
	else
		for(auxPos=firstElement(historicList); auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos))
			printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));
	
}
int cmdHist(const int lenArg, char *args[PHARAM_LEN]){
	if(lenArg==1){
		printNcommands(-1);
		return 1;
	}

	if(strcmp(args[1], "-c")==0){
		clearList(historicList, free);
		return 1;
	}

	int n = checkZeroPharam(args[1]);
	if(n<0)
		printf("[!] Error: %s\n", strerror(EINVAL));
	else
		printNcommands(n);

	return 1;
}

int cmdComando(const int lenArg, char *args[PHARAM_LEN]){
	register int iter=0;
	int nCommand=0;
	Lpos auxPos;
	
	if(lenArg>1){
		if((nCommand=checkZeroPharam(args[1]))<0){
			printf("[!] Error: %s\n", strerror(EINVAL));
			return 1;
		}

		// Se recorre la lista hasta que se llegue a la posición o al fnal de la lista
		for(auxPos=firstElement(historicList); iter<nCommand && auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos));

		// Comprobar la salida del bucle
		if(auxPos==NULL){
			printf("[!] Error: %s\n", strerror(EOPNOTSUPP));
			return 1;
		}

		strcpy(linea, getElement(historicList, auxPos));
		printf("Ejecutando hist (%d): %s\n", nCommand, linea);

		// Separar en trozos la cadena de texto introducida
		argLen = TrocearCadena(linea, args);
		return executeCommand(argLen, args);
	}

	printNcommands(-1);
	return 1;
}

int cmdInfosys(const int lenArg, char *args[PHARAM_LEN]){
	struct utsname systemData;
	if(uname(&systemData)==-1)
		printf("[!] Error: %s\n", strerror(ENODATA));
	else
		printf("%s (%s), OS: %s-%s-%s\n", systemData.nodename, systemData.machine, systemData.sysname, systemData.release, systemData.version);

	return 1;
}

int cmdHelp(const int lenArg, char *args[PHARAM_LEN]){
	register int i=0;

	if(lenArg==2){
		while(cmd_help[i].cmd_name != NULL && strcmp(cmd_help[i].cmd_name, args[1])!=0)
			++i;

		if(cmd_help[i].cmd_name == NULL)
			printf("[!] Error: %s\n", strerror(ENOSYS));
		else
			printf("%s %s%s", cmd_help[i].cmd_name, cmd_help[i].cmd_pharams!=NULL? cmd_help[i].cmd_pharams : "", cmd_help[i].cmd_usage);
	}else{
		printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n");
		for(i=0; cmd_help[i].cmd_name!=NULL; ++i)
			printf("%s ", cmd_help[i].cmd_name);

		printf("\n");
	}

	return 1;
}

int cmdExit(const int lenArg, char *args[PHARAM_LEN]){
	return 0;
}

// ==================== PRÁCTICA 1 ====================

// Código de ejemplo para la resolucion de la práctica 1
static char LetraTF(mode_t m){
	switch (m&S_IFMT){	/*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK:	return 's'; /*socket */
		case S_IFLNK:	return 'l';	/*symbolic link*/
		case S_IFREG:	return '-';	/* fichero normal*/
		case S_IFBLK:	return 'b';	/*block device*/
		case S_IFDIR:	return 'd';	/*directorio */
		case S_IFCHR:	return 'c';	/*char device*/
		case S_IFIFO:	return 'p';	/*pipe*/
		default: return '?';	/*desconocido, no deberia aparecer*/
	}
}
static char * ConvierteModo2(mode_t m){
	static char permisos[12];
	strcpy (permisos,"---------- ");

	permisos[0]=LetraTF(m);
	if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r';    /*resto*/
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';

	return permisos;
}

int cmdCreate(const int lenArg, char *args[PHARAM_LEN]){
	
	char error [MAX_NAME_LEN] = "[!] Error";
	if(lenArg >1){
	
	char path [MAX_PATH];
	
	
	getcwd(path,sizeof(path));
	strcat(path,"/");
	
	if(strcmp(args[1],"-f") == 0){		// Si se pasa con -f se crea un archivo
		char* name = args[2];
		if(creat(strcat(path,name),0666) == -1){
			perror(error);
			}
		
	}else{				// Si solo se pasa el nombre se crea un directorio
		
			char* name = args[1];
			if(mkdir(strcat(path,name), 0666) ==-1){
				perror(error);	
			}
		}
	
	
	}else {
	
	perror(error);
	return -1;
	}

	return 1;
}

int isDir(const char *path){
    struct stat s;
    stat(path, &s);
    int out = S_ISDIR(s.st_mode);
    return out;
}

int borrarDir(char *dir){  		//Borra el directorio
    DIR *dirp;
    struct dirent *flist;
    char aux[MAX_NAME_LEN];

    if((dirp=opendir(dir)) ==NULL)return -1;

    while ((flist=readdir(dirp))!=NULL) { 	//Recorre el directorio
        strcpy(aux, dir);
        strcat(strcat(aux, "/"),flist->d_name);

        if(strcmp(flist->d_name, "..") == 0 ||
                strcmp(flist->d_name, ".") == 0)continue;

        if(isDir(aux)){ 		//Recursivo
            borrarDir(aux);
        }
        if(remove(aux))return -1; 	
    }
    closedir(dirp);

    return 0;
}

// Muestra por pantalla información del archivo
static void print_file_info(const char *name, const char *allPath, const struct stat *std, int longp, int accp, int linkp){	
	if(longp==1){
		struct tm tl;
		struct passwd *pws=getpwuid(std->st_uid);
		struct group *grp=getgrgid(std->st_gid);
		char *permisos=ConvierteModo2(std->st_mode), linkName[MAX_NAME_LEN]=" ", user[MAX_NAME_LEN], group[MAX_NAME_LEN];

		// Parseo del nombre de usuario
		if(pws!=NULL)	sprintf(user, "%s", pws->pw_name);
		else			sprintf(user, "%d", std->st_uid);

		// Parseo del nombre del grupo
		if(grp!=NULL)	sprintf(group, "%s", grp->gr_name);
		else			sprintf(group, "%d", std->st_gid);

		// Parseo del tiempo de acceso
		if(accp==1)		tl = *localtime(&(std->st_atime));
		else			tl = *localtime(&(std->st_ctime));

		// Printeo parcial/completo de los datos por pantalla
		printf("%04d/%02d/%02d-%02d:%02d ", tl.tm_year+1900, tl.tm_mon+1, tl.tm_mday, tl.tm_hour, tl.tm_min);
		printf("%3ld (%8ld) %8s %8s %s%8ld %s", std->st_nlink, std->st_ino, user, group, permisos, std->st_size, name);

		// Comprobando el linkeo
		if(linkp==1 && permisos[0]=='l'){
			readlink((allPath!=NULL)? allPath : name, linkName, sizeof(linkName));
			printf(" -> %s", linkName);
		}

	}else
		printf("\t%ld\t%s", std->st_size, name);
	printf("\n");
}
int cmdStat(const int lenArg, char *args[PHARAM_LEN]){
	struct stat std;
	register int i=1;
	int numFiles=0, longp=0, accp=0, linkp=0;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-long")==0)
				longp=1;
			if(strcmp(args[i], "-acc")==0)
				accp=1;
			if(strcmp(args[i], "-link")==0)
				linkp=1;
		}else
			++numFiles;
		++i;
	}

	// Longitud del comando o falta de archivos a los que hacer un status
	if(lenArg==1 || numFiles==0)
		return currentDirectory();

	// Stat para todos los archivos que se pasen como parámetros
	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], &std)==0)
			print_file_info(args[i], NULL, &std, longp, accp, linkp);
		else
			if(args[i][0]!='-')
				printf(" ****error al acceder a %s:No such file or directory\n", args[i]);
	}

	return 1;
}

// Muestra por pantalla el contenido del directorio
static void print_dir_data(const char *name, int hidp, int longp, int accp, int linkp){
	DIR *dir;
	struct dirent *ent=NULL;
	struct stat fStd;
	char inn_name[PHARAM_LEN];

	// Abrimos el directorio
	if((dir=opendir(name))!=NULL){
		printf("************%s\n", name);

		// Obtenermos cada uno de los archivos del cirectorio
		while((ent=readdir(dir))!=NULL){
			sprintf(inn_name, "%s/%s", name, ent->d_name);

			// [-hid] pasado como parametro
			if(lstat(inn_name, &fStd)==0){
				if(hidp==1)
					print_file_info(ent->d_name, inn_name, &fStd, longp, accp, linkp);
				else
					if(ent->d_name[0]!='.')
						print_file_info(ent->d_name, inn_name, &fStd, longp, accp, linkp);
			}
		}

		// Cerramos el directorio
		closedir(dir);
	}else
		printf("[!] Error: No se puede abrir el directorio\n");
}
// Pocesa los datos de un directorio
static void process_dir_data(const char *name, int recap, int recbp, int hidp, int longp, int accp, int linkp){
	DIR *dir;
	register struct dirent *ent=NULL;
	struct stat fStd;
	char inn_name[PHARAM_LEN];

	// Abrimos el directorio
	if((dir=opendir(name))!=NULL){

		// Obtenemos los archivos del directorio
		while((ent=readdir(dir))!=NULL){

			// Parseando los directorios . y ..  por la recursividad
			if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0)
				continue;

			// Concatenamos el path introducido con el nombre del directorio
			sprintf(inn_name, "%s/%s", name, ent->d_name);

			// Obtenemos los datos del archivo
			if(lstat(inn_name, &fStd)==0 && ConvierteModo2(fStd.st_mode)[0]=='d')
				list_fd_data(inn_name, &fStd, recap, recbp, hidp, longp, accp, linkp);
		}

		// Cerramos el directorio
		closedir(dir);
	}else
		printf("[!] Error: No se puede abrir el directorio\n");
}
// Comprueba de qiue forma procesar y mostrar los datos de un directorio u archivo
static void list_fd_data(const char *name, const struct stat *std, int recap, int recbp, int hidp, int longp, int accp, int linkp){
	// Se activa por defecto si -reca y -recb están activas a la vez
	// se ejecuta recb
	if(recbp==1){
		process_dir_data(name, recap, recbp, hidp, longp, accp, linkp);
		print_dir_data(name, hidp, longp, accp, linkp);

	// Se ejecuta reca
	}else if(recap==1){
		print_dir_data(name, hidp, longp, accp, linkp);
		process_dir_data(name, recap, recbp, hidp, longp, accp, linkp);

	// Se muestra el contenido del directorio
	}else
		print_dir_data(name, hidp, longp, accp, linkp);
}
int cmdList(const int lenArg, char *args[PHARAM_LEN]){
	struct stat std;
	register int i=1;
	int numData=0, recap=0, recbp=0, hidp=0, longp=0, accp=0, linkp=0;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-reca")==0)
				recap=1;
			if(strcmp(args[i], "-recb")==0)
				recbp=1;
			if(strcmp(args[i], "-hid")==0)
				hidp=1;
			if(strcmp(args[i], "-long")==0)
				longp=1;
			if(strcmp(args[i], "-acc")==0)
				accp=1;
			if(strcmp(args[i], "-link")==0)
				linkp=1;
		}else
			++numData;
		++i;
	}

	// Longitud del comando o falta de archivos a los que hacer un status
	if(lenArg==1 || numData==0)
		return currentDirectory();

	// Procesamos los archivos pasados como parametros
	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], &std)==0)
			if(S_ISDIR(std.st_mode))
				list_fd_data(args[i], &std, recap, recbp, hidp, longp, accp, linkp);
			else
				print_file_info(args[i], NULL, &std, longp, accp, linkp);
		else
			if(args[i][0]!='-')
				printf(" ****error al acceder a %s:No such file or directory\n", args[i]);
	}

	return 1;
}

int cmdDelete(const int lenArg, char *args[PHARAM_LEN]) {
    
    if(lenArg >1){ 		//Borra el archivo o la carpeta
        for(int i=1; i<lenArg; i++){
            if(remove(args[i]) !=0){
                printf("[!] Error al acceder a %s:No such file or directory\n",args[i]);
                
            }
        }
    }else { 			// Muestra directorio actual
          cmdCarpeta(1,0);
    }
    return 1;
}

int cmdDeltree(const int lenArg, char *args[PHARAM_LEN]){
	
	char error [MAX_NAME_LEN] = "[!] Error";

    if(lenArg >1){
        for(int i=1; i< lenArg; i++){
            if(isDir(args[i]) ){
                if(borrarDir(args[i])==-1 || remove(args[i]))
                    perror(error);
            }else if(remove(args[i])){
                perror(error);
            }
        }
    }else { 		//Muestra el directorio actual
        cmdCarpeta(1,0);
    }
	return 1;
}

// ==================== PRÁCTICA 2 ====================

// Liveración de la memoria en la lista de memoria
static void freeMemoryListItem(void *data){
	memory_item *item = (memory_item *)(data);
	
	free(item->dir);
	item->size=0;
	item->data.key = 0;
	item->data.file_name = NULL;
	item->data.file_descriptor = 0;

	if(item->type == SHARED_MEM)
		shmdt(item->dir);
	else
		free(item);
}
static char *t_asigntoa(t_asign asign){
	static char asign_name[7];
	strcpy(asign_name, "no_def");

	if(asign == MALLOC_MEM)
		strcpy(asign_name, "malloc");
	if(asign == SHARED_MEM)
		strcpy(asign_name, "shared");
	if(asign == MAPPED_MEM)
		strcpy(asign_name, "mapped");

	return asign_name;
}
static void print_memList(t_asign asign){
	Lpos auxPos;
	memory_item *auxItem;
	char time_format[MAX_NAME_LEN];

	printf("******Lista de bloques asignados shared para el proceso %d\n", getppid());

	for(auxPos = firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
		auxItem = getElement(memoryList, auxPos);

		strftime(time_format, MAX_NAME_LEN, "%b %d %R", &auxItem->time);

		// Se muestran por pantalla todas las reservas de memoria
		if(asign == NOT_DEFINED){
			printf("\t%p\t%13lu %s %s", auxItem->dir, (unsigned long) auxItem->size, time_format, t_asigntoa(auxItem->type));

			if(auxItem->type == SHARED_MEM)
				printf(" (key %u)", auxItem->data.key);
			printf("\n");
		
		// En caso de necesitar una salida concreta -> parseo del tipo de memoria reservada
		}else if(auxItem->type == asign){
			printf("\t%p\t%13lu %s %s", auxItem->dir, (unsigned long) auxItem->size, time_format, t_asigntoa(auxItem->type));

			if(auxItem->type == SHARED_MEM)
				printf(" (key %u)", auxItem->data.key);
			printf("\n");
		}
	}
}

// Código de ejemplo para la resolucion de la práctica 1
// cmdAllocate
static void * ObtenerMemoriaShmget(key_t clave, size_t tam, memory_item *item){
	void * p;
	int aux, id, flags = 0777;
	struct shmid_ds s;

	if(tam)     //tam distito de 0 indica crear
		flags = flags | IPC_CREAT | IPC_EXCL;

	if(clave==IPC_PRIVATE){  //no nos vale
		printf("La llave no sirve\n");
		errno=EINVAL;
		return NULL;
	
	}if((id=shmget(clave, tam, flags))==-1)
		return (NULL);

	if((p=shmat(id,NULL,0))==(void*) -1){
		aux=errno;
		
		if (tam)
			shmctl(id,IPC_RMID,NULL);
		
		errno=aux;
		return (NULL);
	}

	shmctl(id,IPC_STAT,&s);

	item->size = s.shm_segsz;
	item->dir = p;

	if(!insertElement(memoryList, item)){
		printf("[!] Error: %s\n", strerror(ENOMEM));
		return (NULL);
	}
	
	// Guardar en la lista   InsertarNodoShared (&L, p, s.shm_segsz, clave);
	return (p);
}
// cmdDeallocate
static void do_DeallocateDelkey(char *args){
	key_t clave;
	int id;
	char *key = args;

	if(key==NULL || (clave = (key_t)strtoul(key,NULL,10))==IPC_PRIVATE){
		printf("      delkey necesita clave_valida\n");
		return;
	}

	if((id=shmget(clave,0,0666))==-1){
		perror("shmget: imposible obtener memoria compartida");
		return;
	}

	if(shmctl(id,IPC_RMID,NULL)==-1)
		perror("shmctl: imposible eliminar memoria compartida\n");
}
// cmdRecurse
static void Recursiva(int n){
	char automatico[TAMANO];
	static char estatico[TAMANO];

	printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, (void *)&n, automatico, estatico);

	if(n>0)
		Recursiva(n-1);
}

int cmdAllocate(const int lenArg, char *args[PHARAM_LEN]){
	register short ctpos = 16;
	time_t currentTime;
	memory_item *nwItem = (memory_item *)malloc(sizeof(memory_item));

	if(nwItem==NULL)
		printf("[!] Error: %s\n", strerror(ENOMEM));
	else if(lenArg==1)
		print_memList(NOT_DEFINED);
	else if(strcmp(args[1], "-malloc")==0){
		// En caso de que se pasen solo dos parametros, se muestra la lista solo con las reservas malloc
		if(lenArg==2)
			print_memList(MALLOC_MEM);

		// Se reserva la memoria e inserta el elemento en la lista de memoria, con la indicada
		else{
			// Comprobante de que se intenta hacer una reserva de 0 bytes
			if((nwItem->size = (size_t)strtoul(args[2], NULL, 10))==0){
				printf("No se asignan bloques de 0 bytes\n");
				return 1;
			}

			// Datos del item
			currentTime = time(NULL);
			nwItem->time = *localtime(&currentTime);
			nwItem->dir = (void *)malloc(nwItem->size);
			nwItem->type = MALLOC_MEM;
			nwItem->data.key = 0;
			nwItem->data.file_name = NULL;
			nwItem->data.file_descriptor = 0;

			// Comprobamos que podemos hacer la reservas
			if(nwItem->dir == NULL){
				printf("Imposible obtener memoria con malloc: Cannot allocate memory\n");
				return 1;
			}
			
			if(!insertElement(memoryList, nwItem)){
				printf("[!] Error: %s\n", strerror(ENOMEM));
				return 1;
			}

			printf("Asignados %lu bytes en %p\n", (unsigned long) nwItem->size, nwItem->dir);
		}

	}else if(strcmp(args[1], "-createshared")==0){
		// En caso de que no se pasen los parámetros adecuados
		if(lenArg<4)
			print_memList(SHARED_MEM);

		// Se reserva un bloque de memoria compartida y se añade a la lista de memoria compartida
		else{
			// Comprobante de que se intenta hacer una reserva de 0 bytes
			if((nwItem->size = (size_t)strtoul(args[3], NULL, 10))==0){
				printf("No se asignan bloques de 0 bytes\n");
				return 1;
			}

			// Datos del item
			currentTime = time(NULL);
			nwItem->time = *localtime(&currentTime);
			nwItem->type = SHARED_MEM;
			nwItem->data.file_name = NULL;
			nwItem->data.file_descriptor = 0;

			// Comprobamos si se pudo cerear la memoria compartida
			if((nwItem->data.key = (key_t)strtoul(args[2], NULL, 10))==0 || ObtenerMemoriaShmget(nwItem->data.key, nwItem->size, nwItem)==NULL){
				printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) nwItem->data.key, strerror(errno));
				return 1;
			}

			printf("Asignados %lu bytes en %p\n", (unsigned long) nwItem->size, nwItem->dir);
		}

	}else if(strcmp(args[1], "-shared")==0){
		// En caso de que no se pasen los parametros adecuados
		if(lenArg==2)
			print_memList(SHARED_MEM);

		// Se comprueba que haya un segmento de memoria compartido con la clave pasada como parametro
		// en caso contrario lo asigna
		else{
			// Datos del item
			currentTime = time(NULL);
			nwItem->time = *localtime(&currentTime);
			nwItem->type = SHARED_MEM;
			nwItem->data.file_name = NULL;
			nwItem->data.file_descriptor = 0;

			if((nwItem->data.key = (key_t)strtoul(args[2], NULL, 10))==0 || ObtenerMemoriaShmget(nwItem->data.key, nwItem->size, nwItem)==NULL){
				if(nwItem->data.key==0)
					printf("Es el key\n");
				printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) nwItem->data.key, strerror(errno));
				return 1;
			}

			printf("Memoria compartida de clave %lu  en %p\n", (unsigned long) nwItem->size, nwItem->dir);
		}

	}else if(strcmp(args[1], "-mmap")==0){
		// Code
	
	}else{
		if(lenArg>1) printf("uso: allocate %s ....\n", cmd_help[ctpos].cmd_pharams);
		free(nwItem);
	}

	return 1;
}

int cmdDeallocate(const int lenArg, char *args[PHARAM_LEN]){
	memory_item *infoData = (memory_item *)malloc(sizeof(memory_item));
	size_t tam;
	key_t llave;
	Lpos auxPos;
	char directory[MAX_NAME_LEN];

	if(lenArg==1)
		print_memList(NOT_DEFINED);

	else if(strcmp(args[1], "-malloc")==0){
		// En caso de que se pasen solo dos parametros, se muestra la lista solo con las reservas malloc
		if(lenArg==2)
			print_memList(MALLOC_MEM);

		// Se livera el primer bloque reservado de la memoria y se elimina de la lista
		else{
			// Comprobante de que se intenta hacer una reserva de 0 bytes
			if((tam = (size_t)strtoul(args[2], NULL, 10))==0){
				printf("No se asignan bloques de 0 bytes\n");
				return 1;
			}
			
			// Recorremos la lista hasta el primer bloque de memoria asignado con ese tamaño
			for(auxPos = firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
				infoData = getElement(memoryList, auxPos);
				if(infoData->type != MALLOC_MEM) continue;
				if(infoData->size == tam)	break;
			}

			infoData = deletePosition(memoryList, auxPos);

			// Eliminamos el elemento
			free(infoData->dir);
			infoData->size=0;
			infoData->type=NOT_DEFINED;
			infoData->data.key=0;
			infoData->data.file_name=NULL;
			infoData->data.file_descriptor=0;
			free(infoData);
		}
	
	}else if(strcmp(args[1], "-shared")==0){
		// En caso de pasar parametros incorrectos, se muestra la lista de memoria compartida
		if(lenArg<=2)
			print_memList(SHARED_MEM);

		else{
			if(args[2] && (llave = (key_t)strtoul(args[2], NULL, 10))!=0){
				for(auxPos=firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
					infoData = getElement(memoryList, auxPos);
					if(infoData->data.key==llave){
						infoData = deletePosition(memoryList, auxPos);
						if(shmdt(infoData->dir)==-1)
							printf("[!] Error: %s\n", strerror(errno));
						free(infoData);
						break;
					}
				}
			}else
				printf("[!] Error: %s\n", strerror(EINVAL));
		}
	
	}else if(strcmp(args[1], "-delkey")==0){
		// En caso de pasar parametros incorrectos, se muestra la lista de memoria compartida
		if(lenArg<=2)
			print_memList(SHARED_MEM);

		else{
			// Comprobante de que se intenta hacer una reserva de 0 bytes
			if(args[2] && (llave = (key_t)strtoul(args[2], NULL, 10))!=0)
				do_DeallocateDelkey(args[2]);
			else
				printf("[!] Error: %s\n", strerror(EINVAL));
		}
	
	}else if(strcmp(args[1], "-mmap")==0){
		// Code

	}else{
		// Comprobar si el primer parametro es una dirección de memoria en la cual se ha mapeado alguna dirección
		if(argLen==2 && args[1][0]=='0' && args[1][1]=='x'){
			for(auxPos=firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
				infoData = getElement(memoryList, auxPos);
				sprintf(directory, "%p", infoData->dir);
				if(strcmp(args[1], directory)==0) break;
			}
			if(auxPos==NULL)
				printf("Direccion %s no asignada con malloc, shared o mmap\n", args[1]);
			else{
				infoData = deletePosition(memoryList, auxPos);
				free(infoData);
			}
		}else
			printf("Direccion 0x%s no asignada con malloc, shared o mmap\n", args[1]);
	}

	return 1;
}

int LeerFichero (char *fich, void *p, int n){ /* le n bytes del fichero fich en p */
    int nleidos,tam=n; /*si n==-1 lee el fichero completo*/
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return -1;
    if (n==-1)
        tam= s.st_size;
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return (nleidos);
}


int EscribirFichero (char *fich, void *p, int n){
    int nescritos,tam=n;
    int df, aux;
    df=open(fich,O_RDWR);
    if((nescritos=write(df,p,tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return (nescritos);
}

int cmdIo(const int lenArg, char *args[PHARAM_LEN]){
	
	int t;
	
	if(lenArg==1){
		printf("uso: e-s [read|write] ......\n");
		return -1;
		}
	
	if(lenArg >1 && lenArg<5){
	printf("Faltan parametros\n");
	return -1;
	}
	
        char *ptr;
        if(strcmp(args[1], "read")== 0){
            int n = ((ssize_t)-1);
            long addr = strtoul(args[3],&ptr,16);
            
            n = atoi(args[4]);
            
            if((t=LeerFichero(args[2], (long *)addr, n))==-1)
            	perror("error de lectura");
            else printf("leidos %d bytes en %ld\n",t,addr);
            	
            
            
        }else if(strcmp(args[1], "write")== 0){
            if((strcmp(args[2], "-o")==0)){
                creat(args[3], 0777);
                long addr = strtoul(args[4],&ptr,16);
                if((t=EscribirFichero(args[3], (long *)addr, atoi(args[5])))==-1)
                    perror("error de escritura");
                else printf("escritos %d bytes en %ld\n",t,addr);
                	
            }else if(open(args[2],O_RDWR)==-1){
                creat(args[2], 0777);
                long addr = strtoul(args[3],&ptr,16);
                if((t=EscribirFichero(args[2], (long *)addr, atoi(args[4])))==-1)
                    perror("error de escritura");
                else printf("escritos %d bytes en %ld\n",t,addr);
            }
        }
     	
       
    
    return 1;
}


int cmdMemdump(const int lenArg, char *args[PHARAM_LEN]){
	// Cdde
	return 1;
}

int cmdMemfill(const int lenArg, char *args[PHARAM_LEN]){
	// Code
	return 1;
}

void Do_pmap (void) 
 { pid_t pid;       
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
         
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
         perror("cannot execute procmap (OpenBSD)");
         
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
         perror("cannot execute vmmap (Mac-OS)");      
      exit(1);
  }
  waitpid (pid,NULL,0);
}

int n1=0,n2=0,n3=0;	//variables glbales para memory

int cmdMemory(const int lenArg, char *args[PHARAM_LEN]){
	
    if(lenArg >1){
        
    	if(strcmp(args[1], "-vars")== 0){
                int x=0,y=0,z=0;
                static int a=0,b=0,c=0;

                printf("Variables locales:\t%p, %p, %p\n", &x, &y, &z);
                printf("Variables estaticas:\t%p, %p, %p\n", &a, &b, &c);
                printf("Variables globales:\t%p, %p, %p\n", &n1, &n2, &n3);

            }if(strcmp(args[1], "-funcs")== 0){
                printf("Funciones programa:\t%p, %p, %p\n", cmdAutores, cmdPid, cmdInfosys);
                printf("Funciones libreria:\t%p, %p, %p\n", malloc, printf, strcmp);

            }if(strcmp(args[1], "-blocks")== 0){
                cmdAllocate(1, NULL);

            }else if(strcmp(args[1], "-all")== 0){
                
                char *input[PHARAM_LEN] = {" ","-vars"};
                cmdMemory(2,input);
                char *input2[PHARAM_LEN] = {" ","-funcs"};
                cmdMemory(2,input2);
                char *input3[PHARAM_LEN] = {" ","-blocks"};
                cmdMemory(2,input3);

            }else if(strcmp(args[1], "-pmap")== 0){
                Do_pmap();
            }
        
    }else{
        char *input[PHARAM_LEN] = {" ","-all"};
        cmdMemory(2,input);
        
    }
    
	return 1;
}
}

// Done
int cmdRecurse(const int lenArg, char *args[PHARAM_LEN]){
	if(lenArg==1) return 1;
	Recursiva(atoi(args[1]));
	return 1;
}
