#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include "json.hpp"
using json = nlohmann::json;

namespace NJ 
{
    std::string GetJsonStrValue(const json& json, const std::string& key)
    {
        return json[key].get<std::string>();
    }

    int GetJsonIntValue(const json& json, const std::string& key)
    {
        return json[key].get<int>();
    }

    float GetJsonFloatValue(const json& json, const std::string& key)
    {
        return json[key].get<float>();
    }

    double GetJsonDoubleValue(const json& json, const std::string& key)
    {
        return json[key].get<double>();
    }

    bool GetJsonBoolValue(const json& json, const std::string& key)
    {
        return json[key].get<bool>();
    }

    json GetJsonObj(const json& json, const std::string& key)
    {
        return json[key];
    }

    template<typename T>
    bool IsJsonValueEqual(const json& json, const std::string& key, const T& v)
    {
        return json[key].get<T>() == v;
    }

    bool HasJsonKey(const json& json, const std::string& key)
    {
        return json.find(key) != json.end();
    }
}

#endif // 
