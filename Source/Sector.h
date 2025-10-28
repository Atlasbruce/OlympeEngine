/* Sector.h
 Represents a sector (area) inside a level. Owns a CollisionMap and a GraphicMap.
*/
#pragma once

#include "CollisionMap.h"
#include "GraphicMap.h"
#include <memory>
#include <string>
#include <iostream>
#include "system/system_utils.h"

class Sector
{
public:
 std::string name;
 std::unique_ptr<CollisionMap> collision;
 std::unique_ptr<GraphicMap> graphics;

 Sector(const std::string& n = "Unnamed") : name(n)
 {
 SYSTEM_LOG << "Sector '" << name << "' created\n";
 collision = std::make_unique<CollisionMap>();
 graphics = std::make_unique<GraphicMap>();
 }
 ~Sector()
 {
 SYSTEM_LOG << "Sector '" << name << "' destroyed\n";
 }
};
