/* QuestManager.h
 Manages quests. Derives from Singleton.
*/
#pragma once

#include "Singleton.h"
#include "Quest.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class QuestManager : public Singleton
{
public:
 QuestManager() { std::cout << "QuestManager created\n"; }
 virtual ~QuestManager() { std::cout << "QuestManager destroyed\n"; }

 static QuestManager& GetInstance()
 {
 static QuestManager instance;
 return instance;
 }
 static QuestManager& Get() { return GetInstance(); }

 void AddQuest(std::unique_ptr<Quest> q)
 {
 quests.push_back(std::move(q));
 }

 const std::vector<std::unique_ptr<Quest>>& GetQuests() const { return quests; }

private:
 std::vector<std::unique_ptr<Quest>> quests;
};
