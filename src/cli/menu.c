#include <stdio.h>
#include "menu.h"
#include "entrada.h"
#include "comandos.h"

typedef struct {
    const char *secao;               /* titulo do grupo, ou NULL para seguir no mesmo */
    const char *texto;
    void (*executar)(Repositorio *r);
} Opcao;

/* Para criar um comando novo, basta uma linha aqui. O numero e a posicao + 1. */
static const Opcao OPCOES[] = {
    { "Staging (fila)",      "Adicionar arquivo (add / enqueue)",            cli_add },
    { NULL,                  "Ver o proximo arquivo (front)",                cli_ver_proximo },
    { NULL,                  "Status do staging",                            cli_status },
    { NULL,                  "Tirar o primeiro arquivo do staging (dequeue)", cli_unstage },
    { "Historico (lista)",   "Fazer commit (staging -> historico)",          cli_commit },
    { NULL,                  "Ver historico (log)",                          cli_log },
    { NULL,                  "Buscar commit pelo id",                        cli_buscar_commit },
    { NULL,                  "Remover commit pelo id",                       cli_remover_commit },
    { "Stash (pilha)",       "Guardar staging no stash (push)",              cli_stash },
    { NULL,                  "Recuperar o ultimo stash (pop -> staging)",    cli_stash_pop },
    { NULL,                  "Ver o stash do topo (top)",                    cli_stash_topo },
    { NULL,                  "Listar stashes",                               cli_stash_listar },
    { NULL,                  "Descartar o ultimo stash (drop)",              cli_stash_drop },
};

#define QTD_OPCOES ((int)(sizeof(OPCOES) / sizeof(OPCOES[0])))

static void mostrar_menu(void){
    for(int i = 0; i < QTD_OPCOES; i++){
        if(OPCOES[i].secao != NULL)
            printf("\n-- %s --\n", OPCOES[i].secao);
        printf("%2d - %s\n", i + 1, OPCOES[i].texto);
    }
    printf("\n 0 - Sair\n");
    printf("Opcao: ");
}

void menu_executar(Repositorio *r){
    int opcao;

    printf("=== MINI-GIT ===\n");
    for(;;){
        mostrar_menu();
        if(!ler_numero(&opcao)){
            if(fim_da_entrada()) break;
            printf("Erro: opcao invalida, digite um numero.\n");
            continue;
        }
        if(opcao == 0) break;
        if(opcao < 1 || opcao > QTD_OPCOES){
            printf("Erro: opcao %d nao existe.\n", opcao);
            continue;
        }
        OPCOES[opcao - 1].executar(r);
    }
    printf("\nSaindo do mini-git.\n");
}
