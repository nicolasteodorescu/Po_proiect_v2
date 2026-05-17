#ifndef OOP_ANALYSIS_H
#define OOP_ANALYSIS_H

#include <iostream>
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
private:
    std::vector<std::shared_ptr<T>> items;
    std::string listName;
    bool sorted;
public:
    explicit RankedList(const std::string& name)
        : listName(name), sorted(false) {}

    void add(std::shared_ptr<T> item) {
        items.push_back(item);
        sorted = false;
    }

    void sortByWinRate() {
        std::sort(items.begin(), items.end(),
            [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b){
                return a->getWinRate() > b->getWinRate();
            });
        sorted = true;
    }

    RankedList<T> filter(std::function<bool(const std::shared_ptr<T>&)> pred) const {
        RankedList<T> result(listName + "_filtered");
        for (const auto& item : items)
            if (pred(item)) result.add(item);
        return result;
    }

    std::vector<std::shared_ptr<T>> topN(int n) const {
        RankedList<T> copy = *this;
        copy.sortByWinRate();
        int count = std::min(n, static_cast<int>(copy.items.size()));
        return std::vector<std::shared_ptr<T>>(
            copy.items.begin(), copy.items.begin() + count);
    }

    void display(int maxItems = -1) const {
        std::cout << "\n" << listName << "\n";
        int count = (maxItems < 0)
            ? static_cast<int>(items.size())
            : std::min(maxItems, static_cast<int>(items.size()));
        for (int i = 0; i < count; ++i) {
            std::cout << "  " << (i + 1) << ". ";
            items[i]->display();
        }
    }

    size_t size()  const { return items.size(); }
    bool   empty() const { return items.empty(); }

    std::shared_ptr<T> operator[](size_t idx) const { return items[idx]; }

    const std::vector<std::shared_ptr<T>>& getItems() const { return items; }
          std::vector<std::shared_ptr<T>>& getItems()       { return items; }
};

class Analysis {
private:
    RankedList<Champion> championPool;
    RankedList<Item> itemPool;
    std::map<std::pair<std::string,std::string>, double> matchupData;
    std::list<std::string> analysisLog;

    Analysis();
    Analysis(const Analysis&) = delete;
    Analysis& operator=(const Analysis&) = delete;

public:
    static Analysis& getInstance();

    void registerChampion(std::shared_ptr<Champion> champ);
    void registerItem(std::shared_ptr<Item> item);

    void   addMatchup(const std::string& c1, const std::string& c2, double wr);
    double getMatchupWinRate(const std::string& c1, const std::string& c2) const;

    double analyzeDraft(const Team* blue, const Team* red) const;

    std::shared_ptr<Champion> findChampion(const std::string& name) const;

    std::vector<std::shared_ptr<Champion>>
        recommendForRole(Champion::Role role, int top = 3) const;

    void printTierList() const;
    void printLog() const;

    const RankedList<Champion>& getChampionPool() const;
    const RankedList<Item>& getItemPool() const;
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

    static std::shared_ptr<Champion> create(const std::string& name, Template tmpl);

    static std::vector<std::shared_ptr<Champion>> createBatch(
        const std::vector<std::pair<std::string, Template>>& specs);
};

#endif // OOP_ANALYSIS_H