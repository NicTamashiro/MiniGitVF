#include <string.h>
#include "minigit.h"

#define TEXTO_(x) #x
#define TEXTO(x)  TEXTO_(x)

const char *mg_mensagem(mg_status s){
    switch(s){
    case MG_OK:                 return "ok";
    case MG_STAGING_VAZIO:      return "staging vazio";
    case MG_STAGING_CHEIO:      return "staging cheio (maximo de " TEXTO(MAX_QUEUE) " arquivos)";
    case MG_NOME_INVALIDO:      return "nome invalido (vazio, com espaco ou longo demais)";
    case MG_JA_NO_STAGING:      return "esse arquivo ja esta no staging";
    case MG_TEXTO_VAZIO:        return "o texto nao pode ficar vazio";
    case MG_HISTORICO_VAZIO:    return "nao ha commits no historico";
    case MG_HISTORICO_CHEIO:    return "historico cheio (maximo de " TEXTO(LST_MAX) " commits)";
    case MG_COMMIT_INEXISTENTE: return "esse commit nao existe";
    case MG_STASH_VAZIO:        return "stash vazio";
    case MG_STASH_CHEIO:        return "stash cheio (maximo de " TEXTO(STACK_MAX) ")";
    case MG_SEM_ESPACO:         return "o staging nao tem espaco para os arquivos do stash";
    }
    return "erro desconhecido";
}

static bool vazio(const char *s){
    return s == NULL || s[0] == '\0';
}

/* Copia texto para um campo de tamanho fixo; falha se nao couber. */
static bool copiar(char *destino, size_t tamanho, const char *origem){
    if(strlen(origem) >= tamanho)
        return false;
    strcpy(destino, origem);
    return true;
}

/* ---------- pre-condicoes ---------- */

mg_status mg_pode_add(const Repositorio *r){
    return queue_isfull(r->staging) ? MG_STAGING_CHEIO : MG_OK;
}

mg_status mg_pode_commit(const Repositorio *r){
    if(queue_isempty(r->staging))  return MG_STAGING_VAZIO;
    if(lst_isfull(r->historico))   return MG_HISTORICO_CHEIO;
    return MG_OK;
}

mg_status mg_pode_stash(const Repositorio *r){
    if(queue_isempty(r->staging))  return MG_STAGING_VAZIO;
    if(stack_isfull(&r->stash))     return MG_STASH_CHEIO;
    return MG_OK;
}

bool mg_staging_contem(const Repositorio *r, const char *nome){
    Arquivo a;
    for(int i = 0; queue_get(r->staging, i, &a); i++)
        if(strcmp(a.nome, nome) == 0)
            return true;
    return false;
}

/* ---------- staging (fila) ---------- */

mg_status mg_add(Repositorio *r, const char *nome){
    Arquivo a;
    mg_status s = mg_pode_add(r);
    if(s != MG_OK) return s;

    if(vazio(nome) || strchr(nome, ' ') != NULL || !copiar(a.nome, sizeof(a.nome), nome))
        return MG_NOME_INVALIDO;
    if(mg_staging_contem(r, nome))
        return MG_JA_NO_STAGING;

    a.ordem = r->proximo_add++;
    enqueue(&r->staging, a);
    return MG_OK;
}

mg_status mg_unstage_primeiro(Repositorio *r, Arquivo *removido){
    Arquivo a;
    if(!dequeue(&r->staging, &a))
        return MG_STAGING_VAZIO;
    if(removido) *removido = a;
    return MG_OK;
}

/* ---------- historico (lista) ---------- */

mg_status mg_commit(Repositorio *r, const char *mensagem, const char *autor, Commit *criado){
    Commit c;
    mg_status s = mg_pode_commit(r);
    if(s != MG_OK) return s;

    if(vazio(mensagem) || vazio(autor))
        return MG_TEXTO_VAZIO;
    if(!copiar(c.mensagem, sizeof(c.mensagem), mensagem) || !copiar(c.autor, sizeof(c.autor), autor))
        return MG_NOME_INVALIDO;

    /* fila -> commit: os arquivos saem na ordem em que entraram (FIFO) */
    c.qtd_arquivos = 0;
    while(dequeue(&r->staging, &c.arquivos[c.qtd_arquivos]))
        c.qtd_arquivos++;

    c.id = r->proximo_commit++;
    lst_inserir(&r->historico, c, lst_len(r->historico));     /* entra no fim */
    if(criado) *criado = c;
    return MG_OK;
}

mg_status mg_remover_commit(Repositorio *r, int id, Commit *removido){
    Commit c;
    int pos;
    if(lst_len(r->historico) == 0)
        return MG_HISTORICO_VAZIO;
    pos = lst_localizar(r->historico, id);
    if(pos == -1 || !lst_remover(&r->historico, pos, &c))
        return MG_COMMIT_INEXISTENTE;
    if(removido) *removido = c;
    return MG_OK;
}

/* ---------- stash (pilha) ---------- */

mg_status mg_stash(Repositorio *r, const char *descricao, Stash *criado){
    Stash s;
    mg_status st = mg_pode_stash(r);
    if(st != MG_OK) return st;

    if(vazio(descricao))
        return MG_TEXTO_VAZIO;
    if(!copiar(s.descricao, sizeof(s.descricao), descricao))
        return MG_NOME_INVALIDO;

    /* fila -> pilha: o staging inteiro vai para o topo */
    s.qtd_arquivos = 0;
    while(dequeue(&r->staging, &s.arquivos[s.qtd_arquivos]))
        s.qtd_arquivos++;

    s.id = r->proximo_stash++;
    push(&r->stash, s);
    if(criado) *criado = s;
    return MG_OK;
}

mg_status mg_stash_pop(Repositorio *r, Stash *recuperado, int *devolvidos){
    Stash s;
    int novos = 0;

    /* Olha o topo SEM tirar: se nao couber, o stash continua guardado. */
    if(!top(&r->stash, &s))
        return MG_STASH_VAZIO;
    for(int i = 0; i < s.qtd_arquivos; i++)
        if(!mg_staging_contem(r, s.arquivos[i].nome))
            novos++;
    if(queue_len(r->staging) + novos > MAX_QUEUE)
        return MG_SEM_ESPACO;

    /* pilha -> fila: os arquivos voltam para o fim do staging, sem repetir */
    pop(&r->stash, &s);
    for(int i = 0; i < s.qtd_arquivos; i++)
        if(!mg_staging_contem(r, s.arquivos[i].nome))
            enqueue(&r->staging, s.arquivos[i]);

    if(recuperado) *recuperado = s;
    if(devolvidos) *devolvidos = novos;
    return MG_OK;
}

mg_status mg_stash_drop(Repositorio *r, Stash *descartado){
    Stash s;
    if(!pop(&r->stash, &s))
        return MG_STASH_VAZIO;
    if(descartado) *descartado = s;
    return MG_OK;
}
