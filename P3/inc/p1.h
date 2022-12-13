#ifndef _P1_H_
#define _P1_H_

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

// ===== Métodos semi-privados de la práctica =====
int borrarDir(char *dir);						// Borra el directorio si es válido
void print_file_info(const char *name, const char *allPath, const struct stat *std, short longp, short accp, short linkp);					// Muestra por pantalla información del archivo
void print_dir_data(const char *name, short hidp, short longp, short accp, short linkp);													// Muestra por pantalla el contenido del directorio
void process_dir_data(const char *name, short recap, short recbp, short hidp, short longp, short accp, short linkp);						// Pocesa los datos de un directorio
void list_fd_data(const char *name, const struct stat *std, short recap, short recbp, short hidp, short longp, short accp, short linkp);	// Comprueba de qiue forma procesar y mostrar los datos de un directorio u archivo

// ===== Programas propios de la shell =====
int cmdCreate(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdStat(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdList(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdDelete(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdDeltree(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);

#endif //_P1_H_