#include <stdio.h>
#include <string.h>
#include "staging.h"
#include "entrada.h"

bool staging_contem(fila f, const char *nome){
    Arquivo a;
    for(int i = 0; queue_get(f, i, &a); i++){
        if(strcmp(a.nome, nome) == 0)
            return true;
    }
    return false;
}

void staging_adicionar(Repositorio *r){
    Arquivo a;

    if(queue_isfull(r->staging)){
        printf("Erro: staging cheio (maximo de %d). Faca um commit ou um stash antes.\n", MAX_QUEUE);
        return;
    }

    printf("Nome do arquivo: ");
    ler_texto(a.nome, sizeof(a.nome));
    if(strlen(a.nome) == 0 || strchr(a.nome, ' ') != NULL){
        printf("Erro: nome invalido (vazio ou com espaco).\n");
        return;
    }
    if(staging_contem(r->staging, a.nome)){
        printf("Erro: %s ja esta no staging.\n", a.nome);
        return;
    }

    a.ordem = r->proximo_add;
    if(enqueue(&r->staging, a)){
        printf("%s entrou no fim do staging.\n", a.nome);
        r->proximo_add++;
    }
}

void staging_ver_proximo(Repositorio *r){
    Arquivo a;
    if(!front(r->staging, &a)){
        printf("Erro: staging vazio, nao ha proximo arquivo.\n");
        return;
    }
    printf("Proximo a sair: %s (add %d)\n", a.nome, a.ordem);
}

void staging_status(Repositorio *r){
    Arquivo a;
    if(queue_isempty(r->staging)){
        printf("Staging vazio.\n");
        return;
    }
    printf("Staging (%d de %d), da frente para o fim:\n", queue_len(r->staging), MAX_QUEUE);
    for(int i = 0; queue_get(r->staging, i, &a); i++)
        printf("  %d. %s (add %d)\n", i + 1, a.nome, a.ordem);
}

void staging_desfazer_primeiro(Repositorio *r){
    Arquivo a;
    if(!dequeue(&r->staging, &a)){
        printf("Erro: staging vazio, nada para tirar.\n");
        return;
    }
    printf("%s saiu do staging (nao vai para o proximo commit).\n", a.nome);
}
