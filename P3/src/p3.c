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

int update_data(t_proc *item){
	int options = WNOHANG | WUNTRACED | WCONTINUED;
	int status;

	if(waitpid(item->pid, &status, options)==item->pid){
		if(WIFEXITED(status)){
			item->status = FINISHED;
			item->end = WEXITSTATUS(status);
			item->end = -1;
		}else if(WIFCONTINUED(status)){
			item->status = ACTIVE;
		}else if(WIFSTOPPED(status)){
			item->status = STOPPED;
			item->end = WTERMSIG(status);
		}else if(WIFSIGNALED(status)){
			item->status = SIGNALED;
			item->end = WTERMSIG(status);
		}else
			item->status = UNKNOWN;
		return 1;
	}
	
	return 0;
}

void print_proc_dataItem(t_proc *item){
	printf("%6d %11s ", item->pid, item->user);
	printf("p=%d ", item->priority);
	printf("%4d/%2d/%2d %2d:%2d:%2d ", item->time.tm_year+1900, item->time.tm_mon+1, item->time.tm_mday, item->time.tm_hour, item->time.tm_min, item->time.tm_sec);
	printf("%s (%03d) %s\n", t_stattoa(item->status), item->end, item->line);
}

void imprimir_lista_procesos(List processList, pid_t pid){
	Lpos auxPos;
	t_proc *item = NULL;

	for(auxPos=firstElement(processList); auxPos!=NULL; auxPos=nextElement(processList, auxPos)){
		item = getElement(processList, auxPos);
		if(pid<0 || pid==item->pid){
			if(update_data(item))
				updateElement(processList, auxPos, item);
			print_proc_dataItem(item);
		}
	}
}

// ==================== PRÁCTICA 3 ====================
// [✔] Hecha
int cmdPriority(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	
	int priority, pid;
    if(lenArg >1){
        pid=atoi(args[1]);
        if(lenArg ==2){
            priority=getpriority(PRIO_PROCESS,pid);
        }else{
            priority=atoi(args[2]);
            setpriority(PRIO_PROCESS, pid, priority);
        }
    }else{
        pid=getpid();
        priority=getpriority(PRIO_PROCESS,pid);
    }
    printf("El proceso %d tiene prioridad %d\n", pid, priority);
    
    return SSUCC_EXIT;
}

// [✔] Hecha
int cmdShowvar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	
	int pos;
    if(lenArg>1){
        if((pos=BuscarVariable(args[1],environ))==-1){
            perror("Imposible encontrar variable");
            return FSUCC_EXIT;
        }
        printf("Con arg3 main %s(%p) @%p\n",environ[pos],environ[pos],&envp[pos]);
        printf("  Con environ %s(%p) @%p\n",environ[pos],environ[pos],&environ[pos]);
        printf("   Con getenv %s(%p)\n",getenv(args[0]),&environ[pos]);
    }
    else{
        cmdShowenv(1,0,envp,historicList,memoryList,processList);}
    
    return SSUCC_EXIT;
}

// [✔] Hecha
int cmdChangevar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	char *aux=malloc(MAX_PATH);
    if(lenArg>1){
        if(lenArg==4){
            if(strcmp(args[1],"-a")==0){
                CambiarVariable(args[2],args[3],envp);
            }else if(strcmp(args[1],"-e")==0){
                CambiarVariable(args[2],args[3],environ);
            }else if(strcmp(args[1],"-p")==0){
                strcpy(aux,args[2]);
                strcat(aux,"=");
                strcat(aux,(args[3]));
                putenv(aux);
            }else
                printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
        }else
            printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
    }else
        printf("Uso: cambiarvar [-a|-e|-p] var valor\n");
    
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

// [✔] Hecha
int cmdFork(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	int pid=fork();
	if(pid==0)
		printf("ejecutando proceso %d\n",getpid());
	else if(pid==-1){
		perror("[!] Error: fork no posible");
		return FSUCC_EXIT;}
	else waitpid(pid,NULL,0);
	return SSUCC_EXIT;
}

// [+] Hacer
int cmdExecute(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=1, j=1,n,pid;
	register char bgp=0;
	char *specific_environ[PHARAM_LEN], *token=NULL;
	t_proc *nwItem=NULL;

	// Si solo se llama a esta función
	if(lenArg==1)
		return report_error_exit(EFAULT);

	// Coger el nuevo entorno
	for(i=1; BuscarVariable(args[i], envp)!=-1; ++i)
		specific_environ[i-1] = args[i];

	// Comprobando la flag de prioridad
	if(lenArg>=3){
		for(n=2;n<lenArg-1;n++){
			if(strchr(args[n],'@')!=NULL){
				if((*token=strtok(args[n],"@")!=NULL))
					setpriority(PRIO_PROCESS,pid=getpid(),atoi(token));
				
			}

		}
	}
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

		args[lenArg-1]=NULL;

		// Cambiar la flag a activo
		bgp=1;
		printf("[%s] => %d", nwItem->line, bgp);
	}


	// Ejecución del programa
	if(execvpe(args[i], args+i, (i==1)? envp : specific_environ)==-1)
		printf("[!] Error: %s\n", strerror(errno));

	return SSUCC_EXIT;
}

// [✔] Hecha
int cmdListjobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	imprimir_lista_procesos(processList, -1);
	return SSUCC_EXIT;
}

// [!] Hacer
int cmdDeljobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	// Code
	return SSUCC_EXIT;
}

// [✔] Hecha
int cmdJob(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	Lpos auxPos;
	t_proc *item=NULL;
	int exit_status=0;
	pid_t pid;

	// En caso de que sea solo un argumento
	if(lenArg==1)
		imprimir_lista_procesos(processList, -1);

	// En caso que se desee pasar de segundo plano a primer plano
	else if(strcmp(args[1], "-fg")==0){
		if((pid=atoi(args[2]))!=0){
			// Pasamos a primer plano el proceso indicado
			waitpid(pid, &exit_status, 0);
			printf("Proceso %d terminado normalmente. Valor devuelto %d\n", pid, exit_status);

			// Eliminar de la lista
			for(auxPos=firstElement(processList); auxPos!=NULL; auxPos=nextElement(processList, auxPos)){
				item = getElement(processList, auxPos);
				if(pid == item->pid){
					deletePosition(processList, auxPos);
					break;
				}
			}
		}
		else return report_error_exit(EINVAL);
	
	// Obtener informacion concreta de un proceso
	}else if((pid=atoi(args[1]))!=0)
		imprimir_lista_procesos(processList, pid);

	return SSUCC_EXIT;
}
