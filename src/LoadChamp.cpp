#include "LoadChamp.h"

std::string LoadChamp::campioni[CHAMPMAXX] = {
    "Aatrox", "Ahri", "Akali", "Akshan", "Alistar", "Ambessa", "Amumu", "Anivia", "Annie",
    "Aphelios", "Ashe", "AurelionSol", "Aurora", "Azir", "Bard", "BelVeth", "Blitzcrank", "Brand",
    "Braum", "Briar", "Caitlyn", "Camille", "Cassiopeia", "ChoGath", "Corki", "Darius", "Diana",
    "DrMundo", "Draven", "Ekko", "Elise", "Evelynn", "Ezreal", "Fiddlesticks", "Fiora", "Fizz",
    "Galio", "Gangplank", "Garen", "Gnar", "Gragas", "Graves", "Gwen", "Hecarim", "Heimerdinger",
    "Hwei", "Illaoi", "Irelia", "Ivern", "Janna", "JarvanIV", "Jax", "Jayce", "Jhin", "Jinx",
    "KaiSa", "Kalista", "Karma", "Karthus", "Kassadin", "Katarina", "Kayle", "Kayn", "Kennen",
    "KhaZix", "Kindred", "Kled", "KogMaw", "KSante", "LeBlanc", "LeeSin", "Leona", "Lillia",
    "Lissandra", "Lucian", "Lulu", "Lux", "Malphite", "Malzahar", "Maokai", "MasterYi", "Mel",
    "Milio", "MissFortune", "Mordekaiser", "Morgana", "Naafiri", "Nami", "Nasus", "Nautilus",
    "Neeko", "Nidalee", "Nilah", "Nocturne", "NunuWillump", "Olaf", "Orianna", "Ornn", "Pantheon",
    "Poppy", "Pyke", "Qiyana", "Quinn", "Rakan", "Rammus", "RekSai", "Rell", "RenataGlasc",
    "Renekton", "Rengar", "Riven", "Rumble", "Ryze", "Samira", "Sejuani", "Senna", "Seraphine",
    "Sett", "Shaco", "Shen", "Shyvana", "Singed", "Sion", "Sivir", "Skarner", "Smolder", "Sona",
    "Soraka", "Swain", "Sylas", "Syndra", "TahmKench", "Taliyah", "Talon", "Taric", "Teemo",
    "Thresh", "Tristana", "Trundle", "Tryndamere", "TwistedFate", "Twitch", "Udyr", "Urgot",
    "Varus", "Vayne", "Veigar", "VelKoz", "Vex", "Vi", "Viego", "Viktor", "Vladimir", "Volibear",
    "Warwick", "Wukong", "Xayah", "Xerath", "XinZhao", "Yasuo", "Yone", "Yorick", "Yunara",
    "Yuumi", "Zaahen", "Zac", "Zed", "Zeri", "Ziggs", "Zilean", "Zoe", "Zyra"
};

std::vector<std::shared_ptr<Champion>> LoadChamp::batch;
std::unordered_map<std::string,
    std::unordered_map<std::string, double>> LoadChamp::matchupData;
std::unordered_map<std::string, double> LoadChamp::champWinRates;

