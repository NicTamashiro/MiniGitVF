#include <curses.h>
#include "teclas.h"
#include "acoes.h"

#define TAB '\t'

static const Tecla TECLAS[] = {
    /* painel          tecla       rotulo   descricao                     barra  acao */
    { P_STAGING,       'a',        "a",     "add (enqueue)",              true,  acao_add },
    { P_STAGING,       'u',        "u",     "tirar o primeiro (dequeue)", true,  acao_unstage },
    { P_STAGING,       's',        "s",     "stash (push)",               true,  acao_stash },
    { P_COMMITS,       'd',        "d",     "remover commit",             true,  acao_remover_commit },
    { P_STASH,         'g',        "g",     "stash pop",                  true,  acao_stash_pop },
    { P_STASH,         'd',        "d",     "stash drop",                 true,  acao_stash_drop },

    { TODOS_PAINEIS,   'a',        "a",     "add",                        false, acao_add },
    { TODOS_PAINEIS,   'c',        "c",     "commit",                     true,  acao_commit },
    { TODOS_PAINEIS,   TAB,        "Tab",   "proximo painel",             true,  acao_proximo_painel },
    { TODOS_PAINEIS,   KEY_BTAB,   "S-Tab", "painel anterior",            false, acao_painel_anterior },
    { TODOS_PAINEIS,   'l',        "l",     "proximo painel",             false, acao_proximo_painel },
    { TODOS_PAINEIS,   'h',        "h",     "painel anterior",            false, acao_painel_anterior },
    { TODOS_PAINEIS,   '1',        "1",     "painel Status",              false, acao_painel_status },
    { TODOS_PAINEIS,   '2',        "2",     "painel Staging",             false, acao_painel_staging },
    { TODOS_PAINEIS,   '3',        "3",     "painel Commits",             false, acao_painel_commits },
    { TODOS_PAINEIS,   '4',        "4",     "painel Stash",               false, acao_painel_stash },
    { TODOS_PAINEIS,   'j',        "j/k",   "mover",                      true,  acao_descer },
    { TODOS_PAINEIS,   'k',        "k",     "subir",                      false, acao_subir },
    { TODOS_PAINEIS,   KEY_DOWN,   "Baixo", "descer",                     false, acao_descer },
    { TODOS_PAINEIS,   KEY_UP,     "Cima",  "subir",                      false, acao_subir },
    { TODOS_PAINEIS,   '?',        "?",     "ajuda",                      true,  acao_ajuda },
    { TODOS_PAINEIS,   'q',        "q",     "sair",                       true,  acao_sair },
};

#define QTD_TECLAS ((int)(sizeof(TECLAS) / sizeof(TECLAS[0])))

int teclas_qtd(void){
    return QTD_TECLAS;
}

const Tecla *teclas_item(int i){
    return (i >= 0 && i < QTD_TECLAS) ? &TECLAS[i] : NULL;
}

bool teclas_vale_em(const Tecla *k, Painel p){
    return k->painel == (int)p || k->painel == TODOS_PAINEIS;
}

bool teclas_encoberta(const Tecla *k, Painel p){
    if(k->painel != TODOS_PAINEIS) return false;
    for(int i = 0; i < QTD_TECLAS; i++)
        if(TECLAS[i].painel == (int)p && TECLAS[i].tecla == k->tecla)
            return true;
    return false;
}

bool teclas_despachar(Tui *t, int tecla){
    /* Primeiro as teclas do painel ativo, depois as globais:
     * assim o 'd' remove commit num painel e descarta stash no outro. */
    for(int passo = 0; passo < 2; passo++){
        for(int i = 0; i < QTD_TECLAS; i++){
            const Tecla *k = &TECLAS[i];
            int alvo = passo == 0 ? (int)t->ativo : TODOS_PAINEIS;
            if(k->tecla == tecla && k->painel == alvo){
                k->acao(t);
                return true;
            }
        }
    }
    return false;
}
