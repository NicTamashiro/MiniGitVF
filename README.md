# Mini-Git – Lista, Pilha e Fila integradas

**Grupo:** MINI GIT – Projeto de Algoritmos, Universidade Católica de Santos

Este repositório junta as três entregas anteriores num único programa:

| Entrega | Estrutura | Parte do mini-git | Repositório de origem |
|---|---|---|---|
| 2 | Lista sequencial | Histórico de commits (`log`) | `MiniGitEntrega2` |
| 3 | Pilha | Stash (`stash`, `stash pop`) | `MiniGitPilha` |
| 4 | Fila circular | Staging (`add`) | `MiniGitFila` |

Separadas, cada estrutura vivia num programa próprio. Juntas, elas formam o fluxo de um
Git de verdade: o arquivo entra no **staging** (fila), o **commit** esvazia o staging e
grava no **histórico** (lista), e o **stash** guarda o staging pela metade numa **pilha**
para recuperar depois.

![Fluxo do mini-git: add coloca o arquivo no staging (fila); commit move o staging para o histórico (lista); stash move o staging para a pilha; stash pop devolve ao staging](docs/fluxo.svg)

## Como compilar e rodar

```
make
./minigit
```

Ou `make run`. No Windows com MinGW, use `mingw32-make` e depois `minigit.exe`. Sem o
`make`:

```
gcc -Wall -Wextra -std=c99 -Iinclude -o minigit src/*.c
```

O programa compila sem erros e sem avisos (também com `-Wpedantic` e com
`-fsanitize=address,undefined`).

## Organização do código

```
include/            cabeçalhos (o que cada módulo oferece)
  tipos.h           Arquivo, Stash, Commit e os tamanhos máximos
  fila.h            fila circular   (queue_info = Arquivo)
  pilha.h           pilha           (stack_info = Stash)
  lista.h           lista sequencial (lst_info  = Commit)
  entrada.h         leitura segura do teclado
  repositorio.h     o estado do mini-git: staging + stash + histórico
  staging.h         comandos da fila
  historico.h       comandos da lista
  stash.h           comandos da pilha
  menu.h            laço do menu
src/                implementação de cada cabeçalho, com o mesmo nome
  main.c            só cria o repositório e chama o menu
```

### Ortogonalidade: cada módulo muda sozinho

O código está em três camadas, e cada camada só conhece a de baixo:

| Camada | Módulos | Sabe de | Não sabe de |
|---|---|---|---|
| Estruturas de dados | `fila`, `pilha`, `lista` | só o seu `typedef` de item | `printf`, teclado, as outras estruturas |
| Comandos do mini-git | `staging`, `historico`, `stash` | as funções das estruturas, `entrada` | os campos internos das estruturas, o menu |
| Interface | `menu`, `main` | os comandos | as estruturas |

Na prática:

- **Nenhum comando lê `dados[]`, `itens[]`, `comeco` ou `topo` direto.** Para percorrer
  sem remover, cada estrutura ganhou uma função de leitura: `queue_get(f, i)` (i-ésimo a
  partir da frente), `stack_get(p, i)` (i-ésimo a partir do topo, como o `stash@{i}`) e
  `lst_get(l, k)`. Assim, trocar a fila circular por uma fila encadeada não muda nenhum
  comando.
- **As estruturas não sabem o que guardam.** A fila usa `queue_info`, a pilha
  `stack_info` e a lista `lst_info`; só o `typedef` liga cada uma ao tipo do mini-git.
- **O menu é uma tabela.** Cada linha de `OPCOES[]` em `src/menu.c` tem a seção, o
  texto e a função. Criar um comando novo é escrever a função e acrescentar uma linha; o
  número da opção sai da posição na tabela.
- **A leitura do teclado fica num lugar só** (`entrada.c`). Antes, cada entrega tinha a
  sua cópia de `ler_texto` e `ler_numero`.

As integrações entre estruturas ficam nos comandos e usam só as funções públicas:

| Comando | Movimento | Funções usadas |
|---|---|---|
| commit | fila → lista | `dequeue` até esvaziar, `lst_inserir` no fim |
| stash | fila → pilha | `dequeue` até esvaziar, `push` |
| stash pop | pilha → fila | `top` (confere se cabe), `pop`, `enqueue` |

## Menu

```
=== MINI-GIT ===

-- Staging (fila) --
 1 - Adicionar arquivo (add / enqueue)
 2 - Ver o proximo arquivo (front)
 3 - Status do staging
 4 - Tirar o primeiro arquivo do staging (dequeue)

-- Historico (lista) --
 5 - Fazer commit (staging -> historico)
 6 - Ver historico (log)
 7 - Buscar commit pelo id
 8 - Remover commit pelo id

-- Stash (pilha) --
 9 - Guardar staging no stash (push)
10 - Recuperar o ultimo stash (pop -> staging)
11 - Ver o stash do topo (top)
12 - Listar stashes
13 - Descartar o ultimo stash (drop)

 0 - Sair
```

