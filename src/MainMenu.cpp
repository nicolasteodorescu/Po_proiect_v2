#include "MainMenu.h"

MainMenu::MainMenu( Analysis& eng ) : engine(eng) {}

void MainMenu::printResultDraft( double score ){
    std::cout << "Blue Team advantage score: " << score << "\n";
    if( score > 0 )
        std::cout << "  -> Blue Team castiga draftul!\n";
    else if( score < 0 )
        std::cout << "  -> Red Team castiga draftul!\n";
    else
        std::cout << "  -> Draft egal!\n";
}

void MainMenu::demoExceptions() const {
    try {
        auto c = engine.findChampion("NonExistentChampion");
        (void)c;
    } catch( const ChampionNotFoundException& e ){
        std::cout << "[ChampionNotFoundException] " << e.what() << "\n";
    } catch( const GameException& e ){
        std::cout << "[GameException via upcast] " << e.what() << "\n";
    }

    try {
        Champion bad("TestChamp", Champion::MID, 1.5);
        (void)bad;
    } catch( const InvalidWinRateException& e ){
        std::cout << "[InvalidWinRateException] " << e.what() << "\n";
    } catch( const GameException& e ){
        std::cout << "[GameException via upcast] " << e.what() << "\n";
    }

    try {
        auto team = std::make_shared<Team>("TestTeam");
        auto d1 = std::make_shared<Champion>("D1", Champion::TOP,    0.50);
        auto d2 = std::make_shared<Champion>("D2", Champion::JUNGLE, 0.50);
        auto d3 = std::make_shared<Champion>("D3", Champion::MID,    0.50);
        auto d4 = std::make_shared<Champion>("D4", Champion::ADC,    0.50);
        auto d5 = std::make_shared<Champion>("D5", Champion::SUPPORT, 0.50);
        auto d6 = std::make_shared<Champion>("D6", Champion::TOP,    0.50);
        team->addChampion(d1); team->addChampion(d2); team->addChampion(d3);
        team->addChampion(d4); team->addChampion(d5); team->addChampion(d6);
    } catch( const TeamFullException& e ){
        std::cout << "[TeamFullException] " << e.what() << "\n";
    } catch( const GameException& e ){
        std::cout << "[GameException via upcast] " << e.what() << "\n";
    }
}

void MainMenu::menuTierList() const {
    engine.printTierList();
}

void MainMenu::menuRecommend() const {
    int r = 0;
    std::cout << "Alege rolul (0=Top 1=Jungle 2=Mid 3=ADC 4=Support): ";
    std::cin >> r;
    if( r < 0 || r > 4 ){
        std::cout << "Rol invalid.\n";
        return;
    }

    auto role = static_cast<Champion::Role>(r);
    auto recs = engine.recommendForRole(role, 5);
    std::cout << "Top 5 " << Champion::roleToString(role) << ":\n";
    for( size_t i = 0; i < recs.size(); ++i ){
        std::cout << "  " << (i + 1) << ". ";
        recs[i]->display();
    }
}

void MainMenu::menuDraftAnalysis(){
    auto blue = std::make_shared<DraftTeam>("Blue Team", false);
    auto red  = std::make_shared<DraftTeam>("Red Team",  true);

    auto pick = [&]( const std::shared_ptr<DraftTeam>& team, const std::string& side ){
        std::cout << "\n" << side << " - introdu 5 bans:\n";
        for( int i = 0; i < 5; i++ ){
            std::string name;
            std::cin >> name;
            team->banChampion(name);
        }
        std::cout << side << " - introdu 5 picks:\n";
        for( int i = 0; i < 5; i++ ){
            std::string name;
            std::cin >> name;
            try {
                team->addChampion(engine.findChampion(name));
            } catch( const ChampionNotFoundException& e ){
                std::cout << "[Avertisment] " << e.what() << " - sarit.\n";
                i--;
            } catch( const TeamFullException& e ){
                std::cout << "[Eroare] " << e.what() << "\n";
                break;
            }
        }
    };

    pick(blue, "Blue Team");
    pick(red,  "Red Team");

    double score = engine.analyzeDraft(blue.get(), red.get());
    printResultDraft(score);

    blue->display();
    red->display();

    std::cout << "\nPolymorphism\n";
    std::cout << "Alegeti tipul de rank pentru bonus win-rate:\n"
              << "  1. LowRank\n"
              << "  2. AverageRank\n"
              << "  3. HighRank\n"
              << "Alegere: ";
    int rankChoice = 1;
    std::cin >> rankChoice;

    std::vector<std::shared_ptr<Team>> teams = { blue, red };
    LoadChamp::demonstratePolymorphism(teams, engine, rankChoice);
}

void MainMenu::menuSearchChampion() const {
    std::cout << "Nume campion: ";
    std::string name;
    std::cin >> name;
    try {
        auto c = engine.findChampion(name);
        c->display();
    } catch( const ChampionNotFoundException& e ){
        std::cout << "[Nu gasit] " << e.what() << "\n";
    }
}

void MainMenu::menuLog() const {
    engine.printLog();
}

void MainMenu::menuAutoDemo() const {
    std::ifstream citire("tastatura.txt");
    if( !citire.is_open() ){
        std::cout << "tastatura.txt negasit.\n";
        return;
    }

    auto blue = std::make_shared<DraftTeam>("Blue Team", false);
    auto red  = std::make_shared<DraftTeam>("Red Team",  true);

    std::string name;
    for( int i = 0; i < 5; i++ ){ citire >> name; blue->banChampion(name); }
    for( int i = 0; i < 5; i++ ){ citire >> name; red->banChampion(name); }

    for( int i = 0; i < 5; i++ ){
        citire >> name;
        try { blue->addChampion(engine.findChampion(name)); }
        catch( const GameException& e ){ std::cout << "[Skip] " << e.what() << "\n"; }
    }
    for( int i = 0; i < 5; i++ ){
        citire >> name;
        try { red->addChampion(engine.findChampion(name)); }
        catch( const GameException& e ){ std::cout << "[Skip] " << e.what() << "\n"; }
    }

    double score = engine.analyzeDraft(blue.get(), red.get());
    printResultDraft(score);
    blue->display();
    red->display();
}

void MainMenu::run(){
    std::cout << "Se incarca datele campionilor...\n";
    LoadChamp::loadChampionData(engine, "lolalytics_all_172.json");
    std::cout << "Date incarcate. " << engine.getChampionPool().size() << " campioni.\n";

    demoExceptions();

    std::cout << "\n--- Demo automat din tastatura.txt ---\n";
    menuAutoDemo();

    int choice = -1;
    do {
        std::cout << "\nMENIU\n"
                  << "1. Tier List\n"
                  << "2. Recomandari pe rol\n"
                  << "3. Analiza draft interactiva\n"
                  << "4. Cauta campion\n"
                  << "5. Log analize\n"
                  << "0. Iesire\n"
                  << "Alege: ";

        if( !(std::cin >> choice) ){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
            continue;
        }

        switch( choice ){
            case 1: menuTierList();       break;
            case 2: menuRecommend();      break;
            case 3: menuDraftAnalysis();  break;
            case 4: menuSearchChampion(); break;
            case 5: menuLog();            break;
            case 0: std::cout << "La revedere!\n"; break;
            default: std::cout << "Optiune invalida.\n"; break;
        }
    } while( choice != 0 );
}