#include <errno.h>			// Librería de captador de errores
#include <stdio.h>			// Librería estándar de entrada/salida
#include <stdlib.h>			// Librería de conversión, memoria, procesos...
#include <string.h>			// Librería de tratamiento de "strings"
#include <unistd.h>			// Librería de funcionalidades del sistema
#include <sys/utsname.h>	// Obtiene informacñon del sistema [LINUX]

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica
#include "p0.h"				// Librería con las cabeceras de la práctica

extern struct cmd_help_data cmd_help[];

// ===== Métodos semi-privados de la práctica =====
int checkZeroPharam(char arg[PHARAM_LEN]){
	int n=-1;
	arg[0]='0';
	return ((n=atoi(arg))<=0 && arg[1]!='0')? -1 : n;
}
void printNcommands(int n, List historicList){
	Lpos auxPos;
	register int iter=0;

	if(n<0) n = getSize(historicList);
	for(auxPos=firstElement(historicList); auxPos!=NULL && iter<n; ++iter, auxPos=nextElement(historicList, auxPos))
		printf("%d->%s\n", iter, (char *)getElement(historicList, auxPos));
}

// ================ COMANDOS PRÁCTICA 0 ===============
int cmdAutores(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)						printf("Adrian Pardo Martinez: adrian.pardo.martinez\nHugo Correa Blanco: hugo.correa.blanco\n");
	else if(strcmp("-l", args[1])==0)	printf("adrian.pardo.martinez\nhugo.correa.blanco\n");
	else if(strcmp("-n", args[1])==0)	printf("Adrian Pardo Martinez\nHugo Correa Blanco\n");
	else								return report_error_exit(EINVAL);
	return SSUCC_EXIT;
}
int cmdPid(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)						printf("Pid de shell: %d\n", getppid());
	else if(strcmp(args[1], "-p")==0)	printf("Pid del padre del shell: %d\n", getpid());
	else								return report_error_exit(EINVAL);
	return SSUCC_EXIT;
}
int cmdCarpeta(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1)				printf("%s\n", currentDirectory());
	else if(chdir(args[1])!=0)	return report_error_exit(ENOENT);
	return SSUCC_EXIT;
}
int cmdFecha(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	char *hourFormat="%02d:%02d:%02d", *dateFormat="%02d/%02d/%04d";
	if(lenArg==1)						printf("%s\n%s\n", currentHour(hourFormat), currentDate(dateFormat));
	else if(strcmp(args[1], "-d")==0)	printf("%s\n", currentDate(dateFormat));
	else if(strcmp(args[1], "-h")==0)	printf("%s\n", currentHour(hourFormat));
	else								return report_error_exit(EINVAL);
	return SSUCC_EXIT;
}
int cmdHist(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg==1){
		printNcommands(-1, historicList);
		return SSUCC_EXIT;
	}
	if(strcmp(args[1], "-c")==0){
		clearList(historicList, free);
		return SSUCC_EXIT;
	}

	int n=checkZeroPharam(args[1]);
	if(n<0)	return report_error_exit(EINVAL);
	else	printNcommands(n, historicList);

	return SSUCC_EXIT;
}
int cmdComando(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	char linea[COMMAND_BUFFER]="";
	int nCommand=0;
	Lpos auxPos;
	
	if(lenArg>1){
		if((nCommand=checkZeroPharam(args[1]))<0)
			return report_error_exit(EINVAL);

		// Obtenemos el elemento en la posicion indicada
		// Comprobamos si la posicion noes valida
		if((auxPos = getPos(historicList, nCommand))==NULL)
			return report_error_exit(EOPNOTSUPP);

		strcpy(linea, getElement(historicList, auxPos));
		printf("Ejecutando hist (%d): %s\n", nCommand, linea);

		// Separar en trozos la cadena de texto introducida
		return executeCommand(TrocearCadena(linea, args), args, envp, historicList, memoryList, processList);
	}

	printNcommands(-1, historicList);
	return SSUCC_EXIT;
}
int cmdInfosys(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	struct utsname systemData;
	if(uname(&systemData)==-1)	return report_error_exit(ENODATA);
	else
		printf("%s (%s), OS: %s-%s-%s\n", systemData.nodename, systemData.machine, systemData.sysname, systemData.release, systemData.version);
	return SSUCC_EXIT;
}
int cmdHelp(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=0;
	if(lenArg==2){
		while(cmd_help[i].cmd_name!=NULL && strcmp(cmd_help[i].cmd_name, args[1])!=0)
			++i;
		if(cmd_help[i].cmd_name==NULL)
			return report_error_exit(ENOSYS);
		else
			printf("%s %s%s", cmd_help[i].cmd_name, cmd_help[i].cmd_pharams!=NULL? cmd_help[i].cmd_pharams : "", cmd_help[i].cmd_usage);
	}else{
		printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n");
		for(i=0; cmd_help[i].cmd_name!=NULL; ++i)
			printf("%s ", cmd_help[i].cmd_name);
		printf("\n");
	}
	return SSUCC_EXIT;
}
int cmdExit(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	return SCSS_EXIT;
}