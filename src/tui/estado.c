#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "estado.h"

void estado_iniciar(Tui *t, Repositorio *r){
    memset(t, 0, sizeof(*t));
    t->repo = r;
    t->ativo = P_STAGING;
}

int estado_qtd_itens(const Tui *t, Painel p){
    switch(p){
    case P_STAGING: return queue_len(t->repo->staging);
    case P_COMMITS: return lst_len(t->repo->historico);
    case P_STASH:   return stack_len(&t->repo->stash);
    default:        return 0;
    }
}

void estado_ajustar_cursores(Tui *t){
    for(int p = 0; p < P_QTD; p++){
        int qtd = estado_qtd_itens(t, (Painel)p);
        if(t->cursor[p] >= qtd) t->cursor[p] = qtd - 1;
        if(t->cursor[p] < 0)    t->cursor[p] = 0;
    }
}

void estado_avisar(Tui *t, bool erro, const char *formato, ...){
    va_list args;
    va_start(args, formato);
    vsnprintf(t->aviso, sizeof(t->aviso), formato, args);
    va_end(args);
    t->aviso_erro = erro;
}

bool estado_commit_na_linha(const Tui *t, int linha, Commit *c){
    return lst_get(t->repo->historico, lst_len(t->repo->historico) - 1 - linha, c);
}
