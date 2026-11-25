#ifndef ACOES_H_
#define ACOES_H_
#include "estado.h"

/* O que cada tecla faz. Cada acao pede dados por popup, chama o core
 * (minigit.h) e deixa o resultado em t->aviso. Nenhuma desenha painel. */

/* navegacao */
void acao_sair(Tui *t);
void acao_ajuda(Tui *t);
void acao_proximo_painel(Tui *t);
void acao_painel_anterior(Tui *t);
void acao_painel_status(Tui *t);
void acao_painel_staging(Tui *t);
void acao_painel_commits(Tui *t);
void acao_painel_stash(Tui *t);
void acao_descer(Tui *t);
void acao_subir(Tui *t);

/* mini-git */
void acao_add(Tui *t);
void acao_unstage(Tui *t);
void acao_commit(Tui *t);
void acao_remover_commit(Tui *t);
void acao_stash(Tui *t);
void acao_stash_pop(Tui *t);
void acao_stash_drop(Tui *t);

#endif
