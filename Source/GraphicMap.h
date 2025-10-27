/* GraphicMap.h
 Simple container for graphic/decoration elements for a sector.
*/
#pragma once

#include <vector>
#include <string>
#include <iostream>

class GraphicMap
{
public:
 GraphicMap() { std::cout << "GraphicMap created\n"; }
 ~GraphicMap() { std::cout << "GraphicMap destroyed\n"; }

 std::vector<std::string> elements;

 void Clear() { elements.clear(); }
};
