#include <grp.h>			// Aporta la definicion de datos de los grupos de linux
#include <pwd.h>			// Aporta la definicion de datos de la estructura passwd
#include <errno.h>			// Librería de captador de errores
#include <fcntl.h>			// Librería para operaciones con archivos
#include <stdio.h>			// Librería estándar de entrada/salida
#include <dirent.h>			// Librería que importa las entradas de directorios
#include <stdlib.h>			// Librería de conversión, memoria, procesos...
#include <string.h>			// Librería de tratamiento de "strings"
#include <unistd.h>			// Librería de funcionalidades del sistema
#include <sys/stat.h>		// Obtener información de los archivos

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica
#include "p1.h"				// Librería con las cabeceras de la práctica

// ===== Métodos semi-privados de la práctica =====
int borrarDir(char *dir){
    DIR *dirp;
    struct dirent *flist;
    char aux[MAX_NAME_LEN];

    if((dirp=opendir(dir))==NULL) return -1;

    while((flist=readdir(dirp))!=NULL){ 	//Recorre el directorio
        strcpy(aux, dir);
        strcat(strcat(aux, "/"),flist->d_name);

        if(strcmp(flist->d_name, "..")==0 || strcmp(flist->d_name, ".")==0) continue;

        if(isDir(aux)) 		//Recursivo
            borrarDir(aux);
        if(remove(aux))return -1; 	
    }

    closedir(dirp);
    return 0;
}
void print_file_info(const char *name, const char *allPath, const struct stat *std, short longp, short accp, short linkp){	
	if(longp){
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
		tl = *localtime((accp)? &(std->st_atime) : &(std->st_ctime));

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
void print_dir_data(const char *name, short hidp, short longp, short accp, short linkp){
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
				if(hidp==1 || ent->d_name[0]!='.')
					print_file_info(ent->d_name, inn_name, &fStd, longp, accp, linkp);
			}
		}

		// Cerramos el directorio
		closedir(dir);
	}else
		printf("[!] Error: No se puede abrir el directorio\n");
}
void process_dir_data(const char *name, short recap, short recbp, short hidp, short longp, short accp, short linkp){
	DIR *dir;
	register struct dirent *ent=NULL;
	struct stat fStd;
	char inn_name[PHARAM_LEN];

	// Abrimos el directorio
	if((dir=opendir(name))!=NULL){

		// Recorremos los archivos del directorio
		while((ent=readdir(dir))!=NULL){

			// Parseo de los directorios . y ..  -> por la recursividad
			if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0)	continue;

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
void list_fd_data(const char *name, const struct stat *std, short recap, short recbp, short hidp, short longp, short accp, short linkp){
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

// ================ COMANDOS PRÁCTICA 1 ===============
int cmdCreate(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	int fd;

	if(lenArg>1){
		if(strcmp(args[1],"-f")==0){				// Si se pasa con -f se crea un archivo
			if((fd = creat(args[2],0644)) == -1)
				printf("[!] Error: %s\n", strerror(ENOENT));
			else
				close(fd);
		}else if(mkdir(args[1], 0755)!=0)	// Si solo se pasa el nombre se crea un directorio
			printf("[!] Error: %s\n", strerror(ENOENT));
	}else
		return report_error_exit(ENOENT);
	return SSUCC_EXIT;
}
int cmdStat(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	struct stat std;
	register int i=1;
	short numFiles=0, longp=0, accp=0, linkp=0;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-long")==0)			longp=1;
			else if(strcmp(args[i], "-acc")==0)		accp=1;
			else if(strcmp(args[i], "-link")==0)	linkp=1;
		}else
			++numFiles;
		++i;
	}

	// Longitud del comando o falta de archivos a los que hacer un status
	if(lenArg==1 || numFiles==0){
		printf("%s\n", currentDirectory());
		return SSUCC_EXIT;
	}

	// Stat para todos los archivos que se pasen como parámetros
	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], &std)==0)
			print_file_info(args[i], NULL, &std, longp, accp, linkp);
		else if(args[i][0]!='-')
			printf(" ****error al acceder a %s:No such file or directory\n", args[i]);
	}
	return SSUCC_EXIT;
}
int cmdList(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register int i=1;
	short recap=0, recbp=0, hidp=0, longp=0, accp=0, linkp=0;
	int numData=0;
	struct stat std;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-reca")==0)	recap=1;
			if(strcmp(args[i], "-recb")==0)	recbp=1;
			if(strcmp(args[i], "-hid")==0)	hidp=1;
			if(strcmp(args[i], "-long")==0)	longp=1;
			if(strcmp(args[i], "-acc")==0)	accp=1;
			if(strcmp(args[i], "-link")==0)	linkp=1;
		}else
			++numData;
		++i;
	}

	// Longitud del comando o falta de archivos a los que hacer un status
	if(lenArg==1 || numData==0){
		printf("%s\n", currentDirectory());
		return SSUCC_EXIT;
	}

	// Procesamos los archivos pasados como parametros
	for(i=1; i<lenArg; ++i){
		if(lstat(args[i], &std)==0)
			if(S_ISDIR(std.st_mode))
				list_fd_data(args[i], &std, recap, recbp, hidp, longp, accp, linkp);
			else
				print_file_info(args[i], NULL, &std, longp, accp, linkp);
		else if(args[i][0]!='-')
			printf(" ****error al acceder a %s:No such file or directory\n", args[i]);
	}
	return SSUCC_EXIT;
}
int cmdDelete(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg >1){		//Borra el archivo o la carpeta
		for(register int i=1; i<lenArg; i++)
			if(remove(args[i]) != 0)
				printf("[!] Error al acceder a %s:No such file or directory\n",args[i]);

	}else				// Muestra directorio actual
		printf("%s\n", currentDirectory());
	return SSUCC_EXIT;
}
int cmdDeltree(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
    if(lenArg >1){
        for(register int i=1; i<lenArg; i++){
            if(isDir(args[i])){
                if(borrarDir(args[i])==-1 || remove(args[i]))
                    perror("[!] Error");
            }else if(remove(args[i]))
                perror("[!] Error");
        }
    }else		//Muestra el directorio actual
		printf("%s\n", currentDirectory());
	return SSUCC_EXIT;
}