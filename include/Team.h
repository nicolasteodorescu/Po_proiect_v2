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
    explicit Team(const std::string& name);
    Team(const std::string& name, std::vector<std::shared_ptr<Champion>> roster);
    Team(const Team& other);
    Team& operator=(const Team& other);
    virtual ~Team() = default;

    friend std::ostream& operator<<(std::ostream& os, const Team& t);
    friend std::istream& operator>>(std::istream& is, Team& t);

    // throws TeamFullException if full
    virtual void addChampion(std::shared_ptr<Champion> champ);
    void removeChampion(const std::string& name);

    virtual double getAverageWinRate() const;
    double         getTeamSynergy()   const;

    virtual void display() const;

    const Champion& operator[](int idx) const;

    const std::string& getName() const;
    int  getSize() const;
    bool isFull()  const;

    const std::vector<std::shared_ptr<Champion>>& getRoster() const;
    static int getTotalTeamsCreated();
};

std::ostream& operator<<(std::ostream& os, const Team& t);
std::istream& operator>>(std::istream& is, Team& t);

class DraftTeam : public Team {
private:
    std::vector<std::string> bannedChampions;
    std::vector<std::string> pickOrder;
    int  currentPickIndex;
    bool side;

public:
    DraftTeam(const std::string& name, bool redSide = false);
    DraftTeam(const DraftTeam& other);
    DraftTeam& operator=(const DraftTeam& other);
    virtual ~DraftTeam() override = default;

    void addChampion(std::shared_ptr<Champion> champ) override;
    void banChampion(const std::string& name);
    bool isChampionBanned(const std::string& name) const;
    void display() const override;

    bool isRedSide()          const;
    int  getCurrentPickIndex() const;
    const std::vector<std::string>& getBans() const;
};

#endif // OOP_TEAM_H