#ifndef TECLAS_H_
#define TECLAS_H_
#include <stdbool.h>
#include "estado.h"

/* Tabela de atalhos, como a OPCOES[] do menu da CLI. A barra de baixo e a
 * ajuda (?) sao geradas a partir dela: nunca ficam diferentes do que as
 * teclas fazem. Tecla nova = uma acao + uma linha na tabela. */

#define TODOS_PAINEIS (-1)

typedef struct {
    int         painel;         /* Painel, ou TODOS_PAINEIS          */
    int         tecla;          /* caractere ou KEY_* do curses      */
    const char *rotulo;         /* como aparece: "a", "Tab", "j/k"   */
    const char *descricao;
    bool        na_barra;       /* aparece na barra de baixo?        */
    void      (*acao)(Tui *t);
} Tecla;

int          teclas_qtd(void);
const Tecla *teclas_item(int i);
bool         teclas_vale_em(const Tecla *k, Painel p);

/* true se a tecla global k e encoberta por uma do painel p (mesma tecla). */
bool         teclas_encoberta(const Tecla *k, Painel p);

/* Executa a acao da tecla no painel ativo. false = tecla sem acao. */
bool teclas_despachar(Tui *t, int tecla);

#endif
