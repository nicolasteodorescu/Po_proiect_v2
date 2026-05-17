#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <limits>

#include "Analysis.h"
#include "Entity.h"
#include "Team.h"
#include "LoadChamp.h"

static std::shared_ptr<Champion> findChamp(Analysis& engine, const std::string& name) {
    return engine.findChampion(name);
}

static void demoExceptions(Analysis& engine) {
    std::cout << "\nException Propagation Demo\n";

    try {
        auto c = findChamp(engine, "NonExistentChampion");
        (void)c;
    } catch (const ChampionNotFoundException& e) {
        std::cout << "[ChampionNotFoundException caught] " << e.what() << "\n";
    } catch (const GameException& e) {
        std::cout << "[GameException caught via upcast] " << e.what() << "\n";
    }

    try {
        Champion bad("TestChamp", Champion::MID, 1.5);
        (void)bad;
    } catch (const InvalidWinRateException& e) {
        std::cout << "[InvalidWinRateException caught] " << e.what() << "\n";
    } catch (const GameException& e) {
        std::cout << "[GameException caught via upcast] " << e.what() << "\n";
    }

    try {
        auto team = std::make_shared<Team>("TestTeam");
        auto dummy = std::make_shared<Champion>("D1", Champion::TOP,    0.50);
        auto d2 = std::make_shared<Champion>("D2", Champion::JUNGLE, 0.50);
        auto d3 = std::make_shared<Champion>("D3", Champion::MID,    0.50);
        auto d4 = std::make_shared<Champion>("D4", Champion::ADC,    0.50);
        auto d5 = std::make_shared<Champion>("D5", Champion::SUPPORT,0.50);
        auto d6 = std::make_shared<Champion>("D6", Champion::TOP,    0.50);
        team->addChampion(dummy);
        team->addChampion(d2);
        team->addChampion(d3);
        team->addChampion(d4);
        team->addChampion(d5);
        team->addChampion(d6);
    } catch (const TeamFullException& e) {
        std::cout << "[TeamFullException caught] " << e.what() << "\n";
    } catch (const GameException& e) {
        std::cout << "[GameException caught via upcast] " << e.what() << "\n";
    }
}

static void demoItems(Analysis& engine) {
    std::cout << "\nItem Hierarchy Demo\n";

    auto trinity = std::make_shared<MythicItem>(
        "Trinity Force", 3333,
        30, 0, 200, 0, 0, 20, 0, 0,
        0, 0, 0.35, 0,
        0.02,
        "+3 AD per legendary", 3,
        "AD bruiser mythic");

    auto longsword = std::make_shared<ComponentItem>(
        "Long Sword", 350,
        10, 0, 0,
        "Various full items",
        "Basic AD component");

    engine.registerItem(trinity);
    engine.registerItem(longsword);

    std::vector<std::shared_ptr<Item>> itemList = { trinity, longsword };
    for (const auto& itemPtr : itemList) {
        itemPtr->display();

        if (const MythicItem* mi = dynamic_cast<const MythicItem*>(itemPtr.get())) {
            std::cout << "   -> Downcast to MythicItem: passive=\"" << mi->getMythicPassive() << "\"\n";
        }
        if (const ComponentItem* ci = dynamic_cast<const ComponentItem*>(itemPtr.get())) {
            std::cout << "   -> Downcast to ComponentItem: builds into \"" << ci->getBuildsInto() << "\"\n";
        }
    }

    int diff = *trinity - *longsword;
    std::cout << "Trinity Force costs " << diff << "g more than Long Sword\n";
}

static void demoChampionOps(Analysis& engine) {
    std::cout << "\nChampion Operator Demo\n";
    try {
        auto ahri   = engine.findChampion("Ahri");
        auto lissandra = engine.findChampion("Lissandra");
        Champion combo = *ahri + *lissandra;
        std::cout << "Combined champion: ";
        combo.display();

        std::cout << "Stream output: " << combo << "\n";

        std::cout << "Ahri < Lissandra (by WR)? " << (*ahri < *lissandra ? "yes" : "no") << "\n";
        std::cout << "Ahri == Ahri? " << (*ahri == *ahri ? "yes" : "no") << "\n";
        std::cout << "Top champion ever (first WR>55%): "
                  << Champion::getTopChampionName() << "\n";
        std::cout << "Live Entity instances: "
                  << Entity::getLiveInstances() << "\n";
    } catch (const GameException& e) {
        std::cout << "[Error] " << e.what() << "\n";
    }
}

static void menuTierList(Analysis& engine) {
    engine.printTierList();
}

static void menuRecommend(Analysis& engine) {
    int r;

    std::cout << "Alege rolul (0=Top 1=Jungle 2=Mid 3=ADC 4=Support): ";
    std::cin >> r;
    if (r < 0 || r > 4) {
        std::cout << "Rol invalid.\n";
        return; }
    auto recs = engine.recommendForRole(static_cast<Champion::Role>(r), 5);
    std::cout << "Top 5 " << Champion::roleToString(static_cast<Champion::Role>(r)) << ":\n";
    for (size_t i = 0; i < recs.size(); ++i) {
        std::cout << "  " << (i+1) << ". ";
        recs[i]->display();
    }
}