void LoadChamp::loadMatchupData( const std::string& jsonPath ){
    std::ifstream f(jsonPath);
    if( !f.is_open() ){
        std::cerr << "[LoadChamp] Nu s-a putut deschide: " << jsonPath << "\n";
        return;
    }

    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    auto extractString = [&]( const std::string& src, const std::string& key ) -> std::string {
        std::string search = "\"" + key + "\"";
        size_t pos = src.find(search);
        if( pos == std::string::npos ) return "";
        pos = src.find('"', pos + search.size());
        if( pos == std::string::npos ) return "";
        size_t end = src.find('"', pos + 1);
        if( end == std::string::npos ) return "";
        return src.substr(pos + 1, end - pos - 1);
    };

    auto extractDouble = [&]( const std::string& src, const std::string& key ) -> double {
        std::string search = "\"" + key + "\"";
        size_t pos = src.find(search);
        if( pos == std::string::npos ) return -1.0;
        pos = src.find(':', pos + search.size());
        if( pos == std::string::npos ) return -1.0;
        pos++;
        while( pos < src.size() && (src[pos] == ' ' || src[pos] == '\n') ) pos++;
        size_t end = pos;
        while( end < src.size() && (std::isdigit(src[end]) || src[end] == '.' || src[end] == '-') )
            end++;
        try { return std::stod(src.substr(pos, end - pos)); }
        catch( ... ) { return -1.0; }
    };

    size_t pos = 0;
    while( (pos = content.find('{', pos)) != std::string::npos ){
        int depth = 0;
        size_t start = pos, end = pos;
        for( size_t k = pos; k < content.size(); k++ ){
            if( content[k] == '{' ) depth++;
            else if( content[k] == '}' ){
                depth--;
                if( depth == 0 ){ end = k; break; }
            }
        }
        std::string champBlock = content.substr(start, end - start + 1);
        std::string champName  = extractString(champBlock, "name");
        if( champName.empty() ){ pos = end + 1; continue; }

        double champWR = extractDouble(champBlock, "championWinRate");
        if( champWR >= 0.0 ) champWinRates[champName] = champWR / 100.0;

        size_t cpos = champBlock.find("\"counters\"");
        if( cpos != std::string::npos ){
            size_t arrStart = champBlock.find('[', cpos);
            size_t arrEnd   = champBlock.find(']', arrStart);
            if( arrStart != std::string::npos && arrEnd != std::string::npos ){
                std::string countersBlock = champBlock.substr(arrStart, arrEnd - arrStart + 1);
                size_t cobj = 0;
                while( (cobj = countersBlock.find('{', cobj)) != std::string::npos ){
                    int d2 = 0;
                    size_t cs = cobj, copy = cobj;
                    for( size_t k = cobj; k < countersBlock.size(); k++ ){
                        if( countersBlock[k] == '{' ) d2++;
                        else if( countersBlock[k] == '}' ){
                            d2--;
                            if( d2 == 0 ){ copy = k; break; }
                        }
                    }
                    std::string counterObj = countersBlock.substr(cs, copy - cs + 1);
                    std::string oppName    = extractString(counterObj, "name");
                    double wr              = extractDouble(counterObj, "winRateAgainst");
                    if( !oppName.empty() && wr >= 0.0 )
                        matchupData[champName][oppName] = wr / 100.0;
                    cobj = copy + 1;
                }
            }
        }
        pos = end + 1;
    }
    std::cout << "[LoadChamp] JSON incarcat: " << matchupData.size()
              << " campioni cu date de matchup.\n";
}

