#ifndef ENTRADA_H_
#define ENTRADA_H_
#include <stdbool.h>

/* Leitura segura do teclado: nada de scanf solto deixando lixo no buffer. */

void ler_texto(char *texto, int tamanho);   /* le uma linha sem o '\n' */
bool ler_numero(int *numero);               /* true so se a linha for um inteiro */
bool fim_da_entrada(void);                  /* true depois de Ctrl+D / EOF */

#endif