static void menuDraftAnalysis(Analysis& engine) {
    auto blue = std::make_shared<DraftTeam>("Blue Team", false);
    auto red  = std::make_shared<DraftTeam>("Red Team",  true);

    auto pick = [&](std::shared_ptr<DraftTeam>& team, const std::string& side) {
        std::cout << "\n" << side << " - introdu 5 bans:\n";
        for (int i = 0; i < 5; i++) {
            std::string name; std::cin >> name;
            team->banChampion(name);
        }
        std::cout << side << " - introdu 5 picks:\n";
        for (int i = 0; i < 5; i++) {
            std::string name; std::cin >> name;
            try {
                team->addChampion(engine.findChampion(name));
            } catch (const ChampionNotFoundException& e) {
                std::cout << "[Avertisment] " << e.what() << " - sarit.\n";
                i--;
            } catch (const TeamFullException& e) {
                std::cout << "[Eroare] " << e.what() << "\n";
                break;
            }
        }
    };

    pick(blue, "Blue Team");
    pick(red,  "Red Team");

    double score = engine.analyzeDraft(blue.get(), red.get());
    std::cout << "\nBlue Team advantage score: " << score << "\n";
    if (score > 0)
        std::cout << "  -> Blue Team castiga draftul!\n";
    else if (score < 0)
        std::cout << "  -> Red Team castiga draftul!\n";
    else
        std::cout << "  -> Draft egal!\n";

    blue->display();
    red->display();

    std::vector<std::shared_ptr<Team>> teams = { blue, red };
    LoadChamp::demonstratePolymorphism(teams, engine);
}

static void menuSearchChampion(Analysis& engine) {
    std::cout << "Nume campion: ";
    std::string name; std::cin >> name;
    try {
        auto c = engine.findChampion(name);
        c->display();
    } catch (const ChampionNotFoundException& e) {
        std::cout << "[Nu gasit] " << e.what() << "\n";
    }
}

static void menuAutoDemo(Analysis& engine) {
    std::ifstream citire("tastatura.txt");
    if (!citire.is_open()) {
        std::cout << "tastatura.txt negasit.\n";
        return;
    }

    auto blue = std::make_shared<DraftTeam>("Blue Team", false);
    auto red  = std::make_shared<DraftTeam>("Red Team",  true);

    std::string name;
    for (int i = 0; i < 5; i++) {
        citire >> name;
        blue->banChampion(name);
    }
    for (int i = 0; i < 5; i++) {
        citire >> name;
        red->banChampion(name);
    }

    for (int i = 0; i < 5; i++) {
        citire >> name;
        try { blue->addChampion(engine.findChampion(name)); }
        catch (const GameException& e) {
            std::cout << "[Skip] " << e.what() << "\n";
        }
    }
    for (int i = 0; i < 5; i++) {
        citire >> name;
        try { red->addChampion(engine.findChampion(name)); }
        catch (const GameException& e) {
            std::cout << "[Skip] " << e.what() << "\n";
        }
    }

    double score = engine.analyzeDraft(blue.get(), red.get());
    std::cout << "Blue Team advantage score: " << score << "\n";
    if (score > 0)
        std::cout << "  -> Blue Team castiga draftul!\n";
    else if (score < 0)
        std::cout << "  -> Red Team castiga draftul!\n";
    else
        std::cout << "  -> Draft egal!\n";

    blue->display();
    red->display();
}

int main() {
    Analysis& engine = Analysis::getInstance();

    std::cout << "Se incarca datele campionilor...\n";
    LoadChamp::loadChampionData(engine, "lolalytics_all_172.json");
    std::cout << "Date incarcate. " << engine.getChampionPool().size() << " campioni.\n";

    demoExceptions(engine);
    demoItems(engine);
    demoChampionOps(engine);

    std::cout << "\n--- Demo automat din tastatura.txt ---\n";
    menuAutoDemo(engine);

    int choice = -1;
    do {
        std::cout << "\nMENIU\n";
        std::cout << "1.Tier List\n";
        std::cout << "2.Recomandari pe rol\n";
        std::cout << "3.Analiza draft interactiva\n";
        std::cout << "4.Cauta campion\n";
        std::cout << "5.Log analize\n";
        std::cout << "6.Demo iteme (ierarhie)\n";
        std::cout << "0.Iesire\n";
        std::cout << "Alege: ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
            continue;
        }
        switch (choice) {
            case 1:
                menuTierList(engine);
                break;
            case 2:
                menuRecommend(engine);
                break;
            case 3:
                menuDraftAnalysis(engine);
                break;
            case 4:
                menuSearchChampion(engine);
                break;
            case 5:
                engine.printLog();
                break;
            case 6:
                demoItems(engine);
                break;
            case 0:
                std::cout << "La revedere!\n";
                break;
            default:
                std::cout << "Optiune invalida.\n";
        }
    } while (choice != 0);

    return 0;
}