#include <curses.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "paineis.h"
#include "teclas.h"

enum { COR_ATIVO = 1, COR_TITULO, COR_ERRO, COR_OK, COR_TECLA, COR_APAGADO };

void paineis_iniciar_cores(void){
    if(!has_colors()) return;
    start_color();
    use_default_colors();
    init_pair(COR_ATIVO,   COLOR_GREEN,  -1);
    init_pair(COR_TITULO,  COLOR_CYAN,   -1);
    init_pair(COR_ERRO,    COLOR_RED,    -1);
    init_pair(COR_OK,      COLOR_GREEN,  -1);
    init_pair(COR_TECLA,   COLOR_YELLOW, -1);
    init_pair(COR_APAGADO, COLOR_BLUE,   -1);
}

/* ---------- utilitarios de desenho ---------- */

typedef struct { int y, x, alt, larg; } Area;   /* area INTERNA de uma caixa */

static Area caixa(int y, int x, int alt, int larg, const char *titulo, bool ativo){
    attr_t estilo = ativo ? (COLOR_PAIR(COR_ATIVO) | A_BOLD) : A_NORMAL;
    attron(estilo);
    mvhline(y, x + 1, ACS_HLINE, larg - 2);
    mvhline(y + alt - 1, x + 1, ACS_HLINE, larg - 2);
    mvvline(y + 1, x, ACS_VLINE, alt - 2);
    mvvline(y + 1, x + larg - 1, ACS_VLINE, alt - 2);
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + larg - 1, ACS_URCORNER);
    mvaddch(y + alt - 1, x, ACS_LLCORNER);
    mvaddch(y + alt - 1, x + larg - 1, ACS_LRCORNER);
    mvprintw(y, x + 1, "%s", titulo);
    attroff(estilo);
    return (Area){ y + 1, x + 1, alt - 2, larg - 2 };
}

/* Escreve uma linha formatada dentro da area, cortando no limite. */
static void linha(Area a, int *y, attr_t estilo, const char *formato, ...){
    char buf[256];
    va_list args;
    if(*y >= a.y + a.alt) return;
    va_start(args, formato);
    vsnprintf(buf, sizeof(buf), formato, args);
    va_end(args);
    attron(estilo);
    mvaddnstr(*y, a.x, buf, a.larg);
    attroff(estilo);
    (*y)++;
}

/* Como linha(), mas quebra o texto entre palavras para caber na largura. */
static void paragrafo(Area a, int *y, attr_t estilo, const char *formato, ...){
    char buf[256];
    va_list args;
    va_start(args, formato);
    vsnprintf(buf, sizeof(buf), formato, args);
    va_end(args);

    const char *p = buf;
    while(*p != '\0' && *y < a.y + a.alt){
        int n = (int)strlen(p);
        if(n > a.larg){                         /* procura o ultimo espaco que cabe */
            n = a.larg;
            while(n > 0 && p[n] != ' ') n--;
            if(n == 0) n = a.larg;              /* palavra maior que a linha */
        }
        attron(estilo);
        mvaddnstr(*y, a.x, p, n);
        attroff(estilo);
        (*y)++;
        p += n;
        while(*p == ' ') p++;
    }
}

/* ---------- paineis da esquerda ---------- */

static void texto_item(const Tui *t, Painel p, int i, char *buf, int tam){
    Arquivo a; Commit c; Stash s;
    switch(p){
    case P_STAGING:
        if(queue_get(t->repo->staging, i, &a))
            snprintf(buf, tam, "%s %s", i == 0 ? ">" : " ", a.nome);
        break;
    case P_COMMITS:
        if(estado_commit_na_linha(t, i, &c))
            snprintf(buf, tam, "%3d %s", c.id, c.mensagem);
        break;
    case P_STASH:
        if(stack_get(&t->repo->stash, i, &s))
            snprintf(buf, tam, "stash@{%d} %s", i, s.descricao);
        break;
    default:
        buf[0] = '\0';
    }
}

static void painel_lista(const Tui *t, Painel p, int y, int x, int alt, int larg, const char *titulo){
    bool ativo = t->ativo == p;
    Area a = caixa(y, x, alt, larg, titulo, ativo);
    int qtd = estado_qtd_itens(t, p);
    int cursor = t->cursor[p];
    int primeiro = cursor >= a.alt ? cursor - a.alt + 1 : 0;     /* rolagem */
    char buf[128];

    if(qtd == 0){
        int yy = a.y;
        linha(a, &yy, COLOR_PAIR(COR_APAGADO), "(vazio)");
        return;
    }
    for(int i = primeiro; i < qtd && i - primeiro < a.alt; i++){
        texto_item(t, p, i, buf, sizeof(buf));
        attr_t estilo = (ativo && i == cursor) ? A_REVERSE : A_NORMAL;
        attron(estilo);
        mvhline(a.y + i - primeiro, a.x, ' ', a.larg);
        mvaddnstr(a.y + i - primeiro, a.x, buf, a.larg);
        attroff(estilo);
    }
}

