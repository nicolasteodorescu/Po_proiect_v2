#ifndef OOP_ANALYSIS_H
#define OOP_ANALYSIS_H

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <list>
#include <map>
#include "Entity.h"
#include "Team.h"

template <typename T>
class RankedList {
protected:
    std::vector<std::shared_ptr<T>> items;
    std::string listName;
    bool sorted;

public:
    explicit RankedList( std::string  name ) : listName(std::move(name)), sorted(false) {}

    virtual ~RankedList() = default;

    void add( std::shared_ptr<T> item ){
        items.push_back(std::move(item));
        sorted = false;
    }

    void sortByWinRate(){
        std::sort(items.begin(), items.end(),
            []( const std::shared_ptr<T>& a, const std::shared_ptr<T>& b ){
                return a->getWinRate() > b->getWinRate();
            });
        sorted = true;
    }

    RankedList<T> filter( std::function<bool(const std::shared_ptr<T>&)> pred ) const {
        RankedList<T> result(listName + "_filtered");
        for( const auto& item : items )
            if( pred(item) )
                result.add(item);
        return result;
    }

    [[nodiscard]] std::vector<std::shared_ptr<T>> topN( int n ) const {
        RankedList<T> copy = *this;
        copy.sortByWinRate();
        int count = std::min(n, static_cast<int>(copy.items.size()));
        return std::vector<std::shared_ptr<T>>(
            copy.items.begin(), copy.items.begin() + count);
    }

    void display( int maxItems = -1 ) const {
        std::cout << "\n" << listName << "\n";
        int count = (maxItems < 0) ? static_cast<int>(items.size()) : std::min(maxItems, static_cast<int>(items.size()));
        for( int i = 0; i < count; ++i ){
            std::cout << "  " << (i + 1) << ". ";
            items[i]->display();
        }
    }

    virtual void applyRankBonus() {}

    [[nodiscard]] size_t size() const {
        return items.size();
    }
    [[nodiscard]] bool   empty() const {
        return items.empty();
    }

    std::shared_ptr<T> operator[]( size_t idx ) const {
        return items[idx];
    }

    [[nodiscard]] const std::vector<std::shared_ptr<T>>& getItems() const {
        return items;
    }
    std::vector<std::shared_ptr<T>>& getItems() {
        return items;
    }
};

class LowRank : public RankedList<Champion> {
public:
    explicit LowRank( const std::string& name ) : RankedList<Champion>(name) {}

    static double randomWin() {
        return static_cast<double>(rand() % 7) - 3.0;
    }

    void applyRankBonus() override {
        for( auto& c : items ){
            Champion::Role r = c->getRole();
            if( r == Champion::MID || r == Champion::JUNGLE || r == Champion::SUPPORT ){
                double bonus = static_cast<double>(rand() % 101) / 100.0 * 0.025;
                c->setWinRate(std::min(1.0, c->getWinRate() + bonus));
            }
        }
    }
};

class AverageRank : public RankedList<Champion> {
public:
    explicit AverageRank( const std::string& name ) : RankedList<Champion>(name) {}

    void applyRankBonus() override {
        for( auto& c : items ){
            if( c->getRole() == Champion::TOP ){
                double bonus = static_cast<double>(rand() % 101) / 100.0 * 0.0125;
                c->setWinRate(std::min(1.0, c->getWinRate() + bonus));
            }
        }
    }
};

class HighRank : public RankedList<Champion> {
    double BONUS = 0.0027;
public:
    explicit HighRank( const std::string& name ) : RankedList<Champion>(name) {}

    void applyRankBonus() override {

        for( auto& c : items ){
            Champion::Role r = c->getRole();
            if( r == Champion::MID || r == Champion::ADC )
                c->setWinRate(std::min(1.0, c->getWinRate() + BONUS));
        }
    }
};

class Analysis {
protected:
    RankedList<Champion> championPool;
    std::map<std::pair<std::string,std::string>, double> matchupData;
    std::list<std::string> analysisLog;

    Analysis();

public:
    virtual ~Analysis() = default;

    static Analysis& getInstance();

    void registerChampion( std::shared_ptr<Champion> champ );

    void   addMatchup( const std::string& c1, const std::string& c2, double wr );
    [[nodiscard]] double getMatchupWinRate( const std::string& c1, const std::string& c2 ) const;

    virtual double analyzeDraft( const Team* blue, const Team* red ) const;

    [[nodiscard]] std::shared_ptr<Champion> findChampion( const std::string& name ) const;

    [[nodiscard]] std::vector<std::shared_ptr<Champion>> recommendForRole( Champion::Role role, int top = 3 ) const;

    void printTierList() const;
    void printLog() const;

    [[nodiscard]] const RankedList<Champion>& getChampionPool() const;
};

class MinorData : public Analysis {
public:
    static MinorData& getInstance();
    double analyzeDraft( const Team* blue, const Team* red ) const override;
};

class MajorData : public Analysis {
public:
    static MajorData& getInstance();
    static std::string metaShift( const Team* team, double& bonusOut ) ;
    double analyzeDraft( const Team* blue, const Team* red ) const override;
};

class ChampionFactory {
public:
    using Role = Champion::Role;

    enum class Template {
        ASSASSIN_MAGE,
        ASSASSIN_AD,
        TANK_FIGHTER,
        CONTROL_MAGE,
        MARKSMAN,
        ENCHANTER_SUPPORT,
        TANK_SUPPORT,
        BRUISER
    };

    static std::shared_ptr<Champion> create( const std::string& name, Template tmpl );

    static std::vector<std::shared_ptr<Champion>> createBatch(
        const std::vector<std::pair<std::string, Template>>& specs );
};

#endif // OOP_ANALYSIS_H