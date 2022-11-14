#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"

// Test functions
int test_strings_noInput();
int test_pointerStruct_noInput();
int test_pointerInt_noInput();
int numFunctions();

struct types{
	int (*functionalilty)();
	char *name;
};
struct types functionaliltyes_table[] = {
	{test_strings_noInput, "test_strings_noInput..."},
	{test_pointerStruct_noInput, "test_pointerStruct_noInput..."},
	{test_pointerInt_noInput, "test_pointerInt_noInput..."},
	{NULL, NULL}
};


int main(void){
	int functions=numFunctions();
	int *execution_functionality = (int *)malloc(functions * sizeof(int));

	for(int i=0; i<functions; ++i)
		execution_functionality[i] = functionaliltyes_table[i].functionalilty();
	
	printf("\n== FUNCTIONELITIES STATUS CODE==\n");
	for(int i=0; i<functions; ++i)
		printf("%-30s [%d] (%s)\n", functionaliltyes_table[i].name, execution_functionality[i], (execution_functionality[i]==0)? "done" : "failure");

	free(execution_functionality);
	return 0;
}

int numFunctions(){
	int i=0;
	while(functionaliltyes_table[i].functionalilty!=NULL)
		++i;
	return i;
}
int test_strings_noInput(){
	List lista;
	createList(&lista);

	if(lista==NULL){
		printf("[!] Error\n");
		return 1;
	}

	insertElement(lista, "Hola");
	insertElement(lista, "que");
	insertElement(lista, "tal");
	insertElement(lista, "?");

	if(isEmptyList(lista)==1){
		printf("Está vacia\n");
		deleteList(lista, NULL);
		return 1;
	}else{
		for(Lpos auxPos=firstElement(lista); auxPos!=NULL; auxPos=nextElement(lista, auxPos)){
			printf("[%s] ", (char *)getElement(lista, auxPos));
		}
		printf("\n");
	}

	deleteList(lista, NULL);
	return 0;
}

struct estructurado{
	int entero;
	char cadena[50];
};
void user_mem_struct(void *data){
	free((struct estructurado *)data);
}
int test_pointerStruct_noInput(){
	List lista;
	createList(&lista);

	if(lista==NULL){
		printf("[!] Error\n");
		return 1;
	}

	struct estructurado *a1, *a2, *a3, *a4;
	if((a1=(struct estructurado *)malloc(sizeof(struct estructurado)))==NULL) return 1;
	if((a2=(struct estructurado *)malloc(sizeof(struct estructurado)))==NULL){
		free(a1);
		return 1;
	}
	if((a3=(struct estructurado *)malloc(sizeof(struct estructurado)))==NULL){
		free(a1);
		free(a2);
		return 1;
	}
	if((a4=(struct estructurado *)malloc(sizeof(struct estructurado)))==NULL){
		free(a1);
		free(a2);
		free(a3);
		return 1;
	}


	a1->entero=1;
	strcpy(a1->cadena, "a1");

	a2->entero=2;
	strcpy(a2->cadena, "a2");
	
	a3->entero=3;
	strcpy(a3->cadena, "a3");
	
	a4->entero=4;
	strcpy(a4->cadena, "a4");
	

	insertElement(lista, a1);
	insertElement(lista, a2);
	insertElement(lista, a3);
	insertElement(lista, a4);


	if(isEmptyList(lista)==1){
		printf("Está vacia\n");
		deleteList(lista, NULL);
		return 1;
	}else{
		struct estructurado *prueba;
		for(Lpos auxPos=firstElement(lista); auxPos!=NULL; auxPos=nextElement(lista, auxPos)){
			prueba = (struct estructurado *)getElement(lista, auxPos);
			printf("[%d]->%s ", prueba->entero, prueba->cadena);
		}
		printf("\n");
	}

	deleteList(lista, user_mem_struct);
	return 0;
}

int test_pointerInt_noInput(){
	List lista;
	createList(&lista);

	if(lista==NULL){
		printf("[!] Error\n");
		return 1;
	}

	int *a1, *a2, *a3, *a4, b1=1, b2=2, b3=3, b4=4;
	a1=&b1;
	a2=&b2;
	a3=&b3;
	a4=&b4;

	insertElement(lista, a1);
	insertElement(lista, a2);
	insertElement(lista, a3);
	insertElement(lista, a4);

	if(isEmptyList(lista)==1){
		printf("Está vacia\n");
		deleteList(lista, NULL);
		return 1;
	}else{
		int *prueba;
		for(Lpos auxPos=firstElement(lista); auxPos!=NULL; auxPos=nextElement(lista, auxPos)){
			prueba = (int *)getElement(lista, auxPos);
			printf("[%d] ", *prueba);
		}
		printf("\n");
	}

	deleteList(lista, NULL);
	return 0;
}