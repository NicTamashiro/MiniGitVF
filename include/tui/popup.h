#ifndef POPUP_H_
#define POPUP_H_
#include <stdbool.h>

/* Janelas que aparecem por cima da tela. Nao conhecem o mini-git:
 * so pedem um texto ou uma confirmacao. */

/* Caixa de texto. Devolve false se o usuario apertar Esc. */
bool popup_ler(const char *titulo, const char *rotulo, char *texto, int tamanho);

/* Pergunta de sim/nao. Devolve true para 's' ou 'y'. */
bool popup_confirmar(const char *titulo, const char *pergunta);

#endif
