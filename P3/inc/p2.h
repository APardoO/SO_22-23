#ifndef _P2_H_
#define _P2_H_

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

// ===== Métodos semi-privados de la práctica =====
void do_AllocateMmap(char *fich, char *permisos, List memoryList);		// Codigo de ejemplo p2
void print_data_item(t_mem *item);										// Muestra los datos de un item de la lista de memoria
void print_memList(t_asign asign, List memoryList);						// Muestra la informacion de cada item de la lista de memoria
int malloc_allocation(size_t size, List memoryList);					// Reserva un bloque de memoria malloc del tamaño introducido
int allocate_nshared_block(key_t key, size_t size, List memoryList);	// Reserva un bloque de memoria compartida con la llave y el tamaño introducidos
int alloc_shared_block(key_t key, List memoryList);						// Asocia un boque de memoria compartida con una llave introducida
int malloc_deallocation(size_t size, List memoryList);					// Livera un bloque de memoria malloc del tamaño introducido
int del_shared_block(key_t key, List memoryList);						// Livera un bloque de memoria compartida
int unmapped_memory_block(char *file_name, List memoryList);			// Desmapea el contenido de un fichero de la memoria
int memory_direction_block(char *dir, List memoryList);					// Comprueba si existe un bloque de memoria reservado pasando un string como direccion de memoria
int desc_special_charecter(unsigned int ui);							// Retorna el desplazamiento que se debe aplicar a los caracteres de escape en la tabla ascii
void print_character(char c, const short char_format);					// Muestra un caracter en formato "%3c", independientemente de que sea un caracter de escape o no
void print_memory_block(char *pos, int bytes, List memoryList);			// Mapea los 'bytes' que haya en una direccion de memoria
t_mem *search_mem_pos(List memoryList, void *memdir);					// Comprueba si en la lista, hay un elemento que tenga la direccion pasada como argumento. Si la hay, la retorna

// ===== Programas propios de la shell =====
/*[+]*/int cmdAllocate(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
/*[+]*/int cmdDeallocate(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
/*[!]*/int cmdIo(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdMemdump(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
/*[+]*/int cmdMemfill(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdMemory(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdRecurse(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);

#endif //_P2_H_