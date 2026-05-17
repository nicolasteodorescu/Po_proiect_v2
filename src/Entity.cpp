#include "Entity.h"

int Entity::instCnt = 0;

Entity::Entity(const std::string& n, const std::string& desc) : name(n), description(desc) {
    ++instCnt;
}

Entity::Entity(const Entity& other) : name(other.name), description(other.description) {
    ++instCnt;
}

Entity& Entity::operator=(const Entity& other) {
    if (this != &other) {
        name = other.name;
        description = other.description;
    }

    return *this;
}

Entity::~Entity() {
    --instCnt;
}

void Entity::display() const {
    std::cout << *this << "\n";
}

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

bool Entity::operator<(const Entity& other) const {
    return getWinRate() < other.getWinRate();
}
bool Entity::operator==(const Entity& other) const {
    return name == other.name;
}

std::ostream& operator<<(std::ostream& os, const Entity& e) {
    os << "[" << e.getType() << "] " << e.name;
    if (!e.description.empty())
        os << " - " << e.description;
    return os;
}

std::istream& operator>>(std::istream& is, Entity& e) {
    std::cout << "Nume entitate: ";
    is >> e.name;
    std::cout << "Descriere: ";
    is.ignore();
    std::getline(is, e.description);
    return is;
}

std::string Champion::topChampionName = "";

std::string Champion::roleToString(Role r) {
    switch (r) {
        case TOP:     return "Top";
        case JUNGLE:  return "Jungle";
        case MID:     return "Mid";
        case ADC:     return "ADC";
        case SUPPORT: return "Support";
        default:      return "None";
    }
}

Champion::Champion(const std::string& n, Role r, double wr, double pr, double br, const std::string& desc) : Entity(n, desc), winRate(wr), pickRate(pr), banRate(br), role(r) {
    if (wr < 0.0 || wr > 1.0)
        throw InvalidWinRateException(wr);

    if (wr > 0.55 && topChampionName.empty())
        topChampionName = n;
}

Champion::Champion(const std::string& n, Role r, double wr) : Champion(n, r, wr, 0.1, 0.05, "") {}

Champion::Champion(const Champion& other) : Entity(other), winRate(other.winRate), pickRate(other.pickRate), banRate(other.banRate), synergyScores(other.synergyScores), role(other.role) {}

Champion& Champion::operator=(const Champion& other) {
    if (this != &other) {
        Entity::operator=(other);
        role = other.role;
        winRate = other.winRate;
        pickRate = other.pickRate;
        banRate = other.banRate;
        synergyScores = other.synergyScores;
    }

    return *this;
}

void Champion::setWinRate(double wr) {
    if (wr < 0.0 || wr > 1.0)
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
    std::cout << "[Champion] " << name << " (" << roleToString(role) << ")" << " WR:" << winRate * 100 << "%" << " PR:" << pickRate * 100 << "%" << " BR:" << banRate * 100 << "%";
    if (!description.empty())
        std::cout << " | " << description;
    std::cout << "\n";
}

Champion Champion::operator+(const Champion& other) const {
    Champion combined(*this);
    combined.name = name + "+" + other.name;
    combined.winRate = (winRate + other.winRate) / 2.0;
    for (const auto& [k, v] : other.synergyScores)
        combined.synergyScores[k] = v;
    return combined;
}

void   Champion::addSynergy(const std::string& cname, double score) {
    synergyScores[cname] = score;
}
double Champion::getSynergyWith(const std::string& cname) const {
    auto it = synergyScores.find(cname);
    return (it != synergyScores.end()) ? it->second : 0.0;
}

Champion::Role Champion::getRole() const {
    return role;
}
double Champion::getPickRate() const {
    return pickRate;
}
double Champion::getBanRate()  const {
    return banRate;
}
const std::string& Champion::getTopChampionName() { return topChampionName; }

Item::Item(const std::string& n, int gold,
           int ad, int ap, int hp, int ar, int mr,
           int ah, int ms, int cs,
           double arp, double mrp, double as, double ls,
           double wr, const std::string& desc)
    : Entity(n, desc),
      goldCost(gold), attackDamage(ad), abilityPower(ap), health(hp),
      armor(ar), magicResist(mr), abilityHaste(ah),
      movementSpeed(ms), criticalStrike(cs),
      armorPen(arp), magicPen(mrp), attackSpeed(as), lifeSteal(ls),
      winRateStats(wr) {}

Item::Item(const std::string& n, int gold)
    : Item(n, gold, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0) {}

