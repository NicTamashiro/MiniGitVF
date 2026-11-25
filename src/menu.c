#include <stdio.h>
#include "menu.h"
#include "entrada.h"
#include "staging.h"
#include "historico.h"
#include "stash.h"

typedef struct {
    const char *secao;               /* titulo do grupo, ou NULL para seguir no mesmo */
    const char *texto;
    void (*executar)(Repositorio *r);
} Opcao;

/* Para criar um comando novo, basta uma linha aqui. O numero e a posicao + 1. */
static const Opcao OPCOES[] = {
    { "Staging (fila)",      "Adicionar arquivo (add / enqueue)",            staging_adicionar },
    { NULL,                  "Ver o proximo arquivo (front)",                staging_ver_proximo },
    { NULL,                  "Status do staging",                            staging_status },
    { NULL,                  "Tirar o primeiro arquivo do staging (dequeue)", staging_desfazer_primeiro },
    { "Historico (lista)",   "Fazer commit (staging -> historico)",          historico_commit },
    { NULL,                  "Ver historico (log)",                          historico_log },
    { NULL,                  "Buscar commit pelo id",                        historico_buscar },
    { NULL,                  "Remover commit pelo id",                       historico_remover },
    { "Stash (pilha)",       "Guardar staging no stash (push)",              stash_guardar },
    { NULL,                  "Recuperar o ultimo stash (pop -> staging)",    stash_recuperar },
    { NULL,                  "Ver o stash do topo (top)",                    stash_ver_topo },
    { NULL,                  "Listar stashes",                               stash_listar },
    { NULL,                  "Descartar o ultimo stash (drop)",              stash_descartar },
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
