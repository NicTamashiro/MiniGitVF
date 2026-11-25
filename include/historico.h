#ifndef HISTORICO_H_
#define HISTORICO_H_
#include "repositorio.h"

/* Comandos do historico de commits (lista). */
void historico_commit(Repositorio *r);      /* git commit: esvazia o staging */
void historico_log(Repositorio *r);         /* git log                       */
void historico_buscar(Repositorio *r);      /* git show <id>                 */
void historico_remover(Repositorio *r);     /* apaga um commit pelo id       */

#endif
