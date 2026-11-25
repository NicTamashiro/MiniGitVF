#include <stddef.h>
#include "fila.h"

void queue_init(fila *f){
    f->qtd_elementos = f->comeco = f->fim = 0;
}

bool enqueue(fila *f, queue_info val){
    if(f->qtd_elementos == MAX_QUEUE)
        return false;
    f->dados[f->fim] = val;
    f->fim = (f->fim + 1) % MAX_QUEUE;      /* depois da ultima posicao, volta para a 0 */
    f->qtd_elementos++;
    return true;
}

bool dequeue(fila *f, queue_info *ret){
    if(f->qtd_elementos == 0 || ret == NULL)
        return false;
    *ret = f->dados[f->comeco];
    f->comeco = (f->comeco + 1) % MAX_QUEUE;
    f->qtd_elementos--;
    return true;
}

bool front(fila f, queue_info *ret){
    if(f.qtd_elementos == 0 || ret == NULL)
        return false;
    *ret = f.dados[f.comeco];
    return true;
}

bool queue_isempty(fila f){
    return f.qtd_elementos == 0;
}

bool queue_isfull(fila f){
    return f.qtd_elementos == MAX_QUEUE;
}

int queue_len(fila f){
    return f.qtd_elementos;
}

bool queue_get(fila f, int i, queue_info *ret){
    if(i < 0 || i >= f.qtd_elementos || ret == NULL)
        return false;
    *ret = f.dados[(f.comeco + i) % MAX_QUEUE];
    return true;
}
