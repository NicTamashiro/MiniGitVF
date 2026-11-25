#include "repositorio.h"
#include "menu.h"

int main(void){
    Repositorio repo;
    repo_init(&repo);
    menu_executar(&repo);
    return 0;
}
