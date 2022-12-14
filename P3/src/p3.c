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
	// Code
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
