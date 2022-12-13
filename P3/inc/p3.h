#ifndef _P3_H_
#define _P3_H_

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

// ===== Métodos semi-privados de la práctica =====
int print_environ_values(char *env[], char *env_type);		// Muestra las variables del entorno pasado como parametro

// ===== Programas propios de la shell =====
/*[+]*/int cmdPriority(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[+]*/int cmdShowvar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[+]*/int cmdChangevar(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
int cmdShowenv(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[+]*/int cmdFork(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[+]*/int cmdExecute(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[!]*/int cmdListjobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[!]*/int cmdDeljobs(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);
/*[!]*/int cmdJob(const int lenArg, char *args[PHARAM_LEN], char *envp[], List historicList, List memoryList, List processList);

#endif //_P3_H_