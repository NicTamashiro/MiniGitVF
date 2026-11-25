#ifndef COMANDOS_H_
#define COMANDOS_H_
#include "repositorio.h"

/* Comandos da interface de texto (menu numerado). Cada um so pede os dados,
 * chama o core (minigit.h) e imprime o resultado. As regras ficam no core. */

void cli_add(Repositorio *r);
void cli_ver_proximo(Repositorio *r);
void cli_status(Repositorio *r);
void cli_unstage(Repositorio *r);

void cli_commit(Repositorio *r);
void cli_log(Repositorio *r);
void cli_buscar_commit(Repositorio *r);
void cli_remover_commit(Repositorio *r);

void cli_stash(Repositorio *r);
void cli_stash_pop(Repositorio *r);
void cli_stash_topo(Repositorio *r);
void cli_stash_listar(Repositorio *r);
void cli_stash_drop(Repositorio *r);

#endif