void LoadChamp::loadChampionData( Analysis& engine, const std::string& jsonPath ){
    auto add = [&]( const std::vector<std::pair<std::string, ChampionFactory::Template>>& specs ){
        auto part = ChampionFactory::createBatch(specs);
        batch.insert(batch.end(), part.begin(), part.end());
    };

    loadMatchupData(jsonPath);

    add({
        {"Aatrox", ChampionFactory::Template::BRUISER},
        {"Ahri", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Akali", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Akshan", ChampionFactory::Template::MARKSMAN},
        {"Alistar", ChampionFactory::Template::TANK_SUPPORT},
        {"Ambessa", ChampionFactory::Template::BRUISER},
        {"Amumu", ChampionFactory::Template::TANK_FIGHTER},
        {"Anivia", ChampionFactory::Template::CONTROL_MAGE},
        {"Annie", ChampionFactory::Template::CONTROL_MAGE},
        {"Aphelios", ChampionFactory::Template::MARKSMAN},
        {"Ashe", ChampionFactory::Template::MARKSMAN},
        {"AurelionSol", ChampionFactory::Template::CONTROL_MAGE},
        {"Aurora", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Azir", ChampionFactory::Template::CONTROL_MAGE},
        {"Bard", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"BelVeth", ChampionFactory::Template::BRUISER},
        {"Blitzcrank", ChampionFactory::Template::TANK_SUPPORT},
        {"Brand", ChampionFactory::Template::CONTROL_MAGE},
        {"Braum", ChampionFactory::Template::TANK_SUPPORT},
        {"Briar", ChampionFactory::Template::BRUISER},
    });
    add({
        {"Caitlyn", ChampionFactory::Template::MARKSMAN},
        {"Camille", ChampionFactory::Template::TANK_FIGHTER},
        {"Cassiopeia", ChampionFactory::Template::CONTROL_MAGE},
        {"ChoGath", ChampionFactory::Template::TANK_FIGHTER},
        {"Corki", ChampionFactory::Template::MARKSMAN},
        {"Darius", ChampionFactory::Template::TANK_FIGHTER},
        {"Diana", ChampionFactory::Template::ASSASSIN_MAGE},
        {"DrMundo", ChampionFactory::Template::TANK_FIGHTER},
        {"Draven", ChampionFactory::Template::MARKSMAN},
        {"Ekko", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Elise", ChampionFactory::Template::BRUISER},
        {"Evelynn", ChampionFactory::Template::BRUISER},
        {"Ezreal", ChampionFactory::Template::MARKSMAN},
        {"Fiddlesticks", ChampionFactory::Template::BRUISER},
        {"Fiora", ChampionFactory::Template::TANK_FIGHTER},
        {"Fizz", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Galio", ChampionFactory::Template::TANK_FIGHTER},
        {"Gangplank", ChampionFactory::Template::TANK_FIGHTER},
        {"Garen", ChampionFactory::Template::TANK_FIGHTER},
        {"Gnar", ChampionFactory::Template::TANK_FIGHTER},
    });
    add({
        {"Gragas", ChampionFactory::Template::TANK_FIGHTER},
        {"Graves", ChampionFactory::Template::BRUISER},
        {"Gwen", ChampionFactory::Template::TANK_FIGHTER},
        {"Hecarim", ChampionFactory::Template::BRUISER},
        {"Heimerdinger", ChampionFactory::Template::CONTROL_MAGE},
        {"Hwei", ChampionFactory::Template::CONTROL_MAGE},
        {"Illaoi", ChampionFactory::Template::TANK_FIGHTER},
        {"Irelia", ChampionFactory::Template::TANK_FIGHTER},
        {"Ivern", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Janna", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"JarvanIV", ChampionFactory::Template::BRUISER},
        {"Jax", ChampionFactory::Template::TANK_FIGHTER},
        {"Jayce", ChampionFactory::Template::ASSASSIN_AD},
        {"Jhin", ChampionFactory::Template::MARKSMAN},
        {"Jinx", ChampionFactory::Template::MARKSMAN},
        {"KSante", ChampionFactory::Template::TANK_FIGHTER},
        {"KaiSa", ChampionFactory::Template::MARKSMAN},
        {"Kalista", ChampionFactory::Template::MARKSMAN},
        {"Karma", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Karthus", ChampionFactory::Template::CONTROL_MAGE},
    });
    add({
        {"Kassadin", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Katarina", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Kayle", ChampionFactory::Template::CONTROL_MAGE},
        {"Kayn", ChampionFactory::Template::ASSASSIN_AD},
        {"Kennen", ChampionFactory::Template::CONTROL_MAGE},
        {"KhaZix", ChampionFactory::Template::ASSASSIN_AD},
        {"Kindred", ChampionFactory::Template::ASSASSIN_AD},
        {"Kled", ChampionFactory::Template::TANK_FIGHTER},
        {"KogMaw", ChampionFactory::Template::MARKSMAN},
        {"LeBlanc", ChampionFactory::Template::ASSASSIN_MAGE},
        {"LeeSin", ChampionFactory::Template::BRUISER},
        {"Leona", ChampionFactory::Template::TANK_SUPPORT},
        {"Lillia", ChampionFactory::Template::BRUISER},
        {"Lissandra", ChampionFactory::Template::CONTROL_MAGE},
        {"Lucian", ChampionFactory::Template::MARKSMAN},
        {"Lulu", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Lux", ChampionFactory::Template::CONTROL_MAGE},
        {"Malphite", ChampionFactory::Template::TANK_FIGHTER},
        {"Malzahar", ChampionFactory::Template::CONTROL_MAGE},
        {"Maokai", ChampionFactory::Template::TANK_FIGHTER},
    });
    add({
        {"MasterYi", ChampionFactory::Template::BRUISER},
        {"Mel", ChampionFactory::Template::CONTROL_MAGE},
        {"Milio", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"MissFortune", ChampionFactory::Template::MARKSMAN},
        {"Mordekaiser", ChampionFactory::Template::TANK_FIGHTER},
        {"Morgana", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Naafiri", ChampionFactory::Template::ASSASSIN_AD},
        {"Nami", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Nasus", ChampionFactory::Template::TANK_FIGHTER},
        {"Nautilus", ChampionFactory::Template::TANK_SUPPORT},
        {"Neeko", ChampionFactory::Template::CONTROL_MAGE},
        {"Nidalee", ChampionFactory::Template::BRUISER},
        {"Nilah", ChampionFactory::Template::MARKSMAN},
        {"Nocturne", ChampionFactory::Template::ASSASSIN_AD},
        {"NunuWillump", ChampionFactory::Template::BRUISER},
        {"Olaf", ChampionFactory::Template::TANK_FIGHTER},
        {"Orianna", ChampionFactory::Template::CONTROL_MAGE},
        {"Ornn", ChampionFactory::Template::TANK_FIGHTER},
        {"Pantheon", ChampionFactory::Template::TANK_SUPPORT},
        {"Poppy", ChampionFactory::Template::TANK_FIGHTER},
    });
    add({
        {"Pyke", ChampionFactory::Template::ASSASSIN_AD},
        {"Qiyana", ChampionFactory::Template::ASSASSIN_AD},
        {"Quinn", ChampionFactory::Template::TANK_FIGHTER},
        {"Rakan", ChampionFactory::Template::TANK_SUPPORT},
        {"Rammus", ChampionFactory::Template::BRUISER},
        {"RekSai", ChampionFactory::Template::BRUISER},
        {"Rell", ChampionFactory::Template::TANK_SUPPORT},
        {"RenataGlasc", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Renekton", ChampionFactory::Template::TANK_FIGHTER},
        {"Rengar", ChampionFactory::Template::ASSASSIN_AD},
        {"Riven", ChampionFactory::Template::TANK_FIGHTER},
        {"Rumble", ChampionFactory::Template::TANK_FIGHTER},
        {"Ryze", ChampionFactory::Template::CONTROL_MAGE},
        {"Samira", ChampionFactory::Template::MARKSMAN},
        {"Sejuani", ChampionFactory::Template::BRUISER},
        {"Senna", ChampionFactory::Template::MARKSMAN},
        {"Seraphine", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Sett", ChampionFactory::Template::TANK_FIGHTER},
        {"Shaco", ChampionFactory::Template::ASSASSIN_AD},
        {"Shen", ChampionFactory::Template::TANK_FIGHTER},
    });
    add({
        {"Shyvana", ChampionFactory::Template::BRUISER},
        {"Singed", ChampionFactory::Template::TANK_FIGHTER},
        {"Sion", ChampionFactory::Template::TANK_FIGHTER},
        {"Sivir", ChampionFactory::Template::MARKSMAN},
        {"Skarner", ChampionFactory::Template::BRUISER},
        {"Smolder", ChampionFactory::Template::MARKSMAN},
        {"Sona", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Soraka", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Swain", ChampionFactory::Template::CONTROL_MAGE},
        {"Sylas", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Syndra", ChampionFactory::Template::CONTROL_MAGE},
        {"TahmKench", ChampionFactory::Template::TANK_SUPPORT},
        {"Taliyah", ChampionFactory::Template::CONTROL_MAGE},
        {"Talon", ChampionFactory::Template::ASSASSIN_AD},
        {"Taric", ChampionFactory::Template::TANK_SUPPORT},
        {"Teemo", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Thresh", ChampionFactory::Template::TANK_SUPPORT},
        {"Tristana", ChampionFactory::Template::MARKSMAN},
        {"Trundle", ChampionFactory::Template::TANK_FIGHTER},
        {"Tryndamere", ChampionFactory::Template::TANK_FIGHTER},
    });
    add({
        {"TwistedFate", ChampionFactory::Template::CONTROL_MAGE},
        {"Twitch", ChampionFactory::Template::MARKSMAN},
        {"Udyr", ChampionFactory::Template::BRUISER},
        {"Urgot", ChampionFactory::Template::TANK_FIGHTER},
        {"Varus", ChampionFactory::Template::MARKSMAN},
        {"Vayne", ChampionFactory::Template::MARKSMAN},
        {"Veigar", ChampionFactory::Template::CONTROL_MAGE},
        {"VelKoz", ChampionFactory::Template::CONTROL_MAGE},
        {"Vex", ChampionFactory::Template::CONTROL_MAGE},
        {"Vi", ChampionFactory::Template::BRUISER},
        {"Viego", ChampionFactory::Template::BRUISER},
        {"Viktor", ChampionFactory::Template::CONTROL_MAGE},
        {"Vladimir", ChampionFactory::Template::CONTROL_MAGE},
        {"Volibear", ChampionFactory::Template::BRUISER},
        {"Warwick", ChampionFactory::Template::BRUISER},
        {"Wukong", ChampionFactory::Template::BRUISER},
        {"Xayah", ChampionFactory::Template::MARKSMAN},
        {"Xerath", ChampionFactory::Template::CONTROL_MAGE},
        {"XinZhao", ChampionFactory::Template::BRUISER},
        {"Yasuo", ChampionFactory::Template::TANK_FIGHTER},
        {"Yone", ChampionFactory::Template::TANK_FIGHTER},
        {"Yorick", ChampionFactory::Template::TANK_FIGHTER},
        {"Yunara", ChampionFactory::Template::MARKSMAN},
        {"Yuumi", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Zaahen", ChampionFactory::Template::BRUISER},
        {"Zac", ChampionFactory::Template::TANK_FIGHTER},
        {"Zed", ChampionFactory::Template::ASSASSIN_AD},
        {"Zeri", ChampionFactory::Template::MARKSMAN},
        {"Ziggs", ChampionFactory::Template::CONTROL_MAGE},
        {"Zilean", ChampionFactory::Template::ENCHANTER_SUPPORT},
        {"Zoe", ChampionFactory::Template::ASSASSIN_MAGE},
        {"Zyra", ChampionFactory::Template::CONTROL_MAGE},
    });

    for( const auto& c : batch )
        engine.registerChampion(c);

    for( const auto& c : batch ){
        auto it = champWinRates.find(c->getName());
        if( it != champWinRates.end() )
            c->setWinRate(it->second);
    }

    for( int i = 0; i < CHAMPMAXX; i++ ){
        for( int j = i + 1; j < CHAMPMAXX; j++ ){
            const std::string& ci = campioni[i];
            const std::string& cj = campioni[j];

            double syn = (rand() % 101) / 100.0;
            batch[i]->addSynergy(cj, syn);
            batch[j]->addSynergy(ci, syn);

            double wr_i_vs_j = 0.50, wr_j_vs_i = 0.50;

            auto it_i = matchupData.find(ci);
            if( it_i != matchupData.end() ){
                auto it_ij = it_i->second.find(cj);
                if( it_ij != it_i->second.end() ) wr_i_vs_j = it_ij->second;
            }
            auto it_j = matchupData.find(cj);
            if( it_j != matchupData.end() ){
                auto it_ji = it_j->second.find(ci);
                if( it_ji != it_j->second.end() ) wr_j_vs_i = it_ji->second;
            }

            engine.addMatchup(ci, cj, wr_i_vs_j);
            engine.addMatchup(cj, ci, wr_j_vs_i);
        }
    }
}

void LoadChamp::demonstratePolymorphism( const std::vector<std::shared_ptr<Team>>& teams, const Analysis& engine, int rankChoice ){
    std::cout << "\nDraftTeam\n";
    for( const auto& teamPtr : teams ){
        const Team* base = teamPtr.get();
        base->display();
    }
    std::unique_ptr<RankedList<Champion>> ranked =
        LoadChamp::createRankedPool(rankChoice, engine.getChampionPool());

    ranked->applyRankBonus();

    std::cout << "\nEchipe dupa alegerea rankului:\n";
    for( const auto& teamPtr : teams ){
        const Team* base = teamPtr.get();
        std::cout << base->getName() << "\n";
        for( const auto& c : base->getRoster() )
            c->display();
        std::cout << "Avg WR nou: " << base->getAverageWinRate() * 100 << "%\n";
    }

    if( teams.size() >= 2 ){
        double newScore = engine.analyzeDraft(teams[0].get(), teams[1].get());
        std::cout << "\nScor draft dupa rank bonus:\n";
        std::cout << "Blue Team advantage score: " << newScore << "\n";
        if( newScore > 0 )
            std::cout << "  -> Blue Team castiga draftul!\n";
        else if( newScore < 0 )
            std::cout << "  -> Red Team castiga draftul!\n";
        else
            std::cout << "  -> Draft egal!\n";
    }
}

std::unique_ptr<RankedList<Champion>>
LoadChamp::createRankedPool( int rankChoice, const RankedList<Champion>& pool ){
    std::unique_ptr<RankedList<Champion>> result;
    switch( rankChoice ){
        case 1:
            result = std::make_unique<AverageRank>("AverageRank Pool");
            break;
        case 2:
            result = std::make_unique<HighRank>("HighRank Pool");
            break;
        default:
            result = std::make_unique<LowRank>("LowRank Pool");
            break;
    }
    for( const auto& c : pool.getItems() )
        result->add(c);
    return result;
}