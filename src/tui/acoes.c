#include <stdio.h>
#include "acoes.h"
#include "minigit.h"
#include "popup.h"

/* Mostra o erro do core na linha de aviso e diz se falhou. */
static bool falhou(Tui *t, mg_status s){
    if(s == MG_OK) return false;
    estado_avisar(t, true, "Erro: %s.", mg_mensagem(s));
    return true;
}

static void ir_para(Tui *t, Painel p, int linha){
    t->ativo = p;
    t->cursor[p] = linha;
    estado_ajustar_cursores(t);
}

/* ---------- navegacao ---------- */

void acao_sair(Tui *t)            { t->sair = true; }
void acao_ajuda(Tui *t)           { t->ajuda = true; }
void acao_proximo_painel(Tui *t)  { t->ativo = (Painel)((t->ativo + 1) % P_QTD); }
void acao_painel_anterior(Tui *t) { t->ativo = (Painel)((t->ativo + P_QTD - 1) % P_QTD); }
void acao_painel_status(Tui *t)   { t->ativo = P_STATUS; }
void acao_painel_staging(Tui *t)  { t->ativo = P_STAGING; }
void acao_painel_commits(Tui *t)  { t->ativo = P_COMMITS; }
void acao_painel_stash(Tui *t)    { t->ativo = P_STASH; }

void acao_descer(Tui *t){
    if(t->cursor[t->ativo] < estado_qtd_itens(t, t->ativo) - 1)
        t->cursor[t->ativo]++;
}

void acao_subir(Tui *t){
    if(t->cursor[t->ativo] > 0)
        t->cursor[t->ativo]--;
}

/* ---------- staging (fila) ---------- */

void acao_add(Tui *t){
    char nome[TAM_NOME];
    if(falhou(t, mg_pode_add(t->repo))) return;
    if(!popup_ler("add", "Nome do arquivo:", nome, sizeof(nome))) return;
    if(falhou(t, mg_add(t->repo, nome))) return;
    ir_para(t, P_STAGING, queue_len(t->repo->staging) - 1);
    estado_avisar(t, false, "%s entrou no fim do staging (enqueue).", nome);
}

void acao_unstage(Tui *t){
    Arquivo a;
    if(falhou(t, mg_unstage_primeiro(t->repo, &a))) return;
    estado_ajustar_cursores(t);
    estado_avisar(t, false, "%s saiu da frente do staging (dequeue).", a.nome);
}

/* ---------- historico (lista) ---------- */

void acao_commit(Tui *t){
    char mensagem[TAM_TEXTO], autor[TAM_NOME];
    Commit c;
    if(falhou(t, mg_pode_commit(t->repo))) return;
    if(!popup_ler("commit", "Mensagem:", mensagem, sizeof(mensagem))) return;
    if(!popup_ler("commit", "Autor:", autor, sizeof(autor))) return;
    if(falhou(t, mg_commit(t->repo, mensagem, autor, &c))) return;
    ir_para(t, P_COMMITS, 0);
    estado_avisar(t, false, "Commit %d criado com %d arquivo(s). Staging vazio.", c.id, c.qtd_arquivos);
}

void acao_remover_commit(Tui *t){
    Commit c;
    char pergunta[TAM_TEXTO + 32];
    if(!estado_commit_na_linha(t, t->cursor[P_COMMITS], &c)){
        falhou(t, MG_HISTORICO_VAZIO);
        return;
    }
    snprintf(pergunta, sizeof(pergunta), "Remover o commit %d \"%s\"?", c.id, c.mensagem);
    if(!popup_confirmar("remover commit", pergunta)) return;
    if(falhou(t, mg_remover_commit(t->repo, c.id, &c))) return;
    estado_ajustar_cursores(t);
    estado_avisar(t, false, "Commit %d removido da lista.", c.id);
}

/* ---------- stash (pilha) ---------- */

void acao_stash(Tui *t){
    char descricao[TAM_TEXTO];
    Stash s;
    if(falhou(t, mg_pode_stash(t->repo))) return;
    if(!popup_ler("stash", "Descricao:", descricao, sizeof(descricao))) return;
    if(falhou(t, mg_stash(t->repo, descricao, &s))) return;
    ir_para(t, P_STASH, 0);
    estado_avisar(t, false, "Stash %d guardado no topo (push) com %d arquivo(s).", s.id, s.qtd_arquivos);
}

void acao_stash_pop(Tui *t){
    Stash s;
    int devolvidos;
    if(falhou(t, mg_stash_pop(t->repo, &s, &devolvidos))) return;
    estado_ajustar_cursores(t);
    estado_avisar(t, false, "Stash %d recuperado (pop): %d arquivo(s) voltaram ao staging.", s.id, devolvidos);
}

void acao_stash_drop(Tui *t){
    Stash s;
    char pergunta[TAM_TEXTO + 32];
    if(!top(&t->repo->stash, &s)){
        falhou(t, MG_STASH_VAZIO);
        return;
    }
    snprintf(pergunta, sizeof(pergunta), "Descartar stash@{0} \"%s\"?", s.descricao);
    if(!popup_confirmar("stash drop", pergunta)) return;
    if(falhou(t, mg_stash_drop(t->repo, &s))) return;
    estado_ajustar_cursores(t);
    estado_avisar(t, false, "Stash %d descartado (pop sem devolver).", s.id);
}
