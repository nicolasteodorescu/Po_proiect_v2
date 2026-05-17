#ifndef OOP_ENTITY_H
#define OOP_ENTITY_H

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

class GameException : public std::exception {
protected:
    std::string msg;
public:
    explicit GameException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

class ChampionNotFoundException : public GameException {
public:
    explicit ChampionNotFoundException(const std::string& name)
        : GameException("Champion not found: " + name) {}
};

class InvalidWinRateException : public GameException {
public:
    explicit InvalidWinRateException(double wr)
        : GameException("Invalid win rate: " + std::to_string(wr)) {}
};

class TeamFullException : public GameException {
public:
    explicit TeamFullException(const std::string& team)
        : GameException("Team is full: " + team) {}
};

class Entity {
protected:
    std::string name;
    std::string description;
    static int  instCnt;
public:
    explicit Entity(const std::string& name, const std::string& desc = "");
    Entity(const Entity& other);
    Entity& operator=(const Entity& other);
    virtual ~Entity();

    friend std::ostream& operator<<(std::ostream& os, const Entity& e);
    friend std::istream& operator>>(std::istream& is, Entity& e);

    virtual double getWinRate() const = 0;
    virtual std::string getType() const = 0;
    virtual void display() const;

    const std::string& getName() const;
    const std::string& getDescription() const;

    static int  getLiveInstances();
    static void resetInstanceCount();

    bool operator<(const Entity& other) const;
    bool operator==(const Entity& other) const;
};

std::ostream& operator<<(std::ostream& os, const Entity& e);
std::istream& operator>>(std::istream& is, Entity& e);

class Champion : public Entity {
private:
    double winRate;
    double pickRate;
    double banRate;
    std::map<std::string, double> synergyScores;
    static std::string topChampionName;

public:
    enum Role { TOP, JUNGLE, MID, ADC, SUPPORT, NONE };
    Role role;

    static std::string roleToString(Role r);

    Champion(const std::string& name, Role role,
             double winRate, double pickRate, double banRate,
             const std::string& desc = "");
    Champion(const std::string& name, Role role, double winRate);

    Champion(const Champion& other);
    Champion& operator=(const Champion& other);
    virtual ~Champion() override = default;

    void setWinRate(double wr);
    double getWinRate() const override;
    std::string getType() const override;
    void display() const override;

    Champion operator+(const Champion& other) const;

    void addSynergy(const std::string& champName, double score);
    double getSynergyWith(const std::string& champName) const;

    Role getRole() const;
    double getPickRate() const;
    double getBanRate() const;

    static const std::string& getTopChampionName();
};

class Item : public Entity {
protected:
    int    goldCost, attackDamage, abilityPower, health,
           armor, magicResist, abilityHaste, movementSpeed, criticalStrike;
    double armorPen, magicPen, attackSpeed, lifeSteal;
    double winRateStats;

public:
    Item(const std::string& name, int gold,
         int ad, int ap, int hp, int ar, int mr,
         int ah, int ms, int cs,
         double arp, double mrp, double as, double ls,
         double winRate, const std::string& desc = "");
    explicit Item(const std::string& name, int gold);

    Item(const Item& other);
    Item& operator=(const Item& other);
    virtual ~Item() override = default;

    double getWinRate() const override { return 0.5 + winRateStats; }
    std::string getType() const override { return "Item"; }
    void  display() const override;

    int getGoldCost() const;
    int getAD() const;
    int getAP() const;
    int getHP() const;
    int getAR() const;
    int getMR() const;
    int getAH() const;
    int getMS() const;
    int getCS() const;
    double getARP() const;
    double getMRP() const;
    double getAS() const;
    double getLS() const;
    double getWinRateImpact() const;

    int operator-(const Item& other) const;
};

class MythicItem : public Item {
private:
    std::string mythicPassive;
    int  stacksPerChampion;
public:
    MythicItem(const std::string& name, int gold,
               int ad, int ap, int hp, int ar, int mr,
               int ah, int ms, int cs,
               double arp, double mrp, double as, double ls,
               double winRate,
               const std::string& passive, int stacks,
               const std::string& desc = "");
    MythicItem(const MythicItem& other);
    MythicItem& operator=(const MythicItem& other);
    virtual ~MythicItem() override = default;

    std::string getType() const override {
        return "MythicItem";
    }
    void display() const override;

    const std::string& getMythicPassive() const;
    int getStacksPerLegendary() const;
};

class ComponentItem : public Item {
private:
    std::string buildsInto;
public:
    ComponentItem(const std::string& name, int gold,
                  int ad, int ap, int hp,
                  const std::string& buildsInto,
                  const std::string& desc = "");
    ComponentItem(const ComponentItem& other);
    ComponentItem& operator=(const ComponentItem& other);
    virtual ~ComponentItem() override = default;

    std::string getType() const override { return "ComponentItem"; }
    void display() const override;

    const std::string& getBuildsInto() const;
};

#endif // OOP_ENTITY_H