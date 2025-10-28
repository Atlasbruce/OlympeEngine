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
 GraphicMap() { SYSTEM_LOG << "GraphicMap created\n"; }
 ~GraphicMap() { SYSTEM_LOG << "GraphicMap destroyed\n"; }

 std::vector<std::string> elements;

 void Clear() { elements.clear(); }
};
