# Entrega 4 – Fila no Mini-git

**Grupo:** MINI GIT – Projeto de Algoritmos, Universidade Católica de Santos

## Como compilar e rodar

```
make
./minigit
```

No Windows com MinGW, use `mingw32-make` e depois `minigit.exe`. Sem o `make`:

```
gcc -Wall -Wextra -o minigit main.c fila.c
```

O programa compila sem erros e sem avisos.

## Onde a Fila foi aplicada e por quê

A Fila guarda o **staging** (área de preparação) do mini-git. Antes de um commit, o
programador escolhe com `add` quais arquivos vão entrar nele. Cada `add` coloca o arquivo
no **fim** da fila, e o `commit` tira os arquivos pela **frente**, um a um, até a fila
esvaziar. Assim, os arquivos entram no commit na mesma ordem em que foram adicionados: o
primeiro a entrar é o primeiro a sair (FIFO).

Uma pilha levaria os arquivos na ordem inversa (o último `add` sairia primeiro), e uma
lista exigiria deslocar itens para tirar do começo do vetor. Na fila circular, colocar e
tirar mexem só nas pontas, então custam O(1).

Cada item da fila é um arquivo, guardado numa struct no lugar do número da aula:

```c
typedef struct {
    int  ordem;            /* numero do add: 1, 2, 3...  */
    char nome[30];         /* ex.: "login.c"             */
} Arquivo;

typedef Arquivo queue_info;
```

Como o código da aula já usava `typedef int queue_info;`, bastou trocar o `int` pela
struct `Arquivo`: as funções da fila continuaram com os mesmos nomes.

| Opção do menu | Função da fila usada |
|---|---|
| 1 - Adicionar arquivo ao staging | `queue_isfull` e `enqueue` |
| 2 - Fazer commit | `queue_isempty` e `dequeue` até esvaziar |
| 3 - Ver o próximo arquivo | `front` |
| 4 - Status do staging | `queue_isempty` e leitura de `dados[]` da frente para o fim |
| início do programa | `queue_init` |

O staging guarda no máximo 5 arquivos (`MAX_QUEUE`).

## Como a fila funciona por dentro

A fila é um vetor `dados[MAX_QUEUE]` com três inteiros:

- `comeco`: posição de quem sai primeiro (a frente);
- `fim`: primeira posição livre, logo atrás do último;
- `qtd_elementos`: quantos itens existem.

`enqueue` grava em `dados[fim]` e avança o `fim`; `dequeue` lê `dados[comeco]` e avança
o `comeco`. Os dois avançam com `(posição + 1) % MAX_QUEUE`: depois da última posição,
voltam para a 0, como se o vetor fosse um anel. Assim, as posições liberadas pelo commit
são reaproveitadas, e nenhum item é deslocado.

O `qtd_elementos` é necessário porque `comeco == fim` acontece tanto com a fila vazia
quanto com a fila cheia; só a quantidade diferencia os dois casos.

Todas as operações da fila custam O(1). São O(n) só o status, a busca de arquivo
repetido e o commit, que tira os n arquivos.

## Casos especiais

| Caso | O que acontece |
|---|---|
| Fila vazia | Commit, ver o próximo ou status sem nenhum arquivo mostra um aviso |
| Fila cheia | O 6º arquivo é recusado: "staging cheio (maximo de 5)" |
| Valor inválido | Letra no lugar de número, opção que não existe, nome vazio ou com espaço, arquivo que já está no staging, mensagem de commit vazia: o programa avisa e volta ao menu |

## Exemplo de uso

```
=== MINI-GIT: staging (fila) ===

1 - Adicionar arquivo ao staging (enqueue)
2 - Fazer commit (dequeue ate esvaziar)
3 - Ver o proximo arquivo (front)
4 - Status do staging
0 - Sair
Opcao: 1
Nome do arquivo: login.c
login.c entrou no fim do staging.

Opcao: 1
Nome do arquivo: login.h
login.h entrou no fim do staging.

Opcao: 4
Staging (2 de 5), da frente para o fim:
  1. login.c (add 1)
  2. login.h (add 2)

Opcao: 3
Proximo a sair: login.c (add 1)

Opcao: 2
Mensagem do commit: tela de login
Commit 1 "tela de login" com os arquivos:
  login.c (add 1)
  login.h (add 2)
Staging vazio.

Opcao: 2
Erro: staging vazio, nada para commitar.

Opcao: 0
Saindo do mini-git.
```

(O menu aparece antes de cada "Opcao:"; aqui ele foi mostrado só na primeira vez.)

O `login.c` entrou primeiro e saiu primeiro: é o comportamento FIFO da fila.
