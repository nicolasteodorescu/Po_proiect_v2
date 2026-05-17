#include "Analysis.h"

Analysis::Analysis()
    : championPool("Global Champion Pool"),
      itemPool("Global Item Pool") {}

Analysis& Analysis::getInstance() {
    static Analysis instance;
    return instance;
}

void Analysis::registerChampion(std::shared_ptr<Champion> champ) {
    championPool.add(champ);
}

void Analysis::registerItem(std::shared_ptr<Item> item) {
    itemPool.add(item);
}

void Analysis::addMatchup(const std::string& c1, const std::string& c2, double wr) {
    matchupData[{c1, c2}] = wr;
}

double Analysis::getMatchupWinRate(const std::string& c1,
                                   const std::string& c2) const {
    auto it = matchupData.find({c1, c2});
    return (it != matchupData.end()) ? it->second : 0.5;
}

std::shared_ptr<Champion> Analysis::findChampion(const std::string& name) const {
    const auto& items = championPool.getItems();
    auto it = std::find_if(items.begin(), items.end(),
        [&name](const std::shared_ptr<Champion>& c){ return c->getName() == name; });
    if (it == items.end())
        throw ChampionNotFoundException(name);
    return *it;
}

double Analysis::analyzeDraft(const Team* blue, const Team* red) const {
    double score = 0.0;

    score += (blue->getAverageWinRate() - red->getAverageWinRate()) * 10.0;
    score += blue->getTeamSynergy() - red->getTeamSynergy();

    for (const auto& bc : blue->getRoster())
        for (const auto& rc : red->getRoster())
            score += getMatchupWinRate(bc->getName(), rc->getName()) - 0.5;

    const DraftTeam* dt = dynamic_cast<const DraftTeam*>(blue);
    if (dt)
        score += 0.1 * static_cast<int>(dt->getBans().size());

    std::string logEntry = "Analyzed: " + blue->getName()
                         + " vs " + red->getName()
                         + " -> score=" + std::to_string(score);
    const_cast<Analysis*>(this)->analysisLog.push_back(logEntry);

    return score;
}

std::vector<std::shared_ptr<Champion>>
Analysis::recommendForRole(Champion::Role role, int top) const {
    auto filtered = championPool.filter(
        [role](const std::shared_ptr<Champion>& c){
            return c->getRole() == role;
        });
    return filtered.topN(top);
}

void Analysis::printTierList() const {
    RankedList<Champion> copy = championPool;
    copy.sortByWinRate();

    std::cout << "\nTIER LIST\n";
    const auto& items = copy.getItems();

    std::vector<std::string> tiers;
    std::transform(items.begin(), items.end(), std::back_inserter(tiers),
        [](const std::shared_ptr<Champion>& c) -> std::string {
            double wr = c->getWinRate();
            if (wr >= 0.55)
                return "S";
            else if (wr >= 0.52)
                return "A";
            else if (wr >= 0.49)
                return "B";
            else if (wr >= 0.46)
                return "C";
            else
                return "D";
        });

    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << "  [" << tiers[i] << "] "
                  << items[i]->getName()
                  << " (" << Champion::roleToString(items[i]->getRole()) << ")"
                  << " -> " << items[i]->getWinRate() * 100 << "% WR\n";
    }
}

void Analysis::printLog() const {
    std::cout << "\nAnalysis Log (" << analysisLog.size() << " entries)\n";
    for (const auto& entry : analysisLog)
        std::cout << "  * " << entry << "\n";
}

const RankedList<Champion>& Analysis::getChampionPool() const { return championPool; }
const RankedList<Item>&     Analysis::getItemPool()     const { return itemPool; }

std::shared_ptr<Champion>
ChampionFactory::create(const std::string& name, Template tmpl) {
    double wr, pr, br;
    Champion::Role role;
    std::string desc;

    switch (tmpl) {
        case Template::ASSASSIN_MAGE:
            wr = 0.521;
            pr = 0.12;
            br = 0.18;
            role = Role::MID;
            desc = "Burst AP assassin";
            break;
        case Template::ASSASSIN_AD:
            wr = 0.490;
            pr = 0.25;
            br = 0.20;
            role = Role::JUNGLE;
            desc = "Burst AD assassin";
            break;
        case Template::TANK_FIGHTER:
            wr = 0.519;
            pr = 0.08;
            br = 0.10;
            role = Role::TOP;
            desc = "Frontline tank";
            break;
        case Template::CONTROL_MAGE:
            wr = 0.514;
            pr = 0.14;
            br = 0.09;
            role = Role::MID;
            desc = "AP mage";
            break;
        case Template::MARKSMAN:
            wr = 0.508;
            pr = 0.20;
            br = 0.08;
            role = Role::ADC;
            desc = "Hypercarry ADC";
            break;
        case Template::ENCHANTER_SUPPORT:
            wr = 0.525;
            pr = 0.11;
            br = 0.06;
            role = Role::SUPPORT;
            desc = "Utility support";
            break;
        case Template::TANK_SUPPORT:
            wr = 0.510;
            pr = 0.14;
            br = 0.10;
            role = Role::SUPPORT;
            desc = "Frontline engage support";
            break;
        case Template::BRUISER:
        default:
            wr = 0.516;
            pr = 0.13;
            br = 0.11;
            role = Role::JUNGLE;
            desc = "Bruiser jungler";
            break;
    }

    return std::make_shared<Champion>(name, role, wr, pr, br, desc);
}

std::vector<std::shared_ptr<Champion>>
ChampionFactory::createBatch(
    const std::vector<std::pair<std::string, Template>>& specs)
{
    std::vector<std::shared_ptr<Champion>> result;
    result.reserve(specs.size());
    for (const auto& [name, tmpl] : specs)
        result.push_back(create(name, tmpl));
    return result;
}