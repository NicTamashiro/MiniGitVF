#include <stdio.h>
#include <string.h>
#include "fila.h"

void ler_texto(char *texto, int tamanho){
    if(fgets(texto, tamanho, stdin) == NULL){
        texto[0] = '\0';
        return;
    }
    if(strchr(texto, '\n') == NULL){       
        int c;                             
        while((c = getchar()) != '\n' && c != EOF);
    }
    texto[strcspn(texto, "\n")] = '\0';
}


int ler_numero(int *numero){
    char texto[50];
    char sobra;
    ler_texto(texto, sizeof(texto));
    return sscanf(texto, "%d %c", numero, &sobra) == 1;
}

int esta_no_staging(fila staging, char *nome){
    int i, pos = staging.comeco;
    for(i = 0; i < staging.qtd_elementos; i++){
        if(strcmp(staging.dados[pos].nome, nome) == 0)
            return 1;
        pos = (pos + 1) % MAX_QUEUE;        
    }
    return 0;
}

void adicionar(fila *staging, int *proximo_add){
    Arquivo a;

    if(queue_isfull(*staging)){
        printf("Erro: staging cheio (maximo de %d). Faca um commit antes.\n", MAX_QUEUE);
        return;
    }

    printf("Nome do arquivo: ");
    ler_texto(a.nome, sizeof(a.nome));
    if(strlen(a.nome) == 0 || strchr(a.nome, ' ') != NULL){
        printf("Erro: nome invalido (vazio ou com espaco).\n");
        return;
    }
    if(esta_no_staging(*staging, a.nome)){
        printf("Erro: %s ja esta no staging.\n", a.nome);
        return;
    }

    a.ordem = *proximo_add;
    if(enqueue(staging, a)){
        printf("%s entrou no fim do staging.\n", a.nome);
        (*proximo_add)++;
    }
}

void fazer_commit(fila *staging, int *proximo_commit){
    char mensagem[50];
    Arquivo a;

    if(queue_isempty(*staging)){
        printf("Erro: staging vazio, nada para commitar.\n");
        return;
    }

    printf("Mensagem do commit: ");
    ler_texto(mensagem, sizeof(mensagem));
    if(strlen(mensagem) == 0){
        printf("Erro: a mensagem nao pode ficar vazia.\n");
        return;
    }

    printf("Commit %d \"%s\" com os arquivos:\n", *proximo_commit, mensagem);

    while(dequeue(staging, &a))
        printf("  %s (add %d)\n", a.nome, a.ordem);
    printf("Staging vazio.\n");
    (*proximo_commit)++;
}

void ver_proximo(fila *staging){
    Arquivo a;
    if(!front(*staging, &a)){
        printf("Erro: staging vazio, nao ha proximo arquivo.\n");
        return;
    }
    printf("Proximo a sair: %s (add %d)\n", a.nome, a.ordem);
}

void mostrar_status(fila *staging){
    int i, pos;
    if(queue_isempty(*staging)){
        printf("Staging vazio.\n");
        return;
    }
    printf("Staging (%d de %d), da frente para o fim:\n", staging->qtd_elementos, MAX_QUEUE);
    pos = staging->comeco;
    for(i = 0; i < staging->qtd_elementos; i++){
        printf("  %d. %s (add %d)\n", i + 1, staging->dados[pos].nome, staging->dados[pos].ordem);
        pos = (pos + 1) % MAX_QUEUE;
    }
}

int main(){
    fila staging;
    queue_init(&staging);     
    int proximo_add = 1;
    int proximo_commit = 1;
    int opcao;

    printf("=== MINI-GIT: staging (fila) ===\n");
    while(1){
        printf("\n1 - Adicionar arquivo ao staging (enqueue)\n");
        printf("2 - Fazer commit (dequeue ate esvaziar)\n");
        printf("3 - Ver o proximo arquivo (front)\n");
        printf("4 - Status do staging\n");
        printf("0 - Sair\n");
        printf("Opcao: ");

        if(!ler_numero(&opcao)){
            if(feof(stdin)) break;          
            printf("Erro: opcao invalida, digite um numero.\n");
            continue;
        }

        if(opcao == 1) adicionar(&staging, &proximo_add);
        else if(opcao == 2) fazer_commit(&staging, &proximo_commit);
        else if(opcao == 3) ver_proximo(&staging);
        else if(opcao == 4) mostrar_status(&staging);
        else if(opcao == 0) break;
        else printf("Erro: opcao %d nao existe.\n", opcao);
    }

    printf("Saindo do mini-git.\n");
    return 0;
}
