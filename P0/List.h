#ifndef __IMPLEMENTACION_LISTA_MULTITIPO_H__
#define __IMPLEMENTACION_LISTA_MULTITIPO_H__

// Definicion de tipos7
#define MAX_POS 4096
#define LNULL -1

// Definicion de tipos
typedef void *Litem;
typedef int Lpos;
struct list {
	Litem data[MAX_POS];
	Lpos size;
};
typedef struct list *List;

// Métodos de la implementacion
void createList(List l);
int isEmptyList(List l);

Lpos firstElement(List l);
Lpos lastElement(List l);
Lpos nextElement(List l, Lpos pos);
Lpos prevElement(List l, Lpos pos);

int insertElement(List l, Lpos pos, void *element);

void *getElement(List l, Lpos pos);
int getSize(List l);
void updateElement(List l, Lpos pos, void *element);

void clearList(List l, void (*user_mem)(void *data));
void deleteList(List l, void (*user_mem)(void *data));

#endif //__IMPLEMENTACION_LISTA_MULTITIPO_H__
