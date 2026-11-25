#include <stdio.h>
#include <string.h>
#include "entrada.h"

void ler_texto(char *texto, int tamanho){
    if(fgets(texto, tamanho, stdin) == NULL){
        texto[0] = '\0';
        return;
    }
    if(strchr(texto, '\n') == NULL){        /* linha maior que o vetor: descarta o resto */
        int c;
        while((c = getchar()) != '\n' && c != EOF);
    }
    texto[strcspn(texto, "\n")] = '\0';
}

bool ler_numero(int *numero){
    char texto[50];
    char sobra;
    ler_texto(texto, sizeof(texto));
    return sscanf(texto, "%d %c", numero, &sobra) == 1;     /* recusa "3a", "abc"... */
}

bool fim_da_entrada(void){
    return feof(stdin);
}
