#ifndef PAINEIS_H_
#define PAINEIS_H_
#include "estado.h"

/* Desenho da tela. So LE o estado (const Tui *): nunca altera o repositorio. */

#define TUI_MIN_COLUNAS 72
#define TUI_MIN_LINHAS  18

void paineis_iniciar_cores(void);
void paineis_desenhar(const Tui *t);

#endif
