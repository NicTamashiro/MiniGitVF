#ifndef REPOSITORIO_H_
#define REPOSITORIO_H_
#include "fila.h"
#include "pilha.h"
#include "lista.h"

/* O estado do mini-git: cada estrutura cuida de uma parte. */
typedef struct {
    fila  staging;          /* add -> commit            (FIFO)  */
    pilha stash;            /* stash -> stash pop       (LIFO)  */
    lista historico;        /* commits em ordem, do mais antigo */
    int   proximo_add;
    int   proximo_stash;
    int   proximo_commit;
} Repositorio;

void repo_init(Repositorio *r);

#endif
