// Módulos del sistema
#include <stdio.h>
#include <stdlib.h>

// Módulos de la implementacion
#include "List.h"

// ==== IMPLEMENTACIÓN DE LOS MÉTODOS ====
// Crea una lista vacía
// En caso de que no se pueda reservar memoria, se devuelve NULL
void createList(List l){
	l->size=0;
}

// Retorna 1 en caso de que la lista esté vacía
// 0 en caso contrario
int isEmptyList(List l){
	return (l->size==0)? 1 : 0;
}

// Devuelve el primer elemento de la lista
// NULL en caso de que la lista esté vacía
Lpos firstElement(List l){
	return (l->size==0)? LNULL : 0;
}

// Devuelve el último elemento de la lista
// NULL en caso de que la lista esté vacía
Lpos lastElement(List l){
	return (l->size==MAX_POS)? LNULL : l->size-1;
}

// Devuelve la siguiente posición si es válida
// NULL si es la última poisicion
Lpos nextElement(List l, Lpos pos){
	return (pos>=l->size-1)? LNULL : ++pos;
}

// Devuelve el elemento anterior
// NULL en caso de que la posicion pasada sea la primera
Lpos prevElement(List l, Lpos pos){
	return (pos<=0 || pos>l->size)? LNULL : --pos;
}

// Devuelve 1 en caso de que se haya podido insertar un elemento
// al final de la lista
// 0 en caso contrario
int insertElement(List l, Lpos pos, void *element){
	// Comprobar que la lista no esté llena
	if(l->size == MAX_POS)
		return 0;
	
	// Si la lista está vacía
	if(l->size==0){
		l->data[0] = element;
		l->size = 1;
		return 1;
	}

	// Se inserta al final de la lista
	if(pos==LNULL)
		l->data[l->size] = element;

	// Se inserta en una posición concreta
	else{
		if(pos>l->size || pos<0) return 0;
		for(int auxPos=l->size; auxPos>pos;--auxPos)
			l->data[auxPos] = l->data[auxPos-1];
		l->data[pos] = element;
	}

	l->size += 1;
	return 1;
}

// Devuelve el puntero que apunta a la direccion de memoria del
// dato guardado en la posicion
void *getElement(List l, Lpos pos){
	return (pos<0 || pos>=l->size)? NULL : l->data[pos];
}

// Devuelve la longitud de la lista
// En caso de estar vacía devuelve 0
int getSize(List l){
	return l->size;
}

// Actualiza el dato en la posicion indicada de la lista
void updateElement(List l, Lpos pos, void *element){
	l->data[pos] = element;
}

// Se limpia los datos de la lista
// La lista queda inicializada
void clearList(List l, void (*user_mem)(void *data)){
	for(int i=l->size-1; i>=0; --i){
		if(user_mem!=NULL)
			user_mem(l->data[i]);
		else
			free(l->data[i]);
	}

	l->size=0;
}

// Elimina la lista pasada por parámetro
// Esta no será inicializada, L->NULL
void deleteList(List l, void (*user_mem)(void *data)){
	clearList(l, user_mem);
	free(l);
}
