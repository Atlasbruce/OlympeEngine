/* CollisionMap.h
 Simple class holding collision/navigation data for a sector.
*/
#pragma once

#include <vector>
#include <iostream>

class CollisionMap
{
public:
 CollisionMap() { std::cout << "CollisionMap created\n"; }
 ~CollisionMap() { std::cout << "CollisionMap destroyed\n"; }

 // Simple placeholder for collision data
 std::vector<int> data;

 void Clear() { data.clear(); }
};
