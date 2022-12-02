#ifndef _P0_H_
#define _P0_H_

#include "Sys_module.h"		// Ĺibrería de la shell con métodos específicos de cada práctica

// ===== Métodos semi-privados de la práctica =====
int checkZeroPharam(char arg[PHARAM_LEN]);						// Comprueba el parametro introducido '-N'
void printNcommands(int n, List historicList);					// Muestra por pantalla los n primeros comandos del historicos

// ===== Programas propios de la shell =====
int cmdAutores(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdPid(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdCarpeta(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdFecha(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdHist(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdComando(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdInfosys(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdHelp(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);
int cmdExit(const int lenArg, char *args[PHARAM_LEN], List historicList, List memoryList, List processList);

#endif //_P0_H_