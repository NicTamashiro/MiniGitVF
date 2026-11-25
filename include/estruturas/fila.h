#ifndef FILA_H_
#define FILA_H_
#include <stdbool.h>
#include "tipos.h"

/* Fila circular em vetor (FIFO). Usada no staging. */

#define MAX_QUEUE MAX_STAGING

typedef Arquivo queue_info;

typedef struct {
    queue_info dados[MAX_QUEUE];
    int comeco;            /* posicao de quem sai primeiro            */
    int fim;               /* primeira posicao livre, atras do ultimo */
    int qtd_elementos;     /* diferencia fila vazia de fila cheia     */
} fila;

void queue_init(fila *f);
bool enqueue(fila *f, queue_info val);
bool dequeue(fila *f, queue_info *ret);
bool front(fila f, queue_info *ret);
bool queue_isempty(fila f);
bool queue_isfull(fila f);
int  queue_len(fila f);
/* i-esimo item a partir da frente (0 = frente), sem remover */
bool queue_get(fila f, int i, queue_info *ret);

#endif
