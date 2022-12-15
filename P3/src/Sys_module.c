#define _GNU_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1
#include <pwd.h>			// Aporta la definicion de datos de la estructura passwd
#include <errno.h>			// Librería de captador de errores
#include <fcntl.h>			// Librería para operaciones con archivos
#include <stdio.h>			// Librería estándar de entrada/salida
#include <stdlib.h>			// Librería de conversión, memoria, procesos...
#include <string.h>			// Librería de tratamiento de "strings"
#include <unistd.h>			// Librería de funcionalidades del sistema
#include <sys/shm.h>		// Librería con utilidades para la memoria compartida
#include <sys/mman.h>		// Librería para la declaración de gestiones de memoria
#include <sys/stat.h>		// Obtener información de los archivos
#include <sys/wait.h>		// Incluye funciones wait
#include <sys/types.h>		// Obtiene los tipos de datos del sistema

#include "List.h"			// Librería con las funcionalidades de la lista
#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

#include "p3.h"				// Librería con las cabeceras de la práctica 3

extern struct cmd_data cmd_table[];
extern struct cmd_help_data cmd_help[];

// ==================== MÉTODOS DEL SISTEMA ====================
void freed_list_memory(List historicList, List memoryList, List processList){
	// Elimina la lista del historico
	deleteList(historicList, free);

	// Elimina la lista de la memoria
	deleteList(memoryList, freeMemoryListItem);

	// Elimina la lista de memoria
	deleteList(processList, freeProcessListItem);
}
int executeCommand(const int numTrozos, char *tokens[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=0;

	// Comprobando comando no introducido
	if(numTrozos==0) return SSUCC_EXIT;

	// Obtenemos la posicion en la tabla de comandos del comando a ejecutar
	while(cmd_table[i].cmd_name!=NULL && strcmp(cmd_table[i].cmd_name, tokens[0])!=0)
		++i;

	// Comprobando si es una posicion erronea
	if(cmd_table[i].cmd_name!=NULL)

		// Función no implementada en la shell
		if(cmd_table[i].cmd_func == NULL)
			report_error_exit(ENOSYS);
		else
			return cmd_table[i].cmd_func(numTrozos, tokens, envp, historicList, memoryList, processList);

	else
		return external_functionality(numTrozos, tokens, envp, historicList, memoryList, processList);
	return SSUCC_EXIT;
}
void controled_exit(List historicList, List memoryList, List processList, int exitCode){
	freed_list_memory(historicList, memoryList, processList);	// Liberar memoria de las listas
	exit(exitCode);
}
int TrocearCadena(char *line, char *tokens[]){
	register int i = 1;
	if ((tokens[0]=strtok(line," \n\t"))==NULL)
		return 0;
	while((tokens[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}
void check_init_lists(List historicList, List memoryList, List processList){
	// Comprobamos que la lista del histórico está inicializada
	if(historicList == NULL)	createList(&historicList);

	// Comprobamos que la lista de la memoria está inicializada
	if(memoryList == NULL)		createList(&memoryList);

	// Comprobamos que la lista de procesos está inicializada
	if(processList == NULL)		createList(&processList);
}
int report_error_exit(int errorCode){
	if(errorCode<=0 || errorCode>131)	printf("[!] Error: invalid errorCode\n");
	else	printf("[!] Error: %s\n", strerror(errorCode));
	return FSUCC_EXIT;
}
void printPrompt(int exitStatus){
	printf("[#]~$ ");
	//printf("-> ");	// Prompt de la shell de referencia
}
void getCmdLine(char linea[COMMAND_BUFFER], int *argLen, char *args[PHARAM_LEN], List historicList, List memoryList, List processList){
	char get_input[COMMAND_BUFFER];

	// Comprobar que las listas están inicializadas
	check_init_lists(historicList, memoryList, processList);

	// Obteniendo el string de los comandos
	if(fgets(get_input, COMMAND_BUFFER, stdin)==NULL){
		printf("\n[!] Fin...\n");
		controled_exit(historicList, memoryList, processList, SCSS_EXIT);
	}

	// Comprobación del comando introducido
	if(strcmp(get_input, "\n")!=0 && get_input!=NULL){
		char *token = strtok(get_input, "\n");
		strcpy(linea, token);

		// Insertar en la lista
		if(!insertElement(historicList, strndup(linea, COMMAND_BUFFER)))
			printf("[!] Error: %s\n", strerror(ENOMEM));

		// Separar en trozos la cadena de texto introducida
		*argLen = TrocearCadena(linea, args);

	// En caso de no ser válido, la línea se vacía y el número de argumentos se pone a 0
	}else{
		strcpy(linea, " ");
		*argLen = 0;
	}
}

// ================ COMANDOS PRÁCTICA 0 ===============
struct tm currentTime(){
	time_t current_time = time(NULL);
	return *localtime(&current_time);
}
char *currentDirectory(){
	static char path[COMMAND_BUFFER];
	return (getcwd(path, COMMAND_BUFFER)==NULL)? NULL : path;
}
char *currentDate(char *format){
	struct tm tiempoLocal = currentTime();
	static char current_date[MAX_DATE_SIZE];
	sprintf(current_date, format, tiempoLocal.tm_mday, tiempoLocal.tm_mon+1, tiempoLocal.tm_year+1900);
	return (current_date);
}
char *currentHour(char *format){
	struct tm tiempoLocal = currentTime();
	static char current_hour[MAX_HOUR_SIZE];
	sprintf(current_hour, format, tiempoLocal.tm_hour, tiempoLocal.tm_min, tiempoLocal.tm_sec);
	return (current_hour);
}

// ==================== PRÁCTICA 1 ====================
int isDir(const char *path){
    struct stat s;
    stat(path, &s);
    int out = S_ISDIR(s.st_mode);
    return out;
}
char LetraTF(mode_t m){
	switch (m&S_IFMT){	// and bit a bit con los bits de formato,0170000
		case S_IFSOCK:	return 's'; 	// socket
		case S_IFLNK:	return 'l';		// symbolic link
		case S_IFREG:	return '-';		// fichero normal
		case S_IFBLK:	return 'b';		// block device
		case S_IFDIR:	return 'd';		// directorio
		case S_IFCHR:	return 'c';		// char device
		case S_IFIFO:	return 'p';		// pipe
		default: return '?';			// desconocido, no deberia aparecer
	}
}
char * ConvierteModo2(mode_t m){
	static char permisos[12];
	strcpy (permisos,"---------- ");

	permisos[0]=LetraTF(m);
	if (m&S_IRUSR) permisos[1]='r';    // propietario
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r';    // grupo
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r';    // resto
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s';    // setuid, setgid y stickybit
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';

	return permisos;
}

// ==================== PRÁCTICA 2 ====================
void freeMemoryListItem(void *data){	// [!]
	t_mem *item = (t_mem *)data;
	int id;

	switch(item->type){
		case SHARED_MEM:
			if((id=shmget(item->key,0,0666))==-1)
				perror("shmget: imposible obtener memoria compartida");
			else if(shmctl(id,IPC_RMID,NULL)==-1)
				perror("shmctl: imposible eliminar memoria compartida\n");
			break;

		case MAPPED_MEM:
			if(munmap(item->dir, item->size)!=-1){
				close(item->file_descriptor);
				free(item->file_name);
			}else
				printf("[!] Error: %s\n", strerror(errno));
			break;

		case NOT_DEFINED:
			break;

		case MALLOC_MEM:
		default:
			free(item->dir);
			break;
	}

	free(item);
}
void * ObtenerMemoriaShmget(key_t clave, size_t tam, t_mem *item, List memoryList){
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

	if((p=shmat(id,NULL,0))==(void*)-1){
		aux=errno;

		if(tam)
			shmctl(id,IPC_RMID,NULL);

		errno=aux;
		return (NULL);
	}

	shmctl(id,IPC_STAT,&s);

	item->size = s.shm_segsz;
	item->dir = p;

	if(!insertElement(memoryList, item)){
		printf("[!] Error: %s\n", strerror(ENOMEM));
		free(item);
		return (NULL);
	}

	return (p);
}
void * MapearFichero(char * fichero, int protection, List memoryList){
	int df, map = MAP_PRIVATE, modo = O_RDONLY;
	struct stat s;
	void *p;

	time_t currentTime = time(NULL);
	t_mem *nwItem = (t_mem *)malloc(sizeof(t_mem));

	if(protection&PROT_WRITE)
		modo=O_RDWR;

	if(stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
		return NULL;

	if((p=mmap(NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
		return NULL;

	nwItem->dir = p;
	nwItem->size = s.st_size;
	nwItem->time = *localtime(&currentTime);
	nwItem->type = MAPPED_MEM;
	nwItem->key = 0;
	nwItem->file_name = strdup(fichero);
	nwItem->file_descriptor = df;

	if(!insertElement(memoryList, nwItem))
		printf("[!] Error: %s\n", strerror(ENOMEM));
	return p;
}
void Do_pmap(){
	pid_t pid;
	char elpid[32];
	char *argv[4]={"pmap",elpid,NULL};

	sprintf(elpid,"%d", (int)getpid());

	if((pid=fork())==-1){
		perror ("Imposible crear proceso");
		return;
	}

	if(pid==0){
		if(execvp(argv[0],argv)==-1)
			perror("cannot execute pmap (linux, solaris)");

		argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;
		if(execvp(argv[0],argv)==-1)	//No hay pmap, probamos procstat FreeBSD
			perror("cannot execute procstat (FreeBSD)");

		argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;
		if(execvp(argv[0],argv)==-1)	//probamos procmap OpenBSD
			perror("cannot execute procmap (OpenBSD)");

		argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
		if(execvp(argv[0],argv)==-1)	//probamos vmmap Mac-OS
			perror("cannot execute vmmap (Mac-OS)");
		exit(1);
	}
	waitpid(pid,NULL,0);
}
char *t_asigntoa(t_asign asign){
	static char asign_name[7];
	strcpy(asign_name, "no_def");
	if(asign == MALLOC_MEM)	strcpy(asign_name, "malloc");
	if(asign == SHARED_MEM)	strcpy(asign_name, "shared");
	if(asign == MAPPED_MEM)	strcpy(asign_name, "mapped");
	return asign_name;
}

void do_DeallocateDelkey(char *args){
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
void Recursiva(int n){
	char automatico[TAMANO];
	static char estatico[TAMANO];

	printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, (void *)&n, automatico, estatico);

	if(n>0)
		Recursiva(n-1);
}
int LeerFichero(char *fich, void *p, int n){ // leen n bytes del fichero fich en p
    int nleidos,tam=n; //si n==-1 lee el fichero completo
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
int EscribirFichero(char *fich, void *p, int n){
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
void LlenarMemoria(void *p, size_t cont, unsigned char byte){
  unsigned char *arr = (unsigned char *)p;
  size_t i;

  for (i=0; i<cont;i++)
		arr[i]=byte;

  printf("Llenando %ld bytes de memoria con el byte %d(\'%c\') a partir de la direccion %p\n", (unsigned long) cont, byte, byte, p);
}

// ==================== PRÁCTICA 3 ====================
// [!] Crear cuando se cree el tipo de dato
void freeProcessListItem(void *data){
	t_proc *item = (t_proc *)data;
	free(item->line);
	free(item);
}

char *t_stattoa(t_pstat stat){
	static char asign_name[9];
	strcpy(asign_name, "UNKNOWN");
	if(stat == FINISHED)	strcpy(asign_name, "FINISHED");
	if(stat == STOPPED)		strcpy(asign_name, "STOPPED");
	if(stat == SIGNALED)	strcpy(asign_name, "SIGNALED");
	if(stat == ACTIVE)		strcpy(asign_name, "ACTIVE");
	return asign_name;
}

int BuscarVariable(char *var, char *e[]){
	int pos=0;
	char aux[PHARAM_LEN];
	strcpy(aux,var);
	strcat(aux,"=");

	while(e[pos]!=NULL)
		if(!strncmp(e[pos],aux,strlen(aux)))
			return(pos);
		else
			pos++;

	errno=ENOENT;   // no hay tal variable
	return(-1);
}

int CambiarVariable(char *var, char *valor, char *e[]){
	int pos;
	char *aux;

	if((pos=BuscarVariable(var,e))==-1)
		return(-1);

	if((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
		return -1;
	strcpy(aux,var);
	strcat(aux,"=");
	strcat(aux,valor);
	e[pos]=aux;
	return (pos);
}

int external_functionality(const int argLen, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=0, j=0;
	int output_code=SSUCC_EXIT, prior=0;
	char *new_envp[PHARAM_LEN];
	static char linea[COMMAND_BUFFER]="";
	pid_t pid;
	t_proc *nwItem=NULL;

	// Obteniendo las variables de entorno
	for(i=0; BuscarVariable(args[i], envp)!=-1; ++i)
		new_envp[i] = args[i];

	// Creando el proceso
	// EL proceso no se pudo crear correctamente
	if((pid=fork()) == -1)
		return report_error_exit(ESRCH);

	// Proceso hijo
	else if(pid==0){
		// Parseando el segundo plano del programa y la prioridad del programa 
		if(args[argLen-2] && args[argLen-2][0] == '@')
			args[argLen-2] = NULL;

		if(args[argLen-1] && (args[argLen-1][0] == '&' || args[argLen-1][0] == '@'))
			args[argLen-1] = NULL;

		if(i==0)	output_code = execvp(args[i], args+i);
		else		output_code = execvpe(args[i], args+i, new_envp);
		return SCSS_EXIT;	// Slida controlada de la shell

	// Proceso padre
	}else if(pid>0){
		if(args[argLen-1][0] == '&'){
			if((nwItem=(t_proc *)malloc(sizeof(t_proc)))==NULL){
				free(nwItem);
				return report_error_exit(ENOMEM);
			}

			// Línea del comando
			strcpy(linea, "");
			for(j=0; j<argLen-1; ++j){
				strcat(linea, args[j]);
				strcat(linea, " ");
			}

			// Parseando la prioridad del proceso
			if(args[argLen-2] && args[argLen-2][0]=='@'){
				args[argLen-2][0] = '0';
				prior = atoi(args[argLen-2]);
				args[argLen-2] = NULL;
			}

			// Crear el item
			nwItem->pid = pid;
			nwItem->user = getpwuid((uid_t)getuid())->pw_name;
			nwItem->time = currentTime();
			nwItem->status = ACTIVE;
			nwItem->end = ACTIVE;
			nwItem->priority = prior;
			nwItem->line = strdup(linea);

			// Insertar en la lista de procesos el nuevo proceso
			if(!insertElement(processList, nwItem))
				printf("[!] Error: %s\n", strerror(ENOMEM));
		}else
			waitpid(pid, &output_code, 0);
	}

	return (output_code == SCSS_EXIT)? SSUCC_EXIT : FSUCC_EXIT ;
}