#include "Team.h"

#include <utility>

int Team::numberTeams = 0;

Team::Team( std::string  name ) : teamName(std::move(name)) {
    numberTeams++;
}

Team::Team( std::string  name, std::vector<std::shared_ptr<Champion>> r )
    : teamName(std::move(name)), roster(std::move(r)) {
    numberTeams++;
}

Team::Team( const Team& other )
    : teamName(other.teamName), roster(other.roster) {
    numberTeams++;
}

Team& Team::operator=( const Team& other ){
    if( this != &other ){
        teamName = other.teamName;
        roster   = other.roster;
    }
    return *this;
}

void Team::addChampion( std::shared_ptr<Champion> champ ){
    if( isFull() )
        throw TeamFullException(teamName);
    roster.push_back(champ);
}

void Team::removeChampion( const std::string& name ){
    auto it = std::find_if(roster.begin(), roster.end(),
        [&name]( const auto& c ){
            return c->getName() == name;
        });
    if( it != roster.end() )
        roster.erase(it);
}

double Team::getAverageWinRate() const {
    if( roster.empty() )
        return 0.0;
    double sum = std::accumulate(roster.begin(), roster.end(), 0.0,
        []( double acc, const auto& c ){
            return acc + c->getWinRate();
        });
    return sum / static_cast<double>(roster.size());
}

double Team::getTeamSynergy() const {
    double total = 0.0;
    for( size_t i = 0; i < roster.size(); ++i )
        for( size_t j = i + 1; j < roster.size(); ++j )
            total += roster[i]->getSynergyWith(roster[j]->getName());
    return total;
}

void Team::display() const {
    std::cout << *this;
}

const Champion& Team::operator[]( int idx ) const {
    return *roster[idx];
}

const std::string& Team::getName() const { return teamName; }
int  Team::getSize() const { return static_cast<int>(roster.size()); }
bool Team::isFull()  const { return static_cast<int>(roster.size()) == MAX_SIZE; }

const std::vector<std::shared_ptr<Champion>>& Team::getRoster() const {
    return roster;
}

int Team::getTotalTeamsCreated() { return numberTeams; }

std::ostream& operator<<( std::ostream& os, const Team& t ){
    os << "=== Team: " << t.teamName
       << " (" << t.roster.size() << "/" << Team::MAX_SIZE << ") ===\n";
    for( const auto& c : t.roster )
        os << "  * " << c->getName()
           << " [" << Champion::roleToString(c->getRole()) << "]"
           << " WR:" << c->getWinRate() * 100 << "%\n";
    os << "  Avg WR: " << t.getAverageWinRate() * 100 << "%"
       << " | Synergy: " << t.getTeamSynergy() << "\n";
    return os;
}

std::istream& operator>>( std::istream& is, Team& t ){
    std::cout << "Nume echipa: ";
    is >> t.teamName;
    return is;
}

DraftTeam::DraftTeam( const std::string& name, bool redSide )
    : Team(name), currentPickIndex(0), side(redSide) {}

DraftTeam::DraftTeam( const DraftTeam& other ) = default;

DraftTeam& DraftTeam::operator=( const DraftTeam& other ){
    if( this != &other ){
        Team::operator=(other);
        bannedChampions  = other.bannedChampions;
        pickOrder        = other.pickOrder;
        currentPickIndex = other.currentPickIndex;
        side             = other.side;
    }
    return *this;
}

void DraftTeam::addChampion( std::shared_ptr<Champion> champ ){
    if( isChampionBanned(champ->getName()) ){
        std::cout << "[SKIP] " << champ->getName() << " este banat!\n";
        return;
    }
    Team::addChampion(champ);
    pickOrder.push_back(champ->getName());
    currentPickIndex++;
}

void DraftTeam::banChampion( const std::string& name ){
    bannedChampions.push_back(name);
    std::cout << "[BAN] " << teamName << " baneaza: " << name << "\n";
}

bool DraftTeam::isChampionBanned( const std::string& name ) const {
    return std::find(bannedChampions.begin(), bannedChampions.end(), name)
           != bannedChampions.end();
}

void DraftTeam::display() const {
    std::cout << "[Draft] ";
    Team::display();
    if( !bannedChampions.empty() ){
        std::cout << "  Bans: ";
        for( const auto& b : bannedChampions )
            std::cout << b << " ";
        std::cout << "\n";
    }
}

bool DraftTeam::isRedSide()           const { return side; }
int  DraftTeam::getCurrentPickIndex() const { return currentPickIndex; }
const std::vector<std::string>& DraftTeam::getBans() const { return bannedChampions; }