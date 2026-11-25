#ifndef PILHA_H_
#define PILHA_H_
#include <stdbool.h>
#include "tipos.h"

/* Pilha sequencial em vetor (LIFO). Usada no stash. */

#define STACK_MAX 5

typedef Stash stack_info;

typedef struct {
    stack_info itens[STACK_MAX];
    int topo;              /* quantos itens existem; o topo e itens[topo-1] */
} pilha;

void stack_init(pilha *p);
bool push(pilha *p, stack_info x);
bool pop(pilha *p, stack_info *ret);
bool top(const pilha *p, stack_info *ret);
bool stack_isempty(const pilha *p);
bool stack_isfull(const pilha *p);
int  stack_len(const pilha *p);
/* i-esimo item a partir do topo (0 = topo, como stash@{0}), sem remover */
bool stack_get(const pilha *p, int i, stack_info *ret);

#endif
