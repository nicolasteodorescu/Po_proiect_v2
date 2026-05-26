#ifndef OOP_MAINMENU_H
#define OOP_MAINMENU_H

#include "Analysis.h"
#include "Entity.h"
#include "Team.h"
#include "LoadChamp.h"
#include <fstream>
#include <limits>

class MainMenu {
private:
    Analysis& engine;

    void menuTierList() const;
    void menuRecommend() const;
    void menuDraftAnalysis();
    void menuSearchChampion() const;
    void menuLog() const;

    static void printResultDraft( double score );

public:
    explicit MainMenu( Analysis& eng );

    void run();
};

#endif // OOP_MAINMENU_H