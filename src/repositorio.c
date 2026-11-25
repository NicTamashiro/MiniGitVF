#include "repositorio.h"

void repo_init(Repositorio *r){
    queue_init(&r->staging);
    stack_init(&r->stash);
    lst_init(&r->historico);
    r->proximo_add = 1;
    r->proximo_stash = 1;
    r->proximo_commit = 1;
}
