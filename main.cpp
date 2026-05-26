#include "MainMenu.h"

int main(){
    Analysis& engine = Analysis::getInstance();
    MainMenu menu(engine);
    menu.run();
    return 0;
}
