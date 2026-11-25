#ifndef ESTADO_H_
#define ESTADO_H_
#include <stdbool.h>
#include "repositorio.h"

/* Estado da TELA (qual painel esta ativo, onde esta cada cursor, o aviso da
 * linha de baixo). Fica fora do Repositorio: e coisa da interface, nao do git. */

typedef enum { P_STATUS, P_STAGING, P_COMMITS, P_STASH, P_QTD } Painel;

typedef struct {
    Repositorio *repo;
    Painel ativo;
    int    cursor[P_QTD];
    char   aviso[160];
    bool   aviso_erro;
    bool   ajuda;           /* janela de ajuda aberta */
    bool   sair;
} Tui;

void estado_iniciar(Tui *t, Repositorio *r);
int  estado_qtd_itens(const Tui *t, Painel p);
void estado_ajustar_cursores(Tui *t);
void estado_avisar(Tui *t, bool erro, const char *formato, ...);

/* O painel de commits mostra o mais novo em cima, como o git log. */
bool estado_commit_na_linha(const Tui *t, int linha, Commit *c);

#endif
