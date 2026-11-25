#include <stddef.h>
#include "pilha.h"

void stack_init(pilha *p){
    p->topo = 0;
}

bool push(pilha *p, stack_info x){
    if(p->topo == STACK_MAX)
        return false;
    p->itens[p->topo++] = x;        /* grava e depois soma 1 */
    return true;
}

bool pop(pilha *p, stack_info *ret){
    if(p->topo == 0 || ret == NULL)
        return false;
    *ret = p->itens[--p->topo];     /* subtrai 1 e depois le */
    return true;
}

bool top(pilha *p, stack_info *ret){
    if(p->topo == 0 || ret == NULL)
        return false;
    *ret = p->itens[p->topo - 1];
    return true;
}

bool stack_isempty(pilha *p){
    return p->topo == 0;
}

bool stack_isfull(pilha *p){
    return p->topo == STACK_MAX;
}

int stack_len(pilha *p){
    return p->topo;
}

bool stack_get(pilha *p, int i, stack_info *ret){
    if(i < 0 || i >= p->topo || ret == NULL)
        return false;
    *ret = p->itens[p->topo - 1 - i];
    return true;
}
