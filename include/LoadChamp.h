#ifndef OOP_LOADCHAMP_H
#define OOP_LOADCHAMP_H

#include "Analysis.h"
#include "Entity.h"
#include "Team.h"
#include <fstream>
#include <unordered_map>

#define CHAMPMAXX 172

class LoadChamp {
private:
    static std::string campioni[CHAMPMAXX];
    static std::vector<std::shared_ptr<Champion>> batch;
    static std::unordered_map<std::string,
        std::unordered_map<std::string, double>> matchupData;
    static std::unordered_map<std::string, double> champWinRates;

public:
    static void loadMatchupData(const std::string& jsonPath = "lolalytics_all_172.json");
    static void loadChampionData(Analysis& engine,
                                 const std::string& jsonPath = "lolalytics_all_172.json");
    static void demonstratePolymorphism(
        const std::vector<std::shared_ptr<Team>>& teams, Analysis& engine);
};

#endif // OOP_LOADCHAMP_H