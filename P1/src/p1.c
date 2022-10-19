/*	=========================================
	Práctica 1 Sietemas Operativos 26/09/2022

	Authors:
		-> Adrián Pardo Martinez
		-> Hugo Correa Blanco

	Grupo: 2.1
	========================================= */

// Conseguir hacer trap del ctrl_C y una lista con las direcciones de memoria donde se
// reserva memoria para liverar la memoria cuando sea una salida forzosa del programa

#include <pwd.h>					// Aporta la definicion de datos de la estructura passwd
#include <grp.h>					// Aporta la definicion de datos de los grupos de linux
#include <time.h>					// Librería de tiempo del sistema
#include <stdio.h>					// Librería estándar de entrada/salida
#include <stdlib.h>					// Librería de conversión, memoria, procesos...
#include <string.h>					// Librería de tratamiento de "strings"
#include <unistd.h>					// Librería de funcionalidades del sistema
#include <errno.h>					// Librería de captador de errores
#include <sys/stat.h>				// Obtener información de los archivos
#include <sys/types.h>				// Obtiene los tipos de datos del sistema
#include <sys/utsname.h>			// Obtiene informacñon del sistema [LINUX]
#include <sys/stat.h>
#include <fcntl.h>
#include "List.h"					// Librería con las funcionalidades de la lista

#include "List.c"
// Definiciones globales de la shell
#define COMMAND_LEN		512			// Longitud de cada parametro
#define COMMAND_BUFFER	4096		// Longitud máxima del comando introducido
#define MAX_NAME_LEN	70			// Longitud máxima para nombres auxiliares del programa 'char[]'
#define MAX_PATH 1024			//Longitud máxima para el path
// Definiciones globales
int argLen=0;						// Número de parametros del comadno introducido
char *args[COMMAND_LEN];			// Parámetros del comando introducido
char linea[COMMAND_BUFFER];			// String con el comando introducido
char get_input[COMMAND_BUFFER];		// Obtiene la líne introducida por el usuario, se crea para splitearla por \n y que no se sobreescriba en la misma direccion de memoria
List historicList;					// Lista del histórico de comandos

// Métodos del sistema
void printPrompt();
int TrocearCadena(char *line, char *tokens[]);
void getCmdLine();
int executeCommand(const int numTrozos, char *tokens[COMMAND_LEN]);

// Programas shell-in Build
// P0
int cmdAutores(const int lenArg, char *args[COMMAND_LEN]);
int cmdPid(const int lenArg, char *args[COMMAND_LEN]);
int cmdCarpeta(const int lenArg, char *args[COMMAND_LEN]);
int cmdFecha(const int lenArg, char *args[COMMAND_LEN]);
int cmdHist(const int lenArg, char *args[COMMAND_LEN]);
int cmdComando(const int lenArg, char *args[COMMAND_LEN]);
int cmdInfosys(const int lenArg, char *args[COMMAND_LEN]);
int cmdHelp(const int lenArg, char *args[COMMAND_LEN]);
int cmdExit(const int lenArg, char *args[COMMAND_LEN]);

// P1
int cmdCreate(const int lenArg, char *args[COMMAND_LEN]);
int cmdStat(const int lenArg, char *args[COMMAND_LEN]);
int cmdList(const int lenArg, char *args[COMMAND_LEN]);
int cmdDelete(const int lenArg, char *args[COMMAND_LEN]);
int cmdDeltree(const int lenArg, char *args[COMMAND_LEN]);

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

	{"create", cmdCreate},	/* cmdCreate */
	{"stat", cmdStat},
	{"list", NULL},		/* cmdList */
	{"delete", cmdDelete},	/* cmdDelete */
	{"deltree", NULL},	/* cmdDeltree */

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

	{"create", "[-f] [name]\tCrea un directorio o un fichero (-f)\n"},
	{"stat", "[-long][-link][-acc] name1 name2 .. \tlista ficheros;\n\t\t-long: listado largo\n\t\t-acc: acesstime\n\t\t-link: si es enlace simbolico, el path contenido\n"},
	{"list", "[-reca] [-recb] [-hid][-long][-link][-acc] n1 n2 .. \tlista contenidos de directorios\n\t\t-hid: incluye los ficheros ocultos\n\t\t-reca: recursivo (antes)\n\t\t-recb: recursivo (despues)\n\t\tresto parametros como stat\n"},
	{"delete", "[name1 name2 ..]\tBorra ficheros o directorios vacios\n"},
	{"deltree", "[name1 name2 ..]\tBorra ficheros o directorios no vacios recursivamente\n"},
	
	{NULL, NULL}
};

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
static void crear_historic_dump(void *data){
	// Liberar memoria de los datos de lalista del histórico
	free((char *)data);
}


/* == MAIN FUNCTION == */
int main(int argc, char const *argv[]){
	// Crear la lista del histórico
	createList(&historicList);

	do{
		printPrompt();
		getCmdLine();
	}while(executeCommand(argLen, args)!=0);

	// Liberar la memoria de la lista
	deleteList(historicList, crear_historic_dump);
	return 0;
}

