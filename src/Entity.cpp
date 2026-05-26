#include "Entity.h"

#include <utility>

int Entity::instCnt = 0;

Entity::Entity( std::string  n, std::string  desc )
    : name(std::move(n)), description(std::move(desc)) { ++instCnt; }

Entity::Entity( const Entity& other )
    : name(other.name), description(other.description) { ++instCnt; }

Entity& Entity::operator=( const Entity& other ){
    if( this != &other ){
        name        = other.name;
        description = other.description;
    }
    return *this;
}

Entity::~Entity(){ --instCnt; }

void Entity::display() const { std::cout << *this << "\n"; }

const std::string& Entity::getName() const {
    return name;
}
const std::string& Entity::getDescription() const {
    return description;
}
int  Entity::getLiveInstances() {
    return instCnt;
}
void Entity::resetInstanceCount() {
    instCnt = 0;
}

bool Entity::operator<( const Entity& other ) const {
    return getWinRate() < other.getWinRate();
}
bool Entity::operator==( const Entity& other ) const {
    return name == other.name;
}

std::ostream& operator<<( std::ostream& os, const Entity& e ){
    os << "[" << e.getType() << "] " << e.name;
    if( !e.description.empty() )
        os << " - " << e.description;
    return os;
}

std::istream& operator>>( std::istream& is, Entity& e ){
    std::cout << "Nume entitate: ";
    is >> e.name;
    std::cout << "Descriere: ";
    is.ignore();
    std::getline(is, e.description);
    return is;
}

std::string Champion::topChampionName;

std::string Champion::roleToString( Role r ){
    switch( r ){
        case TOP:
            return "Top";
        case JUNGLE:
            return "Jungle";
        case MID:
            return "Mid";
        case ADC:
            return "ADC";
        case SUPPORT:
            return "Support";
        default:
            return "None";
    }
}

Champion::Champion( const std::string& n, Role r, double wr, double pr, double br, const std::string& desc )
    : Entity(n, desc), winRate(wr), pickRate(pr), banRate(br), role(r)
{
    if( wr < 0.0 || wr > 1.0 )
        throw InvalidWinRateException(wr);
    if( wr > 0.55 && topChampionName.empty() )
        topChampionName = n;
}

Champion::Champion( const std::string& n, Role r, double wr ) : Champion(n, r, wr, 0.1, 0.05, "") {}

Champion::Champion( const Champion& other ) = default;


Champion& Champion::operator=( const Champion& other ){
    if( this != &other ){
        Entity::operator=(other);
        role = other.role;
        winRate = other.winRate;
        pickRate = other.pickRate;
        banRate = other.banRate;
        synergyScores = other.synergyScores;
    }
    return *this;
}

void Champion::setWinRate( double wr ){
    if( wr < 0.0 || wr > 1.0 )
        throw InvalidWinRateException(wr);
    winRate = wr;
}

double Champion::getWinRate() const {
    return winRate;
}
std::string Champion::getType() const {
    return "Champion";
}

void Champion::display() const {
    std::cout << "[Champion] " << name
              << " (" << roleToString(role) << ")"
              << " WR:" << winRate * 100 << "%"
              << " PR:" << pickRate * 100 << "%"
              << " BR:" << banRate  * 100 << "%";
    if( !description.empty() )
        std::cout << " | " << description;
    std::cout << "\n";
}

Champion Champion::operator+( const Champion& other ) const {
    Champion combined(*this);
    combined.name = name + "+" + other.name;
    combined.winRate = (winRate + other.winRate) / 2.0;
    for( const auto& [k, v] : other.synergyScores )
        combined.synergyScores[k] = v;
    return combined;
}

void Champion::addSynergy( const std::string& cname, double score ){
    synergyScores[cname] = score;
}

double Champion::getSynergyWith( const std::string& cname ) const {
    auto it = synergyScores.find(cname);
    return (it != synergyScores.end()) ? it->second : 0.0;
}

Champion::Role Champion::getRole() const { return role; }
double Champion::getPickRate() const {
    return pickRate;
}
double Champion::getBanRate() const {
    return banRate;
}
const std::string& Champion::getTopChampionName() {
    return topChampionName;
}