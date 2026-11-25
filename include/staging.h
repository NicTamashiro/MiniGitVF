#ifndef STAGING_H_
#define STAGING_H_
#include "repositorio.h"

/* Comandos do staging (fila). */
void staging_adicionar(Repositorio *r);     /* git add             */
void staging_ver_proximo(Repositorio *r);   /* front               */
void staging_status(Repositorio *r);        /* git status          */
void staging_desfazer_primeiro(Repositorio *r); /* git restore --staged do primeiro */

/* Usado pelos outros modulos: o staging nao aceita nomes repetidos. */
bool staging_contem(fila f, const char *nome);

#endif