// ==================== PRÁCTICA 0 ====================

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
static int currentDirectory(){
	char path[COMMAND_BUFFER];
	if(getcwd(path, COMMAND_BUFFER)==NULL)
		printf("[!] Error: %s\n", strerror(20));
	else
		printf("%s\n", path);
	return 1;
}
int cmdCarpeta(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1)
		return currentDirectory();

	if(chdir(args[1])!=0)
		printf("[!] Error: %s\n", strerror(2));
	return 1;
}
// ====== CODIGO OFUSCADO ======
int cmdFecha(const int lenArg, char *args[COMMAND_LEN]){
	time_t crrent_time = time(NULL);
	struct tm tiempoLocal = *localtime(&crrent_time);

	char datosFecha[MAX_NAME_LEN]="";
	char datosHora[MAX_NAME_LEN]="";

	if(strftime(datosFecha, sizeof datosFecha, "%d/%m/%Y", &tiempoLocal)==0 || strftime(datosHora, sizeof datosHora, "%H:%M:%S", &tiempoLocal)==0){
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
	Lpos auxPos;
	int iter=0;

	// Buscar una forma de aliviar código
	if(n>0){
		for(auxPos=firstElement(historicList); auxPos!=NULL && iter<n; ++iter, auxPos=nextElement(historicList, auxPos))
			printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));

	}else{
		for(auxPos=firstElement(historicList); auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos))
			printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));
	}
}
int cmdHist(const int lenArg, char *args[COMMAND_LEN]){
	if(lenArg==1){
		printNcommands(-1);
		return 1;
	}

	if(strcmp(args[1], "-c")==0){
		clearList(historicList, crear_historic_dump);
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
		int iter=0, nCommand=atoi(args[1]);
		
		if(nCommand<=0 && strcmp(args[1], "0")!=0){
			printf("[!] Error: %s\n", strerror(22));
			return 1;
		}

		Lpos auxPos;
		for(auxPos=firstElement(historicList); iter<nCommand && auxPos!=NULL; ++iter, auxPos=nextElement(historicList, auxPos));

		// Comprobar la salida del bucle
		if(auxPos==NULL){
			printf("[!] Error: %s\n", strerror(95));
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

// ==================== PRÁCTICA 1 ====================

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
int cmdCreate(const int lenArg, char *args[COMMAND_LEN]){
	
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
	
	cmdCarpeta(1,0);		// Muestra directorio actual
	
	}
	
	return 1;
}

// ====== POSIBLE CODIGO OFUSCADO ====== >> BUSCAR OPTIMIZACION DEL CODIGO
static void print_stat_info(const char *name, const struct stat *std, int longp, int accp, int linkp){	
	if(longp==1){
		struct tm tiempoLocal;
		struct passwd *pws=getpwuid(std->st_uid);
		struct group *grp=getgrgid(std->st_gid);
		char fecha[MAX_NAME_LEN]="", *permisos=NULL, linkName[MAX_NAME_LEN]=" ", user[MAX_NAME_LEN], group[MAX_NAME_LEN];
		
		// Permisos del archivo
		permisos = ConvierteModo2(std->st_mode);

		// Parseo del nombre de usuario
		if(pws!=NULL)	sprintf(user, "%s", pws->pw_name);
		else			sprintf(user, "%d", std->st_uid);

		// Parseo del nombre del grupo
		if(grp!=NULL)	sprintf(group, "%s", grp->gr_name);
		else			sprintf(group, "%d", std->st_gid);

		// Parseo del tiempo de acceso
		if(accp==1)		tiempoLocal = *localtime(&(std->st_atime));
		else			tiempoLocal = *localtime(&(std->st_ctime));
		if(strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", &tiempoLocal)==0) return;

		// Printeo parcial/completo de los datos por pantalla
		printf("%s %3ld (%8ld) %8s %8s %s%8ld %s", fecha, std->st_nlink, std->st_ino, user, group, permisos, std->st_size, name);

		// Comprobando el linkeo
		if(linkp==1 && permisos[0]=='l'){
			readlink(name, linkName, sizeof(linkName));
			printf(" -> %s", linkName);
		}

	}else
		printf("\t%ld\t%s", std->st_size, name);
	printf("\n");
}
int cmdStat(const int lenArg, char *args[COMMAND_LEN]){
	struct stat *std;
	int i=1, numFiles=0, longp=0, accp=0, linkp=0;

	// Longitud del comando
	if(lenArg==1)
		return currentDirectory();

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

	// Comprobando que hay archivos a los que hacer un status
	if(numFiles==0)
		return currentDirectory();
	
	// Reservamos memoria para el stat -> Falta imprimir por pantalla el error de memoria
	if((std=(struct stat *)malloc(sizeof(struct stat)))==NULL) return 1;

	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], std)==0)
			print_stat_info(args[i], std, longp, accp, linkp);
		else
			if(args[i][0]!='-')
				printf(" ****error al acceder a %s:No such file or directory\n", args[i]);
	}

	free(std);
	return 1;
}

int cmdList(const int lenArg, char *args[COMMAND_LEN]){
	// Code
	return 1;
}

int cmdDelete(const int lenArg, char *args[COMMAND_LEN]) {
    
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


int cmdDeltree(const int lenArg, char *args[COMMAND_LEN]){
	// Code
	return 1;
}
