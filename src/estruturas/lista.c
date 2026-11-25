#include <stddef.h>
#include "lista.h"

void lst_init(lista *l){
    l->tamanho = 0;
}

int lst_len(lista l){
    return l.tamanho;
}

bool lst_isfull(lista l){
    return l.tamanho == LST_MAX;
}

bool lst_get(lista l, int k, lst_info *ret){
    if(k < 0 || k >= l.tamanho || ret == NULL)
        return false;
    *ret = l.itens[k];
    return true;
}

int lst_localizar(lista l, int id){
    for(int i = 0; i < l.tamanho; i++){
        if(l.itens[i].id == id)
            return i;
    }
    return -1;
}

bool lst_inserir(lista *l, lst_info val, int k){
    if(l->tamanho == LST_MAX || k < 0 || k > l->tamanho)
        return false;
    for(int i = l->tamanho - 1; i >= k; i--)     /* abre espaco na posicao k */
        l->itens[i + 1] = l->itens[i];
    l->itens[k] = val;
    l->tamanho++;
    return true;
}

bool lst_remover(lista *l, int k, lst_info *val){
    if(l->tamanho == 0 || k < 0 || k >= l->tamanho || val == NULL)
        return false;
    *val = l->itens[k];
    for(int i = k; i < l->tamanho - 1; i++)      /* fecha o buraco */
        l->itens[i] = l->itens[i + 1];
    l->tamanho--;
    return true;
}

bool lst_concatenar(lista l1, lista l2, lista *l3){
    if(l1.tamanho + l2.tamanho > LST_MAX || l3 == NULL)
        return false;
    l3->tamanho = 0;
    for(int i = 0; i < l1.tamanho; i++)
        l3->itens[l3->tamanho++] = l1.itens[i];
    for(int i = 0; i < l2.tamanho; i++)
        l3->itens[l3->tamanho++] = l2.itens[i];
    return true;
}
