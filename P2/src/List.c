// Módulos del sistema
#include <stdio.h>
#include <stdlib.h>

// Módulos de la implementacion
#include "List.h"

// Definicion de tipos
struct dNode{
	void *dato;
	struct dNode *next;
};

// Definicion de la lista
struct lista{
	int size;
	struct dNode *last;
	struct dNode *head;
};

// ==== IMPLEMENTACIÓN DE LOS MÉTODOS ====

// Crea una lista vacía
// En caso de que no se pueda reservar memoria, se devuelve NULL
void createList(List *l){
	if((*l)!=NULL) return;
	
	*l = (struct lista *)malloc(sizeof(struct lista));
	if(*l==NULL) return;

	(*l)->size=0;
	(*l)->head=NULL;
	(*l)->last=NULL;
}

// Retorna 1 en caso de que la lista esté vacía
// 0 en caso contrario
int isEmptyList(List l){
	return (l->size==0 || l->head==NULL || l->last==NULL)? 1 : 0;
}

// Devuelve el primer elemento de la lista
// NULL en caso de que la lista esté vacía
Lpos firstElement(List l){
	return l->head;
}

// Devuelve el último elemento de la lista
// NULL en caso de que la lista esté vacía
Lpos lastElement(List l){
	return l->last;
}

// Devuelve la siguiente posición si es válida
// NULL si es la última poisicion
Lpos nextElement(List l, Lpos pos){
	return (pos->next==NULL)? NULL : pos->next;
}

// Devuelve el elemento anterior
// NULL en caso de que la posicion pasada sea la primera
Lpos prevElement(List l, Lpos pos){
	if(l->head==pos) return NULL;
	struct dNode *auxPos=l->head;
	while(auxPos->next!=pos)
		auxPos=auxPos->next;
	return auxPos;
}

// Devuelve 1 en caso de que se haya podido insertar un elemento
// al final de la lista
// 0 en caso contrario
int insertElement(List l, void *element){
	struct dNode *nwNode;
	if((nwNode=(struct dNode *)malloc(sizeof(struct dNode)))==NULL) return 0;

	nwNode->dato=element;
	nwNode->next=NULL;

	if(l->head==NULL)
		l->head=nwNode;

	if(l->last!=NULL)
		l->last->next=nwNode;
	l->last=nwNode;

	l->size++;
	return 1;
}

// Devuelve 1 en caso de que se haya podido insertar un elemento
// en la posicion indicada
// 0 en caso contrario
int insertElementIn(List l, Lpos p, void *element){
	struct dNode *nwNode;
	if((nwNode=(struct dNode *)malloc(sizeof(struct dNode)))==NULL) return 0;

	nwNode->dato=element;
	nwNode->next=NULL;

	// Inicio de la lista o primera posicion de la lista
	if(p==l->head || p==NULL){
		nwNode->next = l->head;
		l->head = nwNode;

	// Ultima posicion de la lista
	}else if(p->next==NULL){
		p->next = nwNode;

	// Si es un elemento en el medio de la lista
	}else{
		nwNode->next = p->next;
		nwNode->dato = p->dato;
		p->dato = element;
		p->next = nwNode;
	}

	l->size++;
	return 1;
}

// Devuelve el puntero que apunta a la direccion de memoria del
// dato guardado en la posicion
void *getElement(List l, Lpos pos){
	return pos->dato;
}

// Devuelve la longitud de la lista
// En caso de estar vacía devuelve 0
int getSize(List l){
	return l->size;
}

// Actualiza el dato en la posicion indicada de la lista
void updateElement(List l, Lpos pos, void *element){
	pos->dato=element;
}

// Se limpia los datos de la lista
// La lista queda inicializada
void clearList(List l, void (*user_mem)(void *data)){
	if(isEmptyList(l)==1) return;
	
	struct dNode *auxPos;

	while(l->head!=NULL){
		auxPos = l->head;
		l->head = auxPos->next;
		auxPos->next = NULL;

		// Liveracion de la memoria de los datos introducidos por el usuario
		if(user_mem!=NULL)
			user_mem(auxPos->dato);

		free(auxPos);
	}
	
	l->size=0;
	l->last=NULL;
}

// Elimina la lista pasada por parámetro
// Esta no será inicializada, L->NULL
void deleteList(List l, void (*user_mem)(void *data)){
	clearList(l, user_mem);
	free(l);
}
