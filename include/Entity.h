#ifndef OOP_ENTITY_H
#define OOP_ENTITY_H

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
#include <utility>

class GameException : public std::exception {
protected:
    std::string msg;
public:
    explicit GameException( std::string  m ) : msg(std::move(m)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }
};

class ChampionNotFoundException : public GameException {
public:
    explicit ChampionNotFoundException( const std::string& name )
        : GameException("Champion not found: " + name) {}
};

class InvalidWinRateException : public GameException {
public:
    explicit InvalidWinRateException( double wr )
        : GameException("Invalid win rate: " + std::to_string(wr)) {}
};

class TeamFullException : public GameException {
public:
    explicit TeamFullException( const std::string& team )
        : GameException("Team is full: " + team) {}
};

class Entity {
protected:
    std::string name;
    std::string description;
    static int instCnt;

public:
    explicit Entity( std::string name, std::string desc = "" );
    Entity( const Entity& other );
    Entity& operator=( const Entity& other );
    virtual ~Entity();

    friend std::ostream& operator<<( std::ostream& os, const Entity& e );
    friend std::istream& operator>>( std::istream& is, Entity& e );

    [[nodiscard]] virtual double getWinRate() const = 0;
    [[nodiscard]] virtual std::string getType() const = 0;
    virtual void display() const;

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] const std::string& getDescription() const;

    static int  getLiveInstances();
    static void resetInstanceCount();

    bool operator<( const Entity& other ) const;
    bool operator==( const Entity& other ) const;
};

std::ostream& operator<<( std::ostream& os, const Entity& e );
std::istream& operator>>( std::istream& is, Entity& e );

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

    static std::string roleToString( Role r );

    Champion( const std::string& name, Role role,
              double winRate, double pickRate, double banRate,
              const std::string& desc = "" );
    Champion( const std::string& name, Role role, double winRate );

    Champion( const Champion& other );
    Champion& operator=( const Champion& other );
    ~Champion() override = default;

    void        setWinRate( double wr );
    [[nodiscard]] double getWinRate() const override;
    [[nodiscard]] std::string getType() const override;
    void display() const override;

    Champion operator+( const Champion& other ) const;

    void addSynergy( const std::string& champName, double score );
    [[nodiscard]] double getSynergyWith( const std::string& champName ) const;

    [[nodiscard]] Role getRole() const;
    [[nodiscard]] double getPickRate() const;
    [[nodiscard]] double getBanRate() const;

    static const std::string& getTopChampionName();
};

#endif // OOP_ENTITY_H