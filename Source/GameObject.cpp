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

static std::string escape_json_string(const std::string& s)
{
    std::string out;
    out.reserve(s.size()+4);
    for (char c : s)
    {
        switch (c)
        {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

std::string GameObject::ToJSON() const
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

// Very small and permissive JSON extractors (not robust but sufficient for simple saved files)
static bool extract_json_string(const std::string& json, const std::string& key, std::string& out)
{
    size_t pos = json.find('"' + key + '"');
    if (pos == std::string::npos) pos = json.find(key);
    if (pos == std::string::npos) return false;
    pos = json.find(':', pos);
    if (pos == std::string::npos) return false;
    ++pos;
    // skip spaces
    while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
    if (pos < json.size() && json[pos] == '"') ++pos;
    size_t start = pos;
    while (pos < json.size() && json[pos] != '"' && json[pos] != '\n' && json[pos] != '\r') ++pos;
    if (pos <= start) return false;
    out = json.substr(start, pos - start);
    return true;
}

static bool extract_json_double(const std::string& json, const std::string& key, double& out)
{
    size_t pos = json.find('"' + key + '"');
    if (pos == std::string::npos) pos = json.find(key);
    if (pos == std::string::npos) return false;
    pos = json.find(':', pos);
    if (pos == std::string::npos) return false;
    ++pos;
    // skip spaces
    while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
    size_t start = pos;
    // accept digits, +, -, ., e, E
    while (pos < json.size() && (isdigit(static_cast<unsigned char>(json[pos])) || json[pos] == '+' || json[pos] == '-' || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E')) ++pos;
    if (pos <= start) return false;
    try {
        out = std::stod(json.substr(start, pos - start));
        return true;
    } catch(...) { return false; }
}

static bool extract_json_int(const std::string& json, const std::string& key, int& out)
{
    double d;
    if (!extract_json_double(json, key, d)) return false;
    out = static_cast<int>(d);
    return true;
}

static bool extract_json_bool(const std::string& json, const std::string& key, bool& out)
{
    size_t pos = json.find('"' + key + '"');
    if (pos == std::string::npos) pos = json.find(key);
    if (pos == std::string::npos) return false;
    pos = json.find(':', pos);
    if (pos == std::string::npos) return false;
    ++pos;
    while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
    if (json.compare(pos, 4, "true") == 0) { out = true; return true; }
    if (json.compare(pos, 5, "false") == 0) { out = false; return true; }
    return false;
}

bool GameObject::FromJSON(const std::string& json)
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