Item::Item(const Item& other)
    : Entity(other),
      goldCost(other.goldCost), attackDamage(other.attackDamage),
      abilityPower(other.abilityPower), health(other.health),
      armor(other.armor), magicResist(other.magicResist),
      abilityHaste(other.abilityHaste), movementSpeed(other.movementSpeed),
      criticalStrike(other.criticalStrike),
      armorPen(other.armorPen), magicPen(other.magicPen),
      attackSpeed(other.attackSpeed), lifeSteal(other.lifeSteal),
      winRateStats(other.winRateStats) {}

Item& Item::operator=(const Item& other) {
    if (this != &other) {
        Entity::operator=(other);
        goldCost = other.goldCost;
        attackDamage = other.attackDamage;
        abilityPower = other.abilityPower;
        health = other.health;
        armor = other.armor;
        magicResist = other.magicResist;
        abilityHaste = other.abilityHaste;
        movementSpeed = other.movementSpeed;
        criticalStrike = other.criticalStrike;
        armorPen = other.armorPen;
        magicPen = other.magicPen;
        attackSpeed = other.attackSpeed;
        lifeSteal = other.lifeSteal;
        winRateStats = other.winRateStats;
    }

    return *this;
}

void Item::display() const {
    std::cout << "[" << getType() << "] " << name << " Cost:" << goldCost << "g" << " WR:" << (0.5 + winRateStats) * 100 << "%";
    if (attackDamage > 0)
        std::cout << " AD:"  << attackDamage;
    if (abilityPower > 0)
        std::cout << " AP:"  << abilityPower;
    if (health > 0)
        std::cout << " HP:"  << health;
    if (armor > 0)
        std::cout << " AR:"  << armor;
    if (magicResist > 0)
        std::cout << " MR:"  << magicResist;
    if (abilityHaste > 0)
        std::cout << " AH:"  << abilityHaste;
    std::cout << "\n";
}

int Item::getGoldCost() const {
    return goldCost;
}
int Item::getAD() const {
    return attackDamage;
}
int Item::getAP() const {
    return abilityPower;
}
int Item::getHP() const {
    return health;
}
int Item::getAR() const {
    return armor;
}
int Item::getMR() const {
    return magicResist;
}
int Item::getAH() const {
    return abilityHaste;
}
int Item::getMS() const {
    return movementSpeed;
}
int Item::getCS() const {
    return criticalStrike;
}
double Item::getARP() const {
    return armorPen;
}
double Item::getMRP() const {
    return magicPen;
}
double Item::getAS() const {
    return attackSpeed;
}
double Item::getLS() const {
    return lifeSteal;
}
double Item::getWinRateImpact() const {
    return winRateStats;
}
int Item::operator-(const Item& other) const {
    return goldCost - other.goldCost;
}

MythicItem::MythicItem(const std::string& n, int gold,
                       int ad, int ap, int hp, int ar, int mr,
                       int ah, int ms, int cs,
                       double arp, double mrp, double as, double ls,
                       double wr,
                       const std::string& passive, int stacks,
                       const std::string& desc)
    : Item(n, gold, ad, ap, hp, ar, mr, ah, ms, cs, arp, mrp, as, ls, wr, desc),
      mythicPassive(passive), stacksPerChampion(stacks) {}

MythicItem::MythicItem(const MythicItem& other)
    : Item(other),
      mythicPassive(other.mythicPassive),
      stacksPerChampion(other.stacksPerChampion) {}

MythicItem& MythicItem::operator=(const MythicItem& other) {
    if (this != &other) {
        Item::operator=(other);
        mythicPassive      = other.mythicPassive;
        stacksPerChampion  = other.stacksPerChampion;
    }
    return *this;
}

void MythicItem::display() const {
    Item::display();
    std::cout << "   Mythic Passive: " << mythicPassive
              << " (+" << stacksPerChampion << " per legendary)\n";
}

const std::string& MythicItem::getMythicPassive() const { return mythicPassive; }
int MythicItem::getStacksPerLegendary() const { return stacksPerChampion; }

ComponentItem::ComponentItem(const std::string& n, int gold,
                             int ad, int ap, int hp,
                             const std::string& into,
                             const std::string& desc)
    : Item(n, gold, ad, ap, hp, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, desc),
      buildsInto(into) {}

ComponentItem::ComponentItem(const ComponentItem& other)
    : Item(other), buildsInto(other.buildsInto) {}

ComponentItem& ComponentItem::operator=(const ComponentItem& other) {
    if (this != &other) {
        Item::operator=(other);
        buildsInto = other.buildsInto;
    }
    return *this;
}

void ComponentItem::display() const {
    Item::display();
    std::cout << "   Builds into: " << buildsInto << "\n";
}

const std::string& ComponentItem::getBuildsInto() const { return buildsInto; }