#include <stdio.h>
#include "comandos.h"
#include "minigit.h"
#include "entrada.h"

/* Mostra o erro do core e diz se a operacao falhou. */
static bool falhou(mg_status s){
    if(s == MG_OK) return false;
    printf("Erro: %s.\n", mg_mensagem(s));
    return true;
}

static void mostrar_arquivos(const Arquivo *arquivos, int qtd){
    for(int i = 0; i < qtd; i++)
        printf("      %s (add %d)\n", arquivos[i].nome, arquivos[i].ordem);
}

static void mostrar_commit(Commit c){
    printf("  commit %d - %s (autor: %s)\n", c.id, c.mensagem, c.autor);
    mostrar_arquivos(c.arquivos, c.qtd_arquivos);
}

static void mostrar_stash(int posicao, Stash s){
    printf("  stash@{%d}: stash %d - \"%s\" (arquivos:", posicao, s.id, s.descricao);
    for(int i = 0; i < s.qtd_arquivos; i++)
        printf(" %s", s.arquivos[i].nome);
    printf(")\n");
}

/* ---------- staging (fila) ---------- */

void cli_add(Repositorio *r){
    char nome[TAM_NOME];
    if(falhou(mg_pode_add(r))) return;
    printf("Nome do arquivo: ");
    ler_texto(nome, sizeof(nome));
    if(!falhou(mg_add(r, nome)))
        printf("%s entrou no fim do staging.\n", nome);
}

void cli_ver_proximo(Repositorio *r){
    Arquivo a;
    if(!front(r->staging, &a)){
        falhou(MG_STAGING_VAZIO);
        return;
    }
    printf("Proximo a sair: %s (add %d)\n", a.nome, a.ordem);
}

void cli_status(Repositorio *r){
    Arquivo a;
    if(queue_isempty(r->staging)){
        printf("Staging vazio.\n");
        return;
    }
    printf("Staging (%d de %d), da frente para o fim:\n", queue_len(r->staging), MAX_QUEUE);
    for(int i = 0; queue_get(r->staging, i, &a); i++)
        printf("  %d. %s (add %d)\n", i + 1, a.nome, a.ordem);
}

void cli_unstage(Repositorio *r){
    Arquivo a;
    if(!falhou(mg_unstage_primeiro(r, &a)))
        printf("%s saiu do staging (nao vai para o proximo commit).\n", a.nome);
}

/* ---------- historico (lista) ---------- */

void cli_commit(Repositorio *r){
    char mensagem[TAM_TEXTO], autor[TAM_NOME];
    Commit c;

    if(falhou(mg_pode_commit(r))) return;
    printf("Mensagem do commit: ");
    ler_texto(mensagem, sizeof(mensagem));
    printf("Autor: ");
    ler_texto(autor, sizeof(autor));

    if(falhou(mg_commit(r, mensagem, autor, &c))) return;
    printf("Commit %d criado com %d arquivo(s):\n", c.id, c.qtd_arquivos);
    mostrar_commit(c);
    printf("Staging vazio.\n");
}

void cli_log(Repositorio *r){
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

/* Le um id; devolve false (ja avisando) se nao for numero. */
static bool ler_id(int *id){
    printf("Id do commit: ");
    if(ler_numero(id)) return true;
    printf("Erro: digite um numero.\n");
    return false;
}

void cli_buscar_commit(Repositorio *r){
    Commit c;
    int id;
    if(lst_len(r->historico) == 0){
        falhou(MG_HISTORICO_VAZIO);
        return;
    }
    if(!ler_id(&id)) return;
    if(!lst_get(r->historico, lst_localizar(r->historico, id), &c)){
        falhou(MG_COMMIT_INEXISTENTE);
        return;
    }
    mostrar_commit(c);
}

void cli_remover_commit(Repositorio *r){
    Commit c;
    int id;
    if(lst_len(r->historico) == 0){
        falhou(MG_HISTORICO_VAZIO);
        return;
    }
    if(!ler_id(&id)) return;
    if(!falhou(mg_remover_commit(r, id, &c)))
        printf("Commit %d removido: %s\n", c.id, c.mensagem);
}

/* ---------- stash (pilha) ---------- */

void cli_stash(Repositorio *r){
    char descricao[TAM_TEXTO];
    Stash s;
    if(falhou(mg_pode_stash(r))) return;
    printf("Descricao do trabalho: ");
    ler_texto(descricao, sizeof(descricao));
    if(!falhou(mg_stash(r, descricao, &s)))
        printf("Stash %d guardado no topo com %d arquivo(s). Staging vazio.\n", s.id, s.qtd_arquivos);
}

void cli_stash_pop(Repositorio *r){
    Stash s;
    int devolvidos;
    if(falhou(mg_stash_pop(r, &s, &devolvidos))) return;
    printf("Stash %d recuperado: \"%s\"\n", s.id, s.descricao);
    printf("%d arquivo(s) voltaram para o fim do staging", devolvidos);
    if(devolvidos < s.qtd_arquivos)
        printf(" (%d ja estava(m) la)", s.qtd_arquivos - devolvidos);
    printf(".\n");
}

void cli_stash_topo(Repositorio *r){
    Stash s;
    if(!top(&r->stash, &s)){
        falhou(MG_STASH_VAZIO);
        return;
    }
    printf("Topo:\n");
    mostrar_stash(0, s);
}

void cli_stash_listar(Repositorio *r){
    Stash s;
    if(stack_isempty(&r->stash)){
        printf("Nenhum stash guardado.\n");
        return;
    }
    printf("Stashes (%d de %d), do topo para a base:\n", stack_len(&r->stash), STACK_MAX);
    for(int i = 0; stack_get(&r->stash, i, &s); i++)
        mostrar_stash(i, s);
}

void cli_stash_drop(Repositorio *r){
    Stash s;
    if(!falhou(mg_stash_drop(r, &s)))
        printf("Stash %d descartado: \"%s\"\n", s.id, s.descricao);
}
