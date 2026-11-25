#ifndef STASH_H_
#define STASH_H_
#include "repositorio.h"

/* Comandos do stash (pilha). */
void stash_guardar(Repositorio *r);         /* git stash: move o staging para o topo */
void stash_recuperar(Repositorio *r);       /* git stash pop: devolve ao staging     */
void stash_ver_topo(Repositorio *r);        /* git stash show                        */
void stash_listar(Repositorio *r);          /* git stash list                        */
void stash_descartar(Repositorio *r);       /* git stash drop                        */

#endif
