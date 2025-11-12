#include "GameObject.h"
#include "Factory.h"
#include "system/system_utils.h"
#include <sstream>
#include <iomanip>

//for tests
//#include <SDL3/SDL_render.h>
#include "GameEngine.h"
//extern SDL_Renderer* renderer;

bool GameObject::FactoryRegistered = Factory::Get().Register("GameObject", Create); // []() { return new GameObject(); });

Object* GameObject::Create()
{
	return new GameObject();
}

std::string GameObject::Save() const
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6);
    ss << "{\n";
    ss << "  \"uid\": " << GetUID() << ",\n";
    ss << "  \"name\": \"" << escape_json_string(name) << "\",\n";
    ss << "  \"className\": \"GameObject\",\n";
    ss << "  \"entityType\": " << static_cast<int>(GetEntityType()) << ",\n";
    ss << "  \"position\": { \"x\": " << position.x << ", \"y\": " << position.y << " },\n";
    ss << "  \"width\": " << width << ",\n";
    ss << "  \"height\": " << height << ",\n";
    ss << "  \"isDynamic\": " << (isDynamic ? "true" : "false") << "\n";
    ss << "}";
    return ss.str();
}

bool GameObject::Load(const std::string& json)
{
    std::string s;
    if (extract_json_string(json, "name", s)) name = s;

    double px=0, py=0;
    // position.x
    if (extract_json_double(json, "position\"[ \"x\"]", px)) { position.x = static_cast<float>(px); }
    // fallback: search for "x": inside position block
    size_t pos = json.find("\"position\"");
    if (pos != std::string::npos)
    {
        size_t brace = json.find('{', pos);
        if (brace != std::string::npos)
        {
            size_t endb = json.find('}', brace);
            if (endb != std::string::npos)
            {
                std::string block = json.substr(brace, endb - brace + 1);
                double x=0,y=0;
                if (extract_json_double(block, "x", x)) position.x = static_cast<float>(x);
                if (extract_json_double(block, "y", y)) position.y = static_cast<float>(y);
            }
        }
    }
    // width / height
    double w=0,h=0; if (extract_json_double(json, "width", w)) width = static_cast<float>(w); if (extract_json_double(json, "height", h)) height = static_cast<float>(h);
    // isDynamic
    bool b=false; if (extract_json_bool(json, "isDynamic", b)) isDynamic = b;
    // entity type
    int et=0; if (extract_json_int(json, "entityType", et)) {
        // best-effort mapping
        if (et >= 0 && et < static_cast<int>(EntityType::Count)) {
            // we ignore setting specific derived type - keep as base GameObject
        }
    }

    return true;
}
