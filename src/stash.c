#include <stdio.h>
#include <string.h>
#include "stash.h"
#include "staging.h"
#include "entrada.h"

static void mostrar_stash(int posicao, Stash s){
    printf("  stash@{%d}: stash %d - \"%s\" (arquivos:", posicao, s.id, s.descricao);
    for(int i = 0; i < s.qtd_arquivos; i++)
        printf(" %s", s.arquivos[i].nome);
    printf(")\n");
}

void stash_guardar(Repositorio *r){
    Stash s;

    if(queue_isempty(r->staging)){
        printf("Erro: staging vazio, nao ha trabalho para guardar.\n");
        return;
    }
    if(stack_isfull(&r->stash)){
        printf("Erro: stash cheio (maximo de %d). Recupere ou descarte um antes.\n", STACK_MAX);
        return;
    }

    printf("Descricao do trabalho: ");
    ler_texto(s.descricao, sizeof(s.descricao));
    if(strlen(s.descricao) == 0){
        printf("Erro: a descricao nao pode ficar vazia.\n");
        return;
    }

    /* fila -> pilha: o staging inteiro vai para o topo do stash */
    s.qtd_arquivos = 0;
    while(dequeue(&r->staging, &s.arquivos[s.qtd_arquivos]))
        s.qtd_arquivos++;

    s.id = r->proximo_stash;
    push(&r->stash, s);
    r->proximo_stash++;
    printf("Stash %d guardado no topo com %d arquivo(s). Staging vazio.\n", s.id, s.qtd_arquivos);
}

void stash_recuperar(Repositorio *r){
    Stash s;
    int novos = 0;

    if(!top(&r->stash, &s)){
        printf("Erro: stash vazio, nada para recuperar.\n");
        return;
    }

    /* Confere antes de tirar da pilha: se nao couber, o stash continua guardado. */
    for(int i = 0; i < s.qtd_arquivos; i++)
        if(!staging_contem(r->staging, s.arquivos[i].nome))
            novos++;
    if(queue_len(r->staging) + novos > MAX_QUEUE){
        printf("Erro: o staging nao tem espaco para os %d arquivo(s) do stash %d.\n", novos, s.id);
        return;
    }

    /* pilha -> fila: os arquivos voltam para o fim do staging */
    pop(&r->stash, &s);
    printf("Stash %d recuperado: \"%s\"\n", s.id, s.descricao);
    for(int i = 0; i < s.qtd_arquivos; i++){
        if(staging_contem(r->staging, s.arquivos[i].nome)){
            printf("  %s ja estava no staging.\n", s.arquivos[i].nome);
        } else {
            enqueue(&r->staging, s.arquivos[i]);
            printf("  %s voltou para o staging.\n", s.arquivos[i].nome);
        }
    }
}

void stash_ver_topo(Repositorio *r){
    Stash s;
    if(!top(&r->stash, &s)){
        printf("Erro: stash vazio, nao ha topo.\n");
        return;
    }
    printf("Topo:\n");
    mostrar_stash(0, s);
}

void stash_listar(Repositorio *r){
    Stash s;
    if(stack_isempty(&r->stash)){
        printf("Nenhum stash guardado.\n");
        return;
    }
    printf("Stashes (%d de %d), do topo para a base:\n", stack_len(&r->stash), STACK_MAX);
    for(int i = 0; stack_get(&r->stash, i, &s); i++)
        mostrar_stash(i, s);
}

void stash_descartar(Repositorio *r){
    Stash s;
    if(!pop(&r->stash, &s)){
        printf("Erro: stash vazio, nada para descartar.\n");
        return;
    }
    printf("Stash %d descartado: \"%s\"\n", s.id, s.descricao);
}
