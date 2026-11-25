#include <stdio.h>
#include <string.h>
#include "historico.h"
#include "entrada.h"

static void mostrar_commit(Commit c){
    printf("  commit %d - %s (autor: %s)\n", c.id, c.mensagem, c.autor);
    for(int i = 0; i < c.qtd_arquivos; i++)
        printf("      %s (add %d)\n", c.arquivos[i].nome, c.arquivos[i].ordem);
}

/* Le um id e devolve a posicao dele na lista, ou -1 (ja avisando o erro). */
static int ler_posicao_commit(lista historico){
    int id, pos;
    printf("Id do commit: ");
    if(!ler_numero(&id)){
        printf("Erro: digite um numero.\n");
        return -1;
    }
    pos = lst_localizar(historico, id);
    if(pos == -1)
        printf("Erro: commit %d nao existe.\n", id);
    return pos;
}

void historico_commit(Repositorio *r){
    Commit c;

    if(queue_isempty(r->staging)){
        printf("Erro: staging vazio, nada para commitar. Use o add antes.\n");
        return;
    }
    if(lst_isfull(r->historico)){
        printf("Erro: historico cheio (maximo de %d commits).\n", LST_MAX);
        return;
    }

    printf("Mensagem do commit: ");
    ler_texto(c.mensagem, sizeof(c.mensagem));
    if(strlen(c.mensagem) == 0){
        printf("Erro: a mensagem nao pode ficar vazia.\n");
        return;
    }
    printf("Autor: ");
    ler_texto(c.autor, sizeof(c.autor));
    if(strlen(c.autor) == 0){
        printf("Erro: o autor nao pode ficar vazio.\n");
        return;
    }

    /* fila -> commit: os arquivos saem na ordem em que entraram (FIFO) */
    c.qtd_arquivos = 0;
    while(dequeue(&r->staging, &c.arquivos[c.qtd_arquivos]))
        c.qtd_arquivos++;

    c.id = r->proximo_commit;
    lst_inserir(&r->historico, c, lst_len(r->historico));    /* entra no fim da lista */
    r->proximo_commit++;

    printf("Commit %d criado com %d arquivo(s):\n", c.id, c.qtd_arquivos);
    mostrar_commit(c);
    printf("Staging vazio.\n");
}

void historico_log(Repositorio *r){
    Commit c;
    if(lst_len(r->historico) == 0){
        printf("Nenhum commit ainda.\n");
        return;
    }
    printf("Historico (%d de %d commits), do mais antigo ao mais novo:\n",
           lst_len(r->historico), LST_MAX);
    for(int k = 0; lst_get(r->historico, k, &c); k++)
        mostrar_commit(c);
}

void historico_buscar(Repositorio *r){
    Commit c;
    int pos;
    if(lst_len(r->historico) == 0){
        printf("Erro: nao ha commits para buscar.\n");
        return;
    }
    pos = ler_posicao_commit(r->historico);
    if(pos != -1 && lst_get(r->historico, pos, &c))
        mostrar_commit(c);
}

void historico_remover(Repositorio *r){
    Commit c;
    int pos;
    if(lst_len(r->historico) == 0){
        printf("Erro: nao ha commits para remover.\n");
        return;
    }
    pos = ler_posicao_commit(r->historico);
    if(pos != -1 && lst_remover(&r->historico, pos, &c))
        printf("Commit %d removido: %s\n", c.id, c.mensagem);
}
