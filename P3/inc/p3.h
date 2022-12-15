#ifndef _P3_H_
#define _P3_H_

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

// ===== Métodos semi-privados de la práctica =====
int print_environ_values(char *env[], char *env_type);			// Muestra las variables del entorno pasado como parametro
int update_data(t_proc *item);									// Actualiza el estado de las entradas de la tabla de procesos
void print_proc_dataItem(t_proc *item);							// Muestra incformación de una entrada de ls lista de procesos
void imprimir_lista_procesos(List processList, pid_t pid);		// Muestra por pantalla la lista de procesos
void del_process_by_status(List processList, t_pstat status);	// Elimina una entrada de la lista de procesos según su estado

// ===== Programas propios de la shell =====
int cmdPriority(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdShowvar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdChangevar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdShowenv(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdFork(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdExecute(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdListjobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdDeljobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdJob(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);

#endif //_P3_H_