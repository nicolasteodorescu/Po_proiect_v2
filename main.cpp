#include "MainMenu.h"

int main(){
    Analysis& engine = Analysis::getGlobalInstance();
    MainMenu menu(engine);
    menu.run();
    return 0;
}