static void painel_status(const Tui *t, int y, int x, int larg){
    Area a = caixa(y, x, 4, larg, "[1] Status", t->ativo == P_STATUS);
    int yy = a.y;
    linha(a, &yy, A_BOLD, "mini-git  commits %d/%d", lst_len(t->repo->historico), LST_MAX);
    linha(a, &yy, A_NORMAL, "staging %d/%d  stash %d/%d",
          queue_len(t->repo->staging), MAX_QUEUE, stack_len(&t->repo->stash), STACK_MAX);
}

/* ---------- painel da direita (detalhes do item selecionado) ---------- */

static void detalhe_arquivos(Area a, int *y, const Arquivo *arq, int qtd){
    for(int i = 0; i < qtd; i++)
        linha(a, y, A_NORMAL, "  %d. %s (add %d)", i + 1, arq[i].nome, arq[i].ordem);
}

static void detalhe_status(Area a, int *y){
    linha(a, y, COLOR_PAIR(COR_TITULO) | A_BOLD, "Fluxo do mini-git");
    linha(a, y, A_NORMAL, "");
    linha(a, y, A_NORMAL, "add        arquivo -> staging (fila)");
    linha(a, y, A_NORMAL, "commit     staging -> historico (lista)");
    linha(a, y, A_NORMAL, "stash      staging -> stash (pilha)");
    linha(a, y, A_NORMAL, "stash pop  stash -> staging");
    linha(a, y, A_NORMAL, "");
    paragrafo(a, y, A_NORMAL, "Troque de painel com 1-4 ou Tab. Aperte ? para ver todas as teclas.");
}

static void detalhe_staging(const Tui *t, Area a, int *y){
    Arquivo sel, frente;
    int qtd = queue_len(t->repo->staging);
    if(!queue_get(t->repo->staging, t->cursor[P_STAGING], &sel)){
        paragrafo(a, y, A_NORMAL, "Staging vazio. Aperte 'a' para adicionar um arquivo.");
        return;
    }
    front(t->repo->staging, &frente);
    linha(a, y, COLOR_PAIR(COR_TITULO) | A_BOLD, "%s", sel.nome);
    linha(a, y, A_NORMAL, "");
    linha(a, y, A_NORMAL, "Numero do add:    %d", sel.ordem);
    linha(a, y, A_NORMAL, "Posicao na fila:  %d de %d", t->cursor[P_STAGING] + 1, qtd);
    linha(a, y, A_NORMAL, "");
    paragrafo(a, y, COLOR_PAIR(COR_APAGADO), "Fila (FIFO): o commit leva os arquivos da frente para o fim.");
    paragrafo(a, y, COLOR_PAIR(COR_APAGADO), "So a frente sai: 'u' tira %s.", frente.nome);
}

static void detalhe_commit(const Tui *t, Area a, int *y){
    Commit c;
    if(!estado_commit_na_linha(t, t->cursor[P_COMMITS], &c)){
        paragrafo(a, y, A_NORMAL, "Nenhum commit ainda. Adicione arquivos e aperte 'c'.");
        return;
    }
    linha(a, y, COLOR_PAIR(COR_TITULO) | A_BOLD, "commit %d", c.id);
    linha(a, y, A_NORMAL, "Autor:     %s", c.autor);
    linha(a, y, A_NORMAL, "Mensagem:  %s", c.mensagem);
    linha(a, y, A_NORMAL, "");
    linha(a, y, A_BOLD, "Arquivos (na ordem em que sairam do staging):");
    detalhe_arquivos(a, y, c.arquivos, c.qtd_arquivos);
    linha(a, y, A_NORMAL, "");
    paragrafo(a, y, COLOR_PAIR(COR_APAGADO), "Lista: 'd' remove o commit selecionado, de qualquer posicao.");
}

static void detalhe_stash(const Tui *t, Area a, int *y){
    Stash s;
    int i = t->cursor[P_STASH];
    if(!stack_get(&t->repo->stash, i, &s)){
        paragrafo(a, y, A_NORMAL, "Nenhum stash. No painel Staging, aperte 's' para guardar o staging.");
        return;
    }
    linha(a, y, COLOR_PAIR(COR_TITULO) | A_BOLD, "stash@{%d}  (stash %d)", i, s.id);
    linha(a, y, A_NORMAL, "Descricao: %s", s.descricao);
    linha(a, y, A_NORMAL, "");
    linha(a, y, A_BOLD, "Arquivos guardados:");
    detalhe_arquivos(a, y, s.arquivos, s.qtd_arquivos);
    linha(a, y, A_NORMAL, "");
    if(i == 0)
        paragrafo(a, y, COLOR_PAIR(COR_APAGADO), "Este e o topo: 'g' devolve ao staging, 'd' descarta.");
    else
        paragrafo(a, y, COLOR_PAIR(COR_APAGADO), "Pilha (LIFO): so o topo sai. 'g' e 'd' agem em stash@{0}.");
}

static void painel_detalhes(const Tui *t, int y, int x, int alt, int larg){
    Area a = caixa(y, x, alt, larg, "Detalhes", false);
    int yy = a.y;
    switch(t->ativo){
    case P_STATUS:  detalhe_status(a, &yy);     break;
    case P_STAGING: detalhe_staging(t, a, &yy); break;
    case P_COMMITS: detalhe_commit(t, a, &yy);  break;
    case P_STASH:   detalhe_stash(t, a, &yy);   break;
    default: break;
    }
}

/* ---------- rodape: aviso e barra de teclas ---------- */

static void barra_teclas(const Tui *t, int y){
    int x = 1;
    move(y, 0);
    clrtoeol();
    /* primeiro as teclas do painel, depois as globais */
    for(int passo = 0; passo < 2; passo++){
        for(int i = 0; i < teclas_qtd(); i++){
            const Tecla *k = teclas_item(i);
            bool do_painel = k->painel == (int)t->ativo;
            if(!k->na_barra || !teclas_vale_em(k, t->ativo) || do_painel != (passo == 0))
                continue;
            int tam = (int)(strlen(k->rotulo) + 2 + strlen(k->descricao));
            if(x + tam >= COLS) return;             /* so mostra atalhos inteiros */
            attron(COLOR_PAIR(COR_TECLA) | A_BOLD);
            mvprintw(y, x, "%s", k->rotulo);
            attroff(COLOR_PAIR(COR_TECLA) | A_BOLD);
            printw(": %s", k->descricao);
            x += tam + 3;
        }
    }
}

static void janela_ajuda(const Tui *t){
    int alt = 0;
    for(int i = 0; i < teclas_qtd(); i++)
        if(teclas_vale_em(teclas_item(i), t->ativo) && !teclas_encoberta(teclas_item(i), t->ativo)) alt++;
    alt += 4;
    if(alt > LINES - 2) alt = LINES - 2;
    int larg = 46, y0 = (LINES - alt) / 2, x0 = (COLS - larg) / 2;

    for(int yy = y0; yy < y0 + alt; yy++) mvhline(yy, x0, ' ', larg);
    Area a = caixa(y0, x0, alt, larg, " Teclas deste painel ", true);
    int yy = a.y;
    for(int i = 0; i < teclas_qtd(); i++){
        const Tecla *k = teclas_item(i);
        if(teclas_vale_em(k, t->ativo) && !teclas_encoberta(k, t->ativo))
            linha(a, &yy, A_NORMAL, " %-6s %s", k->rotulo, k->descricao);
    }
    yy = a.y + a.alt - 1;
    linha(a, &yy, COLOR_PAIR(COR_APAGADO), " qualquer tecla fecha");
}

void paineis_desenhar(const Tui *t){
    erase();
    if(COLS < TUI_MIN_COLUNAS || LINES < TUI_MIN_LINHAS){
        mvprintw(0, 0, "Terminal pequeno demais: minimo %dx%d (agora %dx%d).",
                 TUI_MIN_COLUNAS, TUI_MIN_LINHAS, COLS, LINES);
        refresh();
        return;
    }

    int area = LINES - 2;                          /* 2 linhas de rodape */
    int esq = COLS / 3 < 30 ? 30 : (COLS / 3 > 44 ? 44 : COLS / 3);
    int resto = area - 4;                          /* tira o painel Status */
    int h = resto / 3;

    painel_status(t, 0, 0, esq);
    painel_lista(t, P_STAGING, 4,         0, h,             esq, "[2] Staging (fila)");
    painel_lista(t, P_COMMITS, 4 + h,     0, resto - 2 * h, esq, "[3] Commits (lista)");
    painel_lista(t, P_STASH,   4 + resto - h, 0, h,         esq, "[4] Stash (pilha)");
    painel_detalhes(t, 0, esq, area, COLS - esq);

    if(t->aviso[0] != '\0'){
        attr_t estilo = COLOR_PAIR(t->aviso_erro ? COR_ERRO : COR_OK) | A_BOLD;
        attron(estilo);
        mvaddnstr(LINES - 2, 1, t->aviso, COLS - 2);
        attroff(estilo);
    }
    barra_teclas(t, LINES - 1);

    if(t->ajuda) janela_ajuda(t);
    refresh();
}
