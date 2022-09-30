#ifndef __IMPLEMENTACION_LISTA_MULTITIPO_H__
#define __IMPLEMENTACION_LISTA_MULTITIPO_H__

// Definicion de tipos
typedef struct dNode *Lpos;
typedef struct lista *List;

// Métodos de la implementacion
void createList(List *l);
int isEmptyList(List l);

Lpos firstElement(List l);
Lpos lastElement(List l);
Lpos nextElement(List l, Lpos pos);
Lpos prevElement(List l, Lpos pos);

int insertElement(List l, void *element);
int insertElementIn(List l, Lpos p, void *element);

void *getElement(List l, Lpos pos);
int getSize(List l);
void updateElement(List l, Lpos pos, void *element);

void clearList(List l, void (*user_mem)(void *data));
void deleteList(List l, void (*user_mem)(void *data));

#endif //__IMPLEMENTACION_LISTA_MULTITIPO_H__
