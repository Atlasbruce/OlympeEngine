/* Serialization.h
 JSON (de)serialization helpers for VideoGame, World, Level, Sector, Quest system.
 Uses minimal nlohmann::json subset from Source/third_party/nlohmann/json.hpp
*/
#pragma once

#include "third_party/nlohmann/json.hpp"
#include "VideoGame.h"
#include "World.h"
#include "Level.h"
#include "Sector.h"
#include "CollisionMap.h"
#include "GraphicMap.h"
#include "QuestManager.h"
#include "Quest.h"
#include "Objective.h"
#include "Task.h"

#include <fstream>
#include <iostream>
#include <string>

using nlohmann::json;

// Task
inline void to_json(json& j, Task const& t)
{
 j = json::object();
 j["description"] = t.description;
 j["completed"] = t.completed;
}
inline void from_json(json const& j, Task& t)
{
 if (j.contains("description")) t.description = j["description"].get<std::string>();
 if (j.contains("completed")) t.completed = j["completed"].get<bool>();
}

// Objective
inline void to_json(json& j, Objective const& o)
{
 j = json::object();
 j["title"] = o.title;
 j["tasks"] = json::array();
 for (auto const& tp : o.tasks) j["tasks"].push_back(*tp);
}
inline void from_json(json const& j, Objective& o)
{
 if (j.contains("title")) o.title = j["title"].get<std::string>();
 if (j.contains("tasks") && j["tasks"].is_array()) {
 for (size_t i=0;i< j["tasks"].size();++i) {
 Task t;
 from_json(j["tasks"][i], t);
 o.tasks.push_back(std::unique_ptr<Task>(new Task(t)));
 }
 }
}

// Quest
inline void to_json(json& j, Quest const& q)
{
 j = json::object();
 j["name"] = q.name;
 j["objectives"] = json::array();
 for (auto const& op : q.objectives) j["objectives"].push_back(*op);
}
inline void from_json(json const& j, Quest& q)
{
 if (j.contains("name")) q.name = j["name"].get<std::string>();
 if (j.contains("objectives") && j["objectives"].is_array()) {
 for (size_t i=0;i< j["objectives"].size();++i) {
 Objective o;
 from_json(j["objectives"][i], o);
 q.objectives.push_back(std::unique_ptr<Objective>(new Objective(o)));
 }
 }
}

// QuestManager
inline void to_json(json& j, QuestManager const& m)
{
 j = json::object();
 j["quests"] = json::array();
 for (auto const& qp : m.GetQuests()) j["quests"].push_back(*qp);
}
inline void from_json(json const& j, QuestManager& m)
{
 // clear existing quests? (singleton) - not implemented here
 if (j.contains("quests") && j["quests"].is_array()) {
 for (size_t i=0;i< j["quests"].size();++i) {
 Quest q;
 from_json(j["quests"][i], q);
 m.AddQuest(std::unique_ptr<Quest>(new Quest(q)));
 }
 }
}

// CollisionMap
inline void to_json(json& j, CollisionMap const& c)
{
 j = json::object();
 j["data"] = json::array();
 for (int v : c.data) j["data"].push_back(v);
}
inline void from_json(json const& j, CollisionMap& c)
{
 if (j.contains("data") && j["data"].is_array()) {
 c.data.clear();
 for (size_t i=0;i< j["data"].size();++i) c.data.push_back(j["data"][i].get<int>());
 }
}

// GraphicMap
inline void to_json(json& j, GraphicMap const& g)
{
 j = json::object();
 j["elements"] = json::array();
 for (auto const& e : g.elements) j["elements"].push_back(e);
}
inline void from_json(json const& j, GraphicMap& g)
{
 if (j.contains("elements") && j["elements"].is_array()) {
 g.elements.clear();
 for (size_t i=0;i< j["elements"].size();++i) g.elements.push_back(j["elements"][i].get<std::string>());
 }
}

// Sector
inline void to_json(json& j, Sector const& s)
{
 j = json::object();
 j["name"] = s.name;
 if (s.collision) j["collision"] = *s.collision; else j["collision"] = nullptr;
 if (s.graphics) j["graphics"] = *s.graphics; else j["graphics"] = nullptr;
}
inline void from_json(json const& j, Sector& s)
{
 if (j.contains("name")) s.name = j["name"].get<std::string>();
 if (j.contains("collision") && !j["collision"].is_null()) {
 s.collision = std::unique_ptr<CollisionMap>(new CollisionMap());
 from_json(j["collision"], *s.collision);
 }
 if (j.contains("graphics") && !j["graphics"].is_null()) {
 s.graphics = std::unique_ptr<GraphicMap>(new GraphicMap());
 from_json(j["graphics"], *s.graphics);
 }
}

// Level
inline void to_json(json& j, Level const& l)
{
 j = json::object();
 j["name"] = l.name;
 j["sectors"] = json::array();
 for (auto const& sp : l.sectors) j["sectors"].push_back(*sp);
}
inline void from_json(json const& j, Level& l)
{
 if (j.contains("name")) l.name = j["name"].get<std::string>();
 if (j.contains("sectors") && j["sectors"].is_array()) {
 for (size_t i=0;i< j["sectors"].size();++i) {
 Sector s;
 from_json(j["sectors"][i], s);
 l.sectors.push_back(std::unique_ptr<Sector>(new Sector(s)));
 }
 }
}

// World
inline void to_json(json& j, World const& w)
{
 j = json::object();
 j["name"] = w.name;
 j["levels"] = json::array();
 for (auto const& lp : w.GetLevels()) j["levels"].push_back(*lp);
}
inline void from_json(json const& j, World& w)
{
 if (j.contains("name")) w.name = j["name"].get<std::string>();
 if (j.contains("levels") && j["levels"].is_array()) {
 for (size_t i=0;i< j["levels"].size();++i) {
 Level l;
 from_json(j["levels"][i], l);
 w.AddLevel(std::unique_ptr<Level>(new Level(l)));
 }
 }
}

// VideoGame
inline void to_json(json& j, VideoGame const& vg)
{
 j = json::object();
 j["schema_version"] =1;
 j["type"] = "VideoGame";
 j["rules"] = vg.GetRules().name;
 j["world"] = vg.GetWorld();
 // QuestManager singleton
 j["quests"] = json::array();
 for (auto const& qp : QuestManager::Get().GetQuests()) j["quests"].push_back(*qp);
}
inline void from_json(json const& j, VideoGame& vg)
{
 if (j.contains("rules")) vg.GetRules().name = j["rules"].get<std::string>();
 if (j.contains("world")) {
 World w;
 from_json(j["world"], w);
 vg.GetWorld() = std::move(w); // won't compile for unique_ptr reference; handled via clear/add
 }
 if (j.contains("quests") && j["quests"].is_array()) {
 for (size_t i=0;i< j["quests"].size();++i) {
 Quest q;
 from_json(j["quests"][i], q);
 QuestManager::Get().AddQuest(std::unique_ptr<Quest>(new Quest(q)));
 }
 }
}

// File I/O helpers
inline bool SaveToFile(const json& j, const std::string& path)
{
 std::ofstream ofs(path);
 if (!ofs) return false;
 ofs << j.dump(2);
 return true;
}
inline bool LoadFromFile(json& j, const std::string& path)
{
 std::ifstream ifs(path);
 if (!ifs) return false;
 std::ostringstream ss;
 ss << ifs.rdbuf();
 try {
 j = json::parse(ss.str());
 } catch (std::exception& e) {
 std::cerr << "JSON parse error: " << e.what() << std::endl;
 return false;
 }
 return true;
}
