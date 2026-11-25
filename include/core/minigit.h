#ifndef MINIGIT_H_
#define MINIGIT_H_
#include <stdbool.h>
#include "repositorio.h"

/* Regras do mini-git. Este modulo NAO le o teclado e NAO imprime nada:
 * recebe dados prontos e devolve um mg_status. Cada interface (CLI, TUI...)
 * decide como pedir os dados e como mostrar o resultado.
 *
 * Para LER o estado (listar staging, stash, historico), as interfaces usam
 * as funcoes de leitura das estruturas: queue_get, stack_get e lst_get. */

typedef enum {
    MG_OK = 0,
    MG_STAGING_VAZIO,
    MG_STAGING_CHEIO,
    MG_NOME_INVALIDO,
    MG_JA_NO_STAGING,
    MG_TEXTO_VAZIO,
    MG_HISTORICO_VAZIO,
    MG_HISTORICO_CHEIO,
    MG_COMMIT_INEXISTENTE,
    MG_STASH_VAZIO,
    MG_STASH_CHEIO,
    MG_SEM_ESPACO           /* stash pop nao cabe no staging */
} mg_status;

/* Texto do resultado, o mesmo para todas as interfaces. */
const char *mg_mensagem(mg_status s);

/* Pre-condicoes: a interface pode perguntar ANTES de pedir dados ao usuario
 * (nao faz sentido pedir a mensagem do commit com o staging vazio). */
mg_status mg_pode_add(const Repositorio *r);
mg_status mg_pode_commit(const Repositorio *r);
mg_status mg_pode_stash(const Repositorio *r);

bool mg_staging_contem(const Repositorio *r, const char *nome);

/* Staging (fila) */
mg_status mg_add(Repositorio *r, const char *nome);
mg_status mg_unstage_primeiro(Repositorio *r, Arquivo *removido);

/* Historico (lista) - commit move o staging inteiro para o fim da lista */
mg_status mg_commit(Repositorio *r, const char *mensagem, const char *autor, Commit *criado);
mg_status mg_remover_commit(Repositorio *r, int id, Commit *removido);

/* Stash (pilha) */
mg_status mg_stash(Repositorio *r, const char *descricao, Stash *criado);
mg_status mg_stash_pop(Repositorio *r, Stash *recuperado, int *devolvidos);
mg_status mg_stash_drop(Repositorio *r, Stash *descartado);

#endif
