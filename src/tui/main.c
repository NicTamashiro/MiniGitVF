#include "repositorio.h"
#include "tui.h"

int main(void){
    Repositorio repo;
    repo_init(&repo);
    return tui_executar(&repo);
}
