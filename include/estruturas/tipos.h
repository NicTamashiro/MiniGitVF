#ifndef TIPOS_H_
#define TIPOS_H_

/* Tipos do dominio do mini-git. As estruturas de dados (fila, pilha, lista)
 * so conhecem estes tipos pelo typedef de cada uma (queue_info, stack_info,
 * lst_info); nenhuma delas sabe o que e um arquivo, um stash ou um commit. */

#define MAX_STAGING  5    /* arquivos no staging e em cada commit/stash */
#define TAM_NOME     30
#define TAM_TEXTO    50

typedef struct {
    int  ordem;                 /* numero do add: 1, 2, 3...  */
    char nome[TAM_NOME];        /* ex.: "login.c"             */
} Arquivo;

typedef struct {
    int     id;                         /* numero do stash: 1, 2, 3... */
    char    descricao[TAM_TEXTO];       /* ex.: "login pela metade"    */
    Arquivo arquivos[MAX_STAGING];      /* o staging que foi guardado  */
    int     qtd_arquivos;
} Stash;

typedef struct {
    int     id;                         /* numero do commit: 1, 2, 3... */
    char    mensagem[TAM_TEXTO];
    char    autor[TAM_NOME];
    Arquivo arquivos[MAX_STAGING];      /* o que saiu do staging        */
    int     qtd_arquivos;
} Commit;

#endif
