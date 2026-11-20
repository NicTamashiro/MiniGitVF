#ifndef FILA_H_
#define FILA_H_
#include <stdbool.h>

#define MAX_QUEUE 5  

typedef struct {
    int  ordem;           
    char nome[30];         
} Arquivo;

typedef Arquivo queue_info;

typedef struct {
    queue_info dados[MAX_QUEUE];
    int comeco;            
    int fim;               
    int qtd_elementos;     
} fila;

void queue_init(fila *f);
bool enqueue(fila *f, queue_info val);
bool dequeue(fila *f, queue_info *ret);
bool front(fila f, queue_info *ret);
bool queue_isempty(fila f);
bool queue_isfull(fila f);

#endif