| Opção | Funções das estruturas |
|---|---|
| 1 | `queue_isfull`, `queue_get` (nome repetido), `enqueue` |
| 2 | `front` |
| 3 | `queue_isempty`, `queue_len`, `queue_get` |
| 4 | `dequeue` |
| 5 | `queue_isempty`, `lst_isfull`, `dequeue`, `lst_inserir` |
| 6 | `lst_len`, `lst_get` |
| 7 | `lst_localizar`, `lst_get` |
| 8 | `lst_localizar`, `lst_remover` |
| 9 | `queue_isempty`, `stack_isfull`, `dequeue`, `push` |
| 10 | `top`, `queue_len`, `pop`, `enqueue` |
| 11 | `top` |
| 12 | `stack_isempty`, `stack_len`, `stack_get` |
| 13 | `pop` |
| início | `queue_init`, `stack_init`, `lst_init` (em `repo_init`) |

`lst_concatenar` está implementada e testada, mas não aparece no menu: ela será usada
para juntar históricos (merge) na Entrega Final.

## Limites

| Estrutura | Constante | Máximo |
|---|---|---|
| Staging (fila) | `MAX_QUEUE` = `MAX_STAGING` | 5 arquivos |
| Stash (pilha) | `STACK_MAX` | 5 stashes |
| Histórico (lista) | `LST_MAX` | 10 commits |

Cada commit e cada stash guardam até `MAX_STAGING` arquivos, porque recebem o staging
inteiro.

## Casos especiais

| Caso | O que acontece |
|---|---|
| Staging vazio | Commit, stash, ver o próximo, status ou tirar o primeiro mostram um aviso |
| Staging cheio | O 6º arquivo é recusado: "staging cheio (maximo de 5)" |
| Arquivo repetido | Recusado no `add`; no `stash pop`, o arquivo que já está no staging não é duplicado |
| `stash pop` sem espaço | O stash **continua na pilha** e o staging não muda: primeiro o `top` confere se cabe, só depois o `pop` tira |
| Histórico cheio | O 11º commit é recusado, e o staging fica intacto |
| Stash cheio | O 6º stash é recusado, e o staging fica intacto |
| Valor inválido | Letra no lugar de número, opção ou id que não existem, nome vazio ou com espaço, mensagem, autor ou descrição vazios: o programa avisa e volta ao menu |
| Fim da entrada (Ctrl+D) | O programa sai normalmente |

## Exemplo de uso

```
Opcao: 1
Nome do arquivo: login.c
login.c entrou no fim do staging.

Opcao: 1
Nome do arquivo: login.h
login.h entrou no fim do staging.

Opcao: 9
Descricao do trabalho: login pela metade
Stash 1 guardado no topo com 2 arquivo(s). Staging vazio.

Opcao: 1
Nome do arquivo: diff.c
diff.c entrou no fim do staging.

Opcao: 10
Stash 1 recuperado: "login pela metade"
  login.c voltou para o staging.
  login.h voltou para o staging.

Opcao: 3
Staging (3 de 5), da frente para o fim:
  1. diff.c (add 3)
  2. login.c (add 1)
  3. login.h (add 2)

Opcao: 5
Mensagem do commit: tela de login
Autor: Nicolas
Commit 1 criado com 3 arquivo(s):
  commit 1 - tela de login (autor: Nicolas)
      diff.c (add 3)
      login.c (add 1)
      login.h (add 2)
Staging vazio.

Opcao: 6
Historico (1 de 10 commits), do mais antigo ao mais novo:
  commit 1 - tela de login (autor: Nicolas)
      diff.c (add 3)
      login.c (add 1)
      login.h (add 2)
```

(O menu aparece antes de cada "Opcao:"; aqui ele foi omitido.)

Os arquivos que voltaram do stash entram no **fim** da fila, atrás do `diff.c`, e o
commit os tira na ordem da fila: FIFO no staging, LIFO no stash e ordem cronológica no
histórico.

## O que mudou em relação às entregas separadas

1. Pastas `include/` (cabeçalhos) e `src/` (implementação); o `Makefile` compila cada
   `.c` em `build/` e recompila só o que mudou.
2. Os tipos do domínio (`Arquivo`, `Stash`, `Commit`) foram para `tipos.h`. O `Stash`
   deixou de guardar os arquivos como texto (`char arquivos[60]`) e passou a guardar o
   vetor de `Arquivo` do staging, para o `stash pop` poder devolvê-los à fila. O `Commit`
   ganhou o mesmo vetor, preenchido pelo staging.
3. A lista passou a usar `lst_info` no lugar de `Commit` direto, igual à fila e à pilha,
   e ganhou `lst_init`, `lst_isfull` e `lst_get`.
4. A fila ganhou `queue_len` e `queue_get`; a pilha ganhou `stack_len` e `stack_get`.
5. `lst_remover` também recusa `val` nulo, como `pop` e `dequeue` já faziam.
6. Comandos novos que só existem com as estruturas juntas: commit a partir do staging,
   `stash` do staging, `stash pop` de volta ao staging e `stash drop`.
7. O histórico git das três entregas foi preservado no repositório (ver abaixo).

## Histórico das entregas anteriores

Os commits de `MiniGitPilha` e `MiniGitEntrega2` foram trazidos para este repositório
em `refs/historico/pilha` e `refs/historico/lista`:

```
git log --oneline refs/historico/pilha
git log --oneline refs/historico/lista
```
