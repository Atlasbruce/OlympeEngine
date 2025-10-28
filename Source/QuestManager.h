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
#include "system/system_utils.h"

class QuestManager : public Singleton
{
public:
 QuestManager() { SYSTEM_LOG << "QuestManager created\n"; }
 virtual ~QuestManager() { SYSTEM_LOG << "QuestManager destroyed\n"; }

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
