#include <curses.h>
#include <locale.h>
#include "tui.h"
#include "estado.h"
#include "paineis.h"
#include "teclas.h"

int tui_executar(Repositorio *r){
    Tui t;

    setlocale(LC_ALL, "");          /* bordas e acentos do terminal em UTF-8 */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
#ifdef NCURSES_VERSION
    set_escdelay(25);               /* Esc fecha o popup na hora */
#endif
    paineis_iniciar_cores();

    estado_iniciar(&t, r);
    estado_avisar(&t, false, "Bem-vindo! Aperte 'a' para adicionar um arquivo ou '?' para ver as teclas.");

    while(!t.sair){
        paineis_desenhar(&t);
        int tecla = getch();
        if(tecla == KEY_RESIZE || tecla == ERR) continue;
        if(t.ajuda){                /* qualquer tecla fecha a ajuda */
            t.ajuda = false;
            continue;
        }
        if(!teclas_despachar(&t, tecla))
            estado_avisar(&t, true, "Essa tecla nao faz nada neste painel. Aperte ? para ver as teclas.");
    }

    endwin();
    return 0;
}
