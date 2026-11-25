#include <curses.h>
#include <string.h>
#include "popup.h"

#define TECLA_ESC 27

static WINDOW *abrir(int altura, int largura, const char *titulo){
    if(largura > COLS - 4) largura = COLS - 4;
    WINDOW *w = newwin(altura, largura, (LINES - altura) / 2, (COLS - largura) / 2);
    keypad(w, TRUE);
    werase(w);
    box(w, 0, 0);
    wattron(w, A_BOLD);
    mvwprintw(w, 0, 2, " %s ", titulo);
    wattroff(w, A_BOLD);
    return w;
}

bool popup_ler(const char *titulo, const char *rotulo, char *texto, int tamanho){
    WINDOW *w = abrir(5, 56, titulo);
    int largura = getmaxx(w);
    int x0 = 2 + (int)strlen(rotulo) + 1;
    int visivel = largura - x0 - 2;
    int len = 0;
    bool ok = false;

    texto[0] = '\0';
    curs_set(1);
    for(;;){
        int inicio = len > visivel ? len - visivel : 0;    /* rola o texto longo */
        mvwprintw(w, 2, 2, "%s", rotulo);
        mvwhline(w, 2, x0, ' ', visivel);
        mvwaddnstr(w, 2, x0, texto + inicio, visivel);
        mvwprintw(w, 4, 2, " Enter: confirmar   Esc: cancelar ");
        wmove(w, 2, x0 + len - inicio);
        wrefresh(w);

        int ch = wgetch(w);
        if(ch == TECLA_ESC)                                   { ok = false; break; }
        if(ch == '\n' || ch == '\r' || ch == KEY_ENTER)       { ok = true;  break; }
        if(ch == KEY_BACKSPACE || ch == 127 || ch == 8){
            if(len > 0) texto[--len] = '\0';
        } else if(ch >= 32 && ch < 127 && len < tamanho - 1){ /* so ASCII visivel */
            texto[len++] = (char)ch;
            texto[len] = '\0';
        }
    }
    curs_set(0);
    delwin(w);
    return ok;
}

bool popup_confirmar(const char *titulo, const char *pergunta){
    WINDOW *w = abrir(5, (int)strlen(pergunta) + 6 > 40 ? (int)strlen(pergunta) + 6 : 40, titulo);
    mvwaddnstr(w, 2, 2, pergunta, getmaxx(w) - 4);
    mvwprintw(w, 4, 2, " s: sim   n/Esc: nao ");
    wrefresh(w);
    int ch = wgetch(w);
    delwin(w);
    return ch == 's' || ch == 'S' || ch == 'y' || ch == 'Y';
}
