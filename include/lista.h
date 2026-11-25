#ifndef LISTA_H_
#define LISTA_H_
#include <stdbool.h>
#include "tipos.h"

/* Lista sequencial em vetor. Usada no historico de commits. */

#define LST_MAX 10

typedef Commit lst_info;

typedef struct {
    lst_info itens[LST_MAX];
    int tamanho;
} lista;

void lst_init(lista *l);
int  lst_len(lista l);
bool lst_isfull(lista l);
bool lst_get(lista l, int k, lst_info *ret);
int  lst_localizar(lista l, int id);          /* posicao do id, ou -1 */
bool lst_inserir(lista *l, lst_info val, int k);
bool lst_remover(lista *l, int k, lst_info *val);
bool lst_concatenar(lista l1, lista l2, lista *l3);

#endif
