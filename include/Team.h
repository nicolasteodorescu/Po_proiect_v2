#ifndef OOP_TEAM_H
#define OOP_TEAM_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <numeric>
#include "Entity.h"

class Team {
protected:
    std::string teamName;
    std::vector<std::shared_ptr<Champion>> roster;
    static const int MAX_SIZE = 5;
    static int numberTeams;

public:
    explicit Team( std::string  name );
    Team( std::string  name, std::vector<std::shared_ptr<Champion>> roster );
    Team( const Team& other );
    Team& operator=( const Team& other );
    virtual ~Team() = default;

    friend std::ostream& operator<<( std::ostream& os, const Team& t );
    friend std::istream& operator>>( std::istream& is, Team& t );

    virtual void addChampion( std::shared_ptr<Champion> champ );

    [[nodiscard]] virtual double getAverageWinRate() const;
    [[nodiscard]] double getTeamSynergy() const;

    virtual void display() const;

    const Champion& operator[]( int idx ) const;

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] bool isFull() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Champion>>& getRoster() const;
};

std::ostream& operator<<( std::ostream& os, const Team& t );
std::istream& operator>>( std::istream& is, Team& t );

class DraftTeam : public Team {
private:
    std::vector<std::string> bannedChampions;
    std::vector<std::string> pickOrder;
    int  currentPickIndex;
    bool side;

public:
    explicit DraftTeam( const std::string& name, bool redSide = false );
    DraftTeam( const DraftTeam& other );
    DraftTeam& operator=( const DraftTeam& other );
    ~DraftTeam() override = default;

    void addChampion( std::shared_ptr<Champion> champ ) override;
    void banChampion( const std::string& name );
    [[nodiscard]] bool isChampionBanned( const std::string& name ) const;
    void display() const override;

    [[nodiscard]] const std::vector<std::string>& getBans() const;
};

#endif // OOP_TEAM_H