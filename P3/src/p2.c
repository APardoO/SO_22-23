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
#include "p2.h"				// Librería con las cabeceras de la práctica

extern struct cmd_data cmd_table[];
extern struct cmd_help_data cmd_help[];

// Variable global para la implementación de memory
int int_g=1;

// ===== Métodos semi-privados de la práctica =====
void do_AllocateMmap(char *fich, char *permisos, List memoryList){ 
	char *perm;
	void *p;
	int protection=0;

	if(fich==NULL){
		print_memList(MAPPED_MEM, memoryList);
		return;

	}if((perm=permisos)!=NULL && strlen(perm)<4){
		if(strchr(perm,'r')!=NULL) protection|=PROT_READ;
		if(strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
		if(strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
	}	
	
	if((p=MapearFichero(fich,protection, memoryList))==NULL)
		perror ("Imposible mapear fichero");
	else
		printf ("fichero %s mapeado en %p\n", fich, p);
}
void print_data_item(t_mem *item){
	char time_format[MAX_NAME_LEN];
	strftime(time_format, MAX_NAME_LEN, "%b %d %R", &item->time);

	printf("\t%p\t%13lu %s", item->dir, (unsigned long) item->size, time_format);
	if(item->type == MALLOC_MEM)	printf(" %s", t_asigntoa(item->type));
	if(item->type == SHARED_MEM)	printf(" %s (key %u)", t_asigntoa(item->type), item->key);
	if(item->type == MAPPED_MEM)	printf(" %s (descriptor %d)", item->file_name, item->file_descriptor);
	printf("\n");
}
void print_memList(t_asign asign, List memoryList){
	Lpos auxPos;
	t_mem *auxItem = NULL;

	printf("******Lista de bloques asignados shared para el proceso %d\n", getppid());

	for(auxPos = firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
		auxItem = getElement(memoryList, auxPos);

		// Se muestran por pantalla todas las reservas de memoria
		// En caso de necesitar una salida concreta -> parseo del tipo de memoria reservada
		if(asign == NOT_DEFINED || auxItem->type == asign)
			print_data_item(auxItem);
	}
}
int malloc_allocation(size_t size, List memoryList){
	t_mem *nwItem = (t_mem *)malloc(sizeof(t_mem));

	if(nwItem==NULL){
		free(nwItem);
		return report_error_exit(ENOMEM);
	}

	// Datos del item
	nwItem->size = size;
	nwItem->key = 0;
	nwItem->type = MALLOC_MEM;
	nwItem->file_name = NULL;
	nwItem->file_descriptor = 0;
	nwItem->time = currentTime();
	nwItem->dir = (void *)malloc(nwItem->size);

	// Comprobamos que podemos hacer la reservas
	if(nwItem->dir == NULL){
		printf("Imposible obtener memoria con malloc: Cannot allocate memory\n");
		free(nwItem);
		return FSUCC_EXIT;
	}

	if(!insertElement(memoryList, nwItem)){
		free(nwItem->dir);
		free(nwItem);
		return report_error_exit(ENOMEM);
	}

	printf("Asignados %lu bytes en %p\n", (unsigned long) nwItem->size, nwItem->dir);
	return SSUCC_EXIT;
}
int allocate_nshared_block(key_t key, size_t size, List memoryList){
	t_mem *nwItem = (t_mem *)malloc(sizeof(t_mem));

	if(nwItem==NULL){
		free(nwItem);
		return report_error_exit(ENOMEM);
	}

	// Datos del item
	nwItem->time = currentTime();
	nwItem->type = SHARED_MEM;
	nwItem->file_name = NULL;
	nwItem->file_descriptor = 0;
	nwItem->size = size;
	nwItem->key = key;

	// Comprobamos si se pudo cerear la memoria compartida
	if(ObtenerMemoriaShmget(nwItem->key, nwItem->size, nwItem, memoryList) == NULL){
		printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) nwItem->key, strerror(errno));
		free(nwItem);
		return FSUCC_EXIT;
	}

	printf("Asignados %lu bytes en %p\n", (unsigned long) nwItem->size, nwItem->dir);
	return SSUCC_EXIT;
}
int alloc_shared_block(key_t key, List memoryList){
	t_mem *nwItem = (t_mem *)malloc(sizeof(t_mem));

	if(nwItem==NULL){
		free(nwItem);
		return report_error_exit(ENOMEM);
	}

	// Datos del item
	nwItem->time = currentTime();
	nwItem->type = SHARED_MEM;
	nwItem->file_name = NULL;
	nwItem->file_descriptor = 0;
	nwItem->key = key;

	if(ObtenerMemoriaShmget(nwItem->key, 0, nwItem, memoryList)==NULL){
		printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long)nwItem->key, strerror(errno));
		free(nwItem);
		return FSUCC_EXIT;
	}

	printf("Memoria compartida de clave %lu  en %p\n", (unsigned long) nwItem->size, nwItem->dir);
	return SSUCC_EXIT;
}
int malloc_deallocation(size_t size, List memoryList){
	t_mem *infoData = NULL;
	Lpos auxPos;

	// Recorremos la lista hasta el primer bloque de memoria asignado con ese tamaño
	for(auxPos = firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
		infoData = getElement(memoryList, auxPos);
		if(infoData->type==MALLOC_MEM && infoData->size==size) break;
	}

	// Comprobación de una posición no válida
	if(auxPos == NULL)	return report_error_exit(ENODATA);

	// Eliminamos el elemento
	infoData = deletePosition(memoryList, auxPos);

	free(infoData->dir);
	infoData->size=0;
	infoData->type=NOT_DEFINED;
	infoData->key=0;
	infoData->file_name=NULL;
	infoData->file_descriptor=0;
	free(infoData);

	return SSUCC_EXIT;
}
int del_shared_block(key_t key, List memoryList){
	t_mem *infoData = NULL;
	Lpos auxPos;
	for(auxPos=firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
		infoData = getElement(memoryList, auxPos);
		if(infoData->key==key){
			infoData = deletePosition(memoryList, auxPos);
			if(shmdt(infoData->dir)==-1)
				printf("[!] Error: %s\n", strerror(errno));
			free(infoData);
			break;
		}
	}
	return (auxPos==NULL)? FSUCC_EXIT : SSUCC_EXIT;
}
int unmapped_memory_block(char *file_name, List memoryList){
	t_mem *infoData = NULL;
	Lpos auxPos;

	for(auxPos=firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
		infoData = getElement(memoryList, auxPos);
		if(strcmp(infoData->file_name, file_name)==0) break;
	}

	if(auxPos == NULL){
		printf("Fichero %s no mapeado\n", file_name);
		return FSUCC_EXIT;
	}else if((infoData = deletePosition(memoryList, auxPos))!=NULL && munmap(infoData->dir, infoData->size)!=-1){
		close(infoData->file_descriptor);
		free(infoData->file_name);
	}else
		return report_error_exit(errno);
	return SSUCC_EXIT;
}
int memory_direction_block(char *dir, List memoryList){
	t_mem *infoData = NULL;
	Lpos auxPos;
	char directory[PHARAM_LEN];

	// Comprobar si el primer parametro es una dirección de memoria en la cual se ha mapeado alguna dirección
	if(dir[0]=='0' && dir[1]=='x'){
		for(auxPos=firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
			infoData = getElement(memoryList, auxPos);
			sprintf(directory, "%p", infoData->dir);
			if(strcmp(dir, directory)==0) break;
		}

		if(auxPos==NULL){
			printf("Direccion %s no asignada con malloc, shared o mmap\n", dir);
			return FSUCC_EXIT;

		}else	freeMemoryListItem(deletePosition(memoryList, auxPos));
	}else{
		printf("Direccion 0x%s no asignada con malloc, shared o mmap\n", dir);
		return FSUCC_EXIT;
	}
	return SSUCC_EXIT;
}
int desc_special_charecter(unsigned int ui){
	switch(ui){
		case  7: case  8: case 12: return  90;		// \a \b \f
		case  9: case 11: return 107;				// \t \v
		case 10: return 100;						// \n
		case 13: return 101;						// \r
		case  0: return  48;						// \0
		default: return 0;
	}
}
void print_character(char c, const short char_format){
	const char format_printable[6] = " %*c";
	const char format_not_printable[6] = " \\%1c";
	unsigned int ch=(unsigned int)(unsigned char)c;

	if(isprint(ch) && ch!='\\')	printf(format_printable, char_format, c);
	else						printf(format_not_printable, (int)c+desc_special_charecter(ch));	// [!]
}
void print_memory_block(char *pos, int bytes, List memoryList){
	const register short char_format=2;
	const register int preformat_len=25;
	int i, j/*, cont=0, x=0*/;
	int size=preformat_len;
	void *addr=(void *)pos;
	t_mem *infoData=NULL;

	// Comprobamos que es una dirección válida
	if(addr!=NULL && (infoData=search_mem_pos(memoryList, addr))!=NULL){
		if(bytes>0){
			if((int)infoData->size>bytes)	size=bytes;
			else{
				size=infoData->size;
				printf("No se pueden volcat %d bytes, el maximo son %d bytes\n", bytes, size);
			}
		}else	size=infoData->size;

	}else{
		printf("Direccion no valida\n");
		return;
	}

	for(i=0; i<size; i+=preformat_len){
		for(j=i; j<size && j<i+preformat_len && pos[j]; ++j)
			print_character(pos[j], char_format);

		printf("\n");
		for(j=i; j<size && j<i+preformat_len && pos[j]; ++j)
			printf(" %0*x", char_format, pos[j]);
		printf("\n");
	}
}
t_mem *search_mem_pos(List memoryList, void *memdir){
	Lpos auxPos;
	t_mem *infoData=NULL;
	for(auxPos=firstElement(memoryList); auxPos!=NULL; auxPos=nextElement(memoryList, auxPos)){
		infoData = getElement(memoryList, auxPos);
		if(infoData->dir==memdir) break;
	}
	return (auxPos==NULL)? NULL : infoData;
}

// ================ COMANDOS PRÁCTICA 2 ===============
int cmdAllocate(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	const register short ctpos=16;
	size_t size;
	key_t key;

	if(lenArg==1)
		print_memList(NOT_DEFINED, memoryList);

	else if(strcmp(args[1], "-malloc")==0){
		if(lenArg==2)	print_memList(MALLOC_MEM, memoryList);

		// Comprobante de que se intenta hacer una reserva de 0 bytes
		else if((size = (size_t)strtoul(args[2], NULL, 10))==0){
			printf("No se asignan bloques de 0 bytes\n");
			return FSUCC_EXIT;

		// Se reserva la memoria e inserta el elemento en la lista de memoria, con la indicada
		}else	return malloc_allocation(size, memoryList);

	}else if(strcmp(args[1], "-createshared")==0){
		if(lenArg<4)	print_memList(SHARED_MEM, memoryList);

		// Comprobante de que se intenta hacer una reserva de 0 bytes
		else if((size = (size_t)strtoul(args[3], NULL, 10))==0){
			printf("No se asignan bloques de 0 bytes\n");
			return FSUCC_EXIT;

		// Comprobar si se pudo crear la llave de la memoria compartida
		}else if((key = (key_t)strtoul(args[2], NULL, 10))==0){
			printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) key, strerror(errno));
			return FSUCC_EXIT;

		// Se reserva un bloque de memoria compartida y se añade a la lista de memoria compartida (key y size generados)
		}else	return allocate_nshared_block(key, size, memoryList);

	}else if(strcmp(args[1], "-shared")==0){
		if(lenArg==2)	print_memList(SHARED_MEM, memoryList);

		// Comprobamos que es una clave válida
		else if((key = (key_t)strtoul(args[2], NULL, 10))==0){
			printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) key, strerror(errno));
			return FSUCC_EXIT;
		// Comprobamos que haya un segmento de memoria compartido con la clave (key), en caso contrario lo asigna
		}else	return alloc_shared_block(key, memoryList);

	}else if(strcmp(args[1], "-mmap")==0){
		if(lenArg<4)	print_memList(MAPPED_MEM, memoryList);
		else			do_AllocateMmap(args[2], args[3], memoryList);
	}else
		if(lenArg>1)	printf("uso: allocate %s ....\n", cmd_help[ctpos].cmd_pharams);
	return SSUCC_EXIT;
}
int cmdDeallocate(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	size_t tam;
	key_t llave;

	if(lenArg==1)
		print_memList(NOT_DEFINED, memoryList);
	else if(strcmp(args[1], "-malloc")==0){
		// En caso de que se pasen solo dos parametros, se muestra la lista solo con las reservas malloc
		if(lenArg==2)	print_memList(MALLOC_MEM, memoryList);

		// Se livera el primer bloque reservado de la memoria y se elimina de la lista
		// Comprobante de que se intenta hacer una reserva de 0 bytes
		else if((tam = (size_t)strtoul(args[2], NULL, 10))==0){
			printf("No se asignan bloques de 0 bytes\n");
			return FSUCC_EXIT;
		}else	return malloc_deallocation(tam, memoryList);
	
	}else if(strcmp(args[1], "-shared")==0){
		// En caso de pasar parametros incorrectos, se muestra la lista de memoria compartida
		if(lenArg<3)	print_memList(SHARED_MEM, memoryList);
		else if((llave=(key_t)strtoul(args[2], NULL, 10))!=0)		return del_shared_block(llave, memoryList);
		else			return report_error_exit(EINVAL);
	
	}else if(strcmp(args[1], "-delkey")==0){
		// En caso de pasar parametros incorrectos, se muestra la lista de memoria compartida
		if(lenArg<3)	print_memList(SHARED_MEM, memoryList);

		// Comprobante de que se intenta hacer una reserva de 0 bytes
		else if((llave=(key_t)strtoul(args[2], NULL, 10))!=0)		do_DeallocateDelkey(args[2]);
		else	return report_error_exit(EINVAL);
	
	}else if(strcmp(args[1], "-mmap")==0){
		// En caso de que nos pasen una cantidad de parámetros incorrecta
		if(lenArg<3)	print_memList(MAPPED_MEM, memoryList);
		else			return unmapped_memory_block(args[2], memoryList);

	}else{
		// Comprobar si el primer parametro es una dirección de memoria en la cual se ha mapeado alguna dirección
		if(lenArg==2)	return memory_direction_block(args[1], memoryList);
		else{
			printf("Direccion 0x%s no asignada con malloc, shared o mmap\n", args[1]);
			return FSUCC_EXIT;
		}
	}
	return SSUCC_EXIT;
}
int cmdIo(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){	// [!]
	int t;
	char *ptr;
	
	if(lenArg==1){
		printf("uso: e-s [read|write] ......\n");
		return FSUCC_EXIT;
	}
	if(lenArg<5){
		printf("Faltan parametros\n");
		return FSUCC_EXIT;
	}
	
	if(strcmp(args[1], "read")==0){
		int n = ((ssize_t)-1);
		long addr = strtoul(args[3],&ptr,16);
		n = atoi(args[4]);

		if((t = LeerFichero(args[2], (long *)addr, n))==-1)
			perror("error de lectura");
		else printf("leidos %d bytes en %ld\n",t,addr);

    }else if(strcmp(args[1], "write") == 0){
		if((strcmp(args[2], "-o") == 0)){
			creat(args[3], 0777);
			long addr = strtoul(args[4],&ptr,16);
			if((t = EscribirFichero(args[3], (long *)addr, atoi(args[5]))) == -1)
				perror("error de escritura");
			else printf("escritos %d bytes en %ld\n",t,addr);
                	
		}else if(open(args[2],O_RDWR) == -1){
			creat(args[2], 0777);
			long addr = strtoul(args[3],&ptr,16);
			if((t=EscribirFichero(args[2], (long *)addr, atoi(args[4]))) == -1)
				perror("error de escritura");
			else printf("escritos %d bytes en %ld\n",t,addr);
        }
    }
    return SSUCC_EXIT;
}
int cmdMemdump(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg<2) return FSUCC_EXIT;
	print_memory_block((char *)strtol(args[1], NULL, 16), (args[2])? (int)strtol(args[2], NULL, 10) : -1, memoryList);
	return SSUCC_EXIT;
}
int cmdMemfill(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	size_t size = (unsigned long)0;
	unsigned char c = 'A';
	void *addr=NULL;
	t_mem *infoData=NULL;

	if(lenArg==1) return SSUCC_EXIT;
	if((addr = (void *)strtoul(args[1], NULL, 16))==NULL || (infoData=search_mem_pos(memoryList, addr))==NULL){
		printf("Direccion no valida\n");
		return FSUCC_EXIT;
	}

	// [+] Esto se puede optimizar
	if(args[2]!=NULL && (size = (size_t)strtoul(args[2], NULL, 10))!=0){
		if(size>infoData->size){
			printf("No se pueden rellenar %ld bytes, el maximo son %ld\n", (unsigned long)size, infoData->size);
			size = infoData->size;
		}
	}else
		size = infoData->size;

	// Parseo de caracteres de la forma: "A" (como string), 'A' (como caracter) o 65 (como entero)
	if(args[3]!=NULL){
		if((args[3][0]=='\"' || args[3][0]=='\''))	c = args[3][1];
		else										c = atoi(args[3]);
	}

	LlenarMemoria(infoData->dir, size, c);
	return SSUCC_EXIT;
}
int cmdMemory(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	register short blockp=0, funcp=0, varsp=0, allp=0, pmapp=0;
	register int i=1;

	// Variables para el programa
	int int_l=1;
	char char_l='A';
	float float_l=1.2;
	static int int_s=1;
	static char char_s='A';
	static float float_s=1.2;

	// Parseo de parámetros
	while(i<lenArg){
		if(args[i][0]=='-'){
			if(strcmp(args[i], "-blocks")==0)		blockp=1;
			else if(strcmp(args[i], "-funcs")==0)	funcp=1;
			else if(strcmp(args[i], "-vars")==0)	varsp=1;
			else if(strcmp(args[i], "-all")==0)		{allp=1; break;}
			else if(strcmp(args[i], "-pmap")==0)	pmapp=1;
		}
		++i;
	}

	// Activar todas las flags
	if(allp){ blockp=1; funcp=1; varsp=1; pmapp=1; }

	if(varsp){
		printf("Variables locales\t%p, \t%p, \t%p\n", &int_l, &char_l, &float_l);
		printf("Variables globales\t%p, \t%p, \t%p\n", &int_g, &cmd_table, &cmd_help);
		printf("Variables estaticas\t%p, \t%p, \t%p\n", &int_s, &char_s, &float_s);
	}
	if(funcp){
		printf("Funciones programa\t%p, \t%p, \t%p\n", &cmdAllocate, &cmdMemdump, &cmdRecurse);
		printf("Funciones librería\t%p, \t%p, \t%p\n", &sscanf, &free, &printf);
	}
	if(blockp)
		print_memList(NOT_DEFINED, memoryList);
	if(pmapp)
		Do_pmap();

	return SSUCC_EXIT;
}
int cmdRecurse(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList){
	if(lenArg!=1) Recursiva(atoi(args[1]));
	return SSUCC_EXIT;
}