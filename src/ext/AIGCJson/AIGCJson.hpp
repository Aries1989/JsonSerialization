/**
 * Copyright (C) 2020 - 2021, Yaronzz(yaronhuang@foxmail.com). All rights reserved.
 * 
 * Licensed under the MIT License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 * 
 * http://opensource.org/licenses/MIT
 * 
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 * 
 * @author: yaronzz
 * @email:  yaronhuang@foxmail.com
 * @github: https://github.com/yaronzz
 * @note:   Support type -->> int、uint、int64、uint64、bool、float、double、string、vector、list、map<string,XX>
 * 
 */
#pragma once
#include <set>
#include <list>
#include <map>
#include <array>
#include <vector>
#include <string>
#include <iostream>

#define UN_USED(V) (void)(V)

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace aigc
{

/******************************************************
 * Register class or struct members
 * eg:
 * struct Test
 * {
 *      string A;
 *      string B;
 *      AIGC_JSON_HELPER(A, B)
 * };         
 * 反序列化
 * jsonValue：最后返回的json对象
 * names: 表示使用AIGC_JSON_HELPER_RENAME宏，重命名后的成员变量名
 * standardNames：表示将成员变量名称作为name的情况
 ******************************************************/
#define AIGC_JSON_HELPER(...)                                                            \
    std::map<std::string, std::string> __aigcDefaultValues;                              \
    bool AIGCJsonToObject(aigc::JsonHelperPrivate &handle,                               \
                          json& j,                                                       \
                          std::vector<std::string> &names)                               \
    {                                                                                    \
        std::vector<std::string> standardNames = handle.GetMembersNames(#__VA_ARGS__);   \
        if (names.size() <= standardNames.size())                                        \
        {                                                                                \
            for (size_t i = names.size(); i < standardNames.size(); i++)               \
                names.push_back(standardNames[i]);                                       \
        }                                                                                \
        return handle.SetMembers(names, 0, j, __aigcDefaultValues, __VA_ARGS__);         \
    }                                                                                    \
    bool AIGCObjectToJson(aigc::JsonHelperPrivate &handle,                               \
                          json& j,                                                       \
                          std::vector<std::string> &names)                               \
    {                                                                                    \
        std::vector<std::string> standardNames = handle.GetMembersNames(#__VA_ARGS__);   \
        if (names.size() <= standardNames.size())                                        \
        {                                                                                \
            for (size_t i = names.size(); i < standardNames.size(); i++)               \
                names.push_back(standardNames[i]);                                       \
        }                                                                                \
        return handle.GetMembers(names, 0, j, __VA_ARGS__);                              \
    }

/******************************************************
 * Rename members
 * eg:
 * struct Test
 * {
 *      string A;
 *      string B;
 *      AIGC_JSON_HELPER(A, B)
 *      AIGC_JSON_HELPER_RENAME("a", "b")
 * };         
 ******************************************************/
#define AIGC_JSON_HELPER_RENAME(...)                                            \
    std::vector<std::string> AIGCRenameMembers(aigc::JsonHelperPrivate &handle) \
    {                                                                           \
        return handle.GetMembersNames(#__VA_ARGS__);                            \
    }

/******************************************************
 * Register base-class
 * eg:
 * struct Base
 * {
 *      string name;
 *      AIGC_JSON_HELPER(name)
 * };  
 * struct Test : Base
 * {
 *      string A;
 *      string B;
 *      AIGC_JSON_HELPER(A, B)
 *      AIGC_JSON_HELPER_BASE((Base*)this)
 * };         
 ******************************************************/
#define AIGC_JSON_HELPER_BASE(...)                                           \
    bool AIGCBaseJsonToObject(aigc::JsonHelperPrivate &handle, json& j)      \
    {                                                                        \
        return handle.SetBase(j, __VA_ARGS__);                               \
    }                                                                        \
    bool AIGCBaseObjectToJson(aigc::JsonHelperPrivate &handle, json& j)      \
    {                                                                        \
        return handle.GetBase(j, __VA_ARGS__);                               \
    }

/******************************************************
 * Set default value
 * eg:
 * struct Base
 * {
 *      string name;
 *      int age;
 *      AIGC_JSON_HELPER(name, age)
 *      AIGC_JSON_HELPER_DEFAULT(age=18)
 * };  
 ******************************************************/
#define AIGC_JSON_HELPER_DEFAULT(...)                                  \
    void AIGCDefaultValues(aigc::JsonHelperPrivate &handle)            \
    {                                                                  \
        __aigcDefaultValues = handle.GetMembersValueMap(#__VA_ARGS__); \
    }


class JsonHelperPrivate
{
public:
    /******************************************************
         *
         * enable_if
         *
         ******************************************************/
    // 主模版
    template <bool, class TYPE = void>
    struct enable_if
    {
    };

    // 偏特化模版
    template <class TYPE>
    struct enable_if<true, TYPE>
    {
        typedef TYPE type;
    };

public:
    /******************************************************
         * 
         * Check Interface
         *      If class or struct add AIGC_JSON_HELPER\AIGC_JSON_HELPER_RENAME\AIGC_JSON_HELPER_BASE,
         *      it will go to the correct conver function.
         *
         ******************************************************/
    template <typename T>
    struct HasConverFunction
    {
        template <typename TT>
        static char func(decltype(&TT::AIGCJsonToObject));

        template <typename TT>
        static int func(...);

        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T>
    struct HasRenameFunction
    {
        template <typename TT>
        static char func(decltype(&TT::AIGCRenameMembers));
        template <typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T>
    struct HasBaseConverFunction
    {
        template <typename TT>
        static char func(decltype(&TT::AIGCBaseJsonToObject));
        template <typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T>
    struct HasDefaultValueFunction
    {
        template <typename TT>
        static char func(decltype(&TT::AIGCDefaultValues));
        template <typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

public:
    /******************************************************
         *
         * Interface of JsonToObject\ObjectToJson
         *
         ******************************************************/
    // 反序列化
    template <typename T, typename enable_if<HasConverFunction<T>::has, int>::type = 0>
    bool JsonToObject(T &obj, json& j)
    {
        if (!BaseConverJsonToObject(obj, j))
            return false;

        LoadDefaultValuesMap(obj);
        std::vector<std::string> names = LoadRenameArray(obj);
        return obj.AIGCJsonToObject(*this, j, names);
    }

    template <typename T, typename enable_if<!HasConverFunction<T>::has, int>::type = 0>
    bool JsonToObject(T &obj, json& j)
    {
        if (std::is_enum<T>::value)
        {
            int ivalue;
            if (!JsonToObject(ivalue, j))
                return false;

            obj = static_cast<T>(ivalue);
            return true;
        }

        m_message = "unsupported this type.";
        return false;
    }

    // 序列化
    template <typename T, typename enable_if<HasConverFunction<T>::has, int>::type = 0>
    bool ObjectToJson(T &obj, json& j)
    {
        //if (j.is_null())
        //{
        //    j = json();
        //}

        // 基类序列化
        if (!BaseConverObjectToJson(obj, j))
            return false;

        std::vector<std::string> names = LoadRenameArray(obj);
        // 类对象obj的序列化
        return obj.AIGCObjectToJson(*this, j, names);
    }

    // 枚举类型的序列化进行单独处理
    // 先转为int，再进行序列化
    template <typename T, typename enable_if<!HasConverFunction<T>::has, int>::type = 0>
    bool ObjectToJson(T &obj, json& j)
    {
        if (std::is_enum<T>::value)
        {
            int ivalue = static_cast<int>(obj);
            return ObjectToJson(ivalue, j);
        }

        m_message = "unsupported this type.";
        return false;
    }

    /******************************************************
         *
         * Interface of LoadRenameArray
         *
         ******************************************************/
    // 获取重命名的成员名称
    template <typename T, typename enable_if<HasRenameFunction<T>::has, int>::type = 0>
    std::vector<std::string> LoadRenameArray(T &obj)
    {
        return obj.AIGCRenameMembers(*this);
    }

    template <typename T, typename enable_if<!HasRenameFunction<T>::has, int>::type = 0>
    std::vector<std::string> LoadRenameArray(T &obj)
    {
        UN_USED(obj);

        return std::vector<std::string>();
    }

    /******************************************************
         *
         * Interface of BaseConverJsonToObject\BaseConverObjectToJson
         *
         ******************************************************/
    // 基类反序列化
    // 过滤HasBaseConverFunction的对象
    template <typename T, typename enable_if<HasBaseConverFunction<T>::has, int>::type = 0>
    bool BaseConverJsonToObject(T &obj, json& j)
    {
        return obj.AIGCBaseJsonToObject(*this, j);
    }

    // 过滤!HasBaseConverFunction的对象,直接返回true
    template <typename T, typename enable_if<!HasBaseConverFunction<T>::has, int>::type = 0>
    bool BaseConverJsonToObject(T &obj, json& j)
    {
        UN_USED(j);
        UN_USED(obj);

        return true;
    }

    // 基类序列化
    template <typename T, typename enable_if<HasBaseConverFunction<T>::has, int>::type = 0>
    bool BaseConverObjectToJson(T &obj, json& j)
    {
        return obj.AIGCBaseObjectToJson(*this, j);
    }

    // 过滤!HasBaseConverFunction的对象,直接返回true
    template <typename T, typename enable_if<!HasBaseConverFunction<T>::has, int>::type = 0>
    bool BaseConverObjectToJson(T &obj, json& j)
    {
        UN_USED(j);
        UN_USED(obj);

        return true;
    }

    /******************************************************
         *
         * Interface of Default value
         *
         ******************************************************/
    // 获取默认key-value映射
    // 过滤HasDefaultValueFunction的对象
    template <typename T, typename enable_if<HasDefaultValueFunction<T>::has, int>::type = 0>
    void LoadDefaultValuesMap(T &obj)
    {
        obj.AIGCDefaultValues(*this);
    }

    // 过滤!HasDefaultValueFunction的对象，直接返回true
    template <typename T, typename enable_if<!HasDefaultValueFunction<T>::has, int>::type = 0>
    void LoadDefaultValuesMap(T &obj)
    {
        (void)obj;
    }

public:
    /******************************************************
        *
        * Tool function
        *
        ******************************************************/
    // 字符串分割
    static std::vector<std::string> StringSplit(const std::string &str, char sep = ',')
    {
        std::vector<std::string> array;
        std::string::size_type pos1, pos2;
        pos1 = 0;
        pos2 = str.find(sep);
        while (std::string::npos != pos2)
        {
            array.push_back(str.substr(pos1, pos2 - pos1));
            pos1 = pos2 + 1;
            pos2 = str.find(sep, pos1);
        }
        if (pos1 != str.length())
            array.push_back(str.substr(pos1));

        return array;
    }

    // 字符串去空格
    static std::string StringTrim(std::string key)
    {
        std::string newStr = key;
        if (!newStr.empty())
        {
            newStr.erase(0, newStr.find_first_not_of(" "));
            newStr.erase(newStr.find_last_not_of(" ") + 1);
        }
        if (!newStr.empty())
        {
            newStr.erase(0, newStr.find_first_not_of("\""));
            newStr.erase(newStr.find_last_not_of("\"") + 1);
        }
        return newStr;
    }

    static void StringTrim(std::vector<std::string> &array)
    {
        for (int i = 0; i < (int)array.size(); i++)
        {
            array[i] = StringTrim(array[i]);
        }
    }

    /**
         * Get json value type
         */
    static std::string GetJsonValueTypeName(json& j)
    {
        return j.type_name();
    }

    static std::string GetStringFromJsonValue(json& j)
    {
        //rapidjson::StringBuffer buffer;
        //rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        //jsonValue.Accept(writer);
        //std::string ret = std::string(buffer.GetString());
        //return ret;

        return j.dump();
    }

    static std::string FindStringFromMap(std::string name, 
        std::map<std::string, std::string> &stringMap)
    {
        std::map<std::string, std::string>::iterator iter = stringMap.find(name);
        if (iter == stringMap.end())
            return "";
        return iter->second;
    }

public:
    /******************************************************
        *
        * Set class/struct members value
        *
        ******************************************************/
    std::vector<std::string> GetMembersNames(const std::string membersStr)
    {
        std::vector<std::string> array = StringSplit(membersStr);
        StringTrim(array);
        return array;
    }

    std::map<std::string, std::string> GetMembersValueMap(const std::string valueStr)
    {
        std::vector<std::string> array = StringSplit(valueStr);
        std::map<std::string, std::string> ret;
        for (int i = 0; i < array.size(); i++)
        {
            std::vector<std::string> keyValue = StringSplit(array[i], '=');
            if (keyValue.size() != 2)
                continue;

            std::string key = StringTrim(keyValue[0]);
            std::string value = StringTrim(keyValue[1]);
            if (ret.find(key) != ret.end())
                continue;
            ret.insert(std::pair<std::string, std::string>(key, value));
        }
        return ret;
    }

    template <typename TYPE, typename... TYPES>
    bool SetMembers(const std::vector<std::string> &names, int index, 
        json &j, std::map<std::string, std::string> defaultValues, 
        TYPE &arg, TYPES &...args)
    {
        if (!SetMembers(names, index, j, defaultValues, arg))
            return false;

        // 递归调用自己
        return SetMembers(names, ++index, j, defaultValues, args...);
    }

    template <typename TYPE>
    bool SetMembers(const std::vector<std::string> &names, int index,
       json& j, std::map<std::string, std::string> defaultValues, TYPE &arg)
    {
        if (j.is_null())
            return true;

        const char *key = names[index].c_str();
        if (!j.is_object())
            return false;

        // 提前判断是否有某个key
        // 1、防止直接解析导致崩溃
        // 2、可以读取默认值
        auto it = j.find(key);
        if (it == j.end())
        {
            std::string defaultV = FindStringFromMap(names[index], defaultValues);
            if (!defaultV.empty())
                StringToObject(arg, defaultV);
            return true;
        }

        // 基于arg的具体类型，调用相应的JsonToObject，实现具体参数的反序列化
        if (!JsonToObject(arg, *it))
        {
            m_message = "[" + names[index] + "] " + m_message;
            return false;
        }
        return true;
    }

    /******************************************************
        *
        * Get class/struct members value
        *
        ******************************************************/
    template <typename TYPE, typename... TYPES>
    bool GetMembers(const std::vector<std::string> &names, int index, 
        json &j, TYPE &arg, TYPES &...args)
    {
        // index对应的字段参数的序列化
        if (!GetMembers(names, index, j, arg))
            return false;

        // 递归调用自己
        return GetMembers(names, ++index, j, args...);
    }

    template <typename TYPE>
    bool GetMembers(const std::vector<std::string> &names, int index,
        json &j, TYPE &arg)
    {
        json jv;
        // 基于arg的具体类型，调用相应的ObjectToJson，实现具体参数的序列化
        bool check = ObjectToJson(arg, jv);
        if (!check)
        {
            m_message = "[" + names[index] + "] " + m_message;
            return false;
        }

        auto it = j.find(names[index]);
        if (it != j.end())
        {
            auto next = it + 1;
            j.erase(it, next);
        }

        j[names[index]] = jv;
        return true;
    }

public:
    /******************************************************
        *
        * Set base class value
        *
        ******************************************************/
    // 基类反序列化
    template <typename TYPE, typename... TYPES>
    bool SetBase(json& j, TYPE *arg, TYPES *...args)
    {
        if (!SetBase(j, arg))
            return false;
        return SetBase(j, args...);
    }

    template <typename TYPE>
    bool SetBase(json& j, TYPE *arg)
    {
        return JsonToObject(*arg, j);
    }

    /******************************************************
        *
        * Get base class value
        *
        ******************************************************/
    // 基类序列化
    template <typename TYPE, typename... TYPES>
    bool GetBase(json& j, TYPE *arg, TYPES *...args)
    {
        if (!GetBase(j, arg))
            return false;
        return GetBase(j, args...);
    }

    template <typename TYPE>
    bool GetBase(json& j, TYPE *arg)
    {
        return ObjectToJson(*arg, j);
    }

public:
    /******************************************************
         * Conver base-type : string to base-type
         * Contain: int\uint、int64_t\uint64_t、bool、float
         *          double、string
         *
         ******************************************************/
    template <typename TYPE>
    void StringToObject(TYPE &obj, std::string &value)
    {
        UN_USED(value);
        UN_USED(obj);

        return;
    }

    void StringToObject(std::string &obj, std::string &value)
    {
        obj = value;
    }

    void StringToObject(int &obj, std::string &value)
    {
        obj = atoi(value.c_str());
    }

    void StringToObject(unsigned int &obj, std::string &value)
    {
        char *end;
        obj = strtoul(value.c_str(), &end, 10);
    }

    void StringToObject(int64_t &obj, std::string &value)
    {
        char *end;
        obj = strtoll(value.c_str(), &end, 10);
    }

    void StringToObject(uint64_t &obj, std::string &value)
    {
        char *end;
        obj = strtoull(value.c_str(), &end, 10);
    }

    void StringToObject(bool &obj, std::string &value)
    {
        obj = (value == "true");
    }

    void StringToObject(float &obj, std::string &value)
    {
        obj = (float)atof(value.c_str());
    }

    void StringToObject(double &obj, std::string &value)
    {
        obj = atof(value.c_str());
    }

public:
    /******************************************************
         * Conver base-type : Json string to base-type
         * Contain: int\uint、int64_t\uint64_t、bool、float
         *          double、string、vector、list、map<string,XX>
         *
         ******************************************************/
    bool JsonToObject(int &obj, json& j)
    {
        if (!j.is_number_integer())
        {
            
            m_message = "json-value is " + std::string(j.type_name()) + " but object is int.";
            return false;
        }
        obj = j.get<int>();
        return true;
    }

    bool JsonToObject(unsigned int &obj, json& j)
    {
        if (!j.is_number_unsigned())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is unsigned int.";
            return false;
        }
        obj = j.get<unsigned int>();
        return true;
    }

    bool JsonToObject(int64_t &obj, json& j)
    {
        if (!j.is_number_integer())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is int64_t.";
            return false;
        }
        obj = j.get<int64_t>();
        return true;
    }

    bool JsonToObject(uint64_t &obj, json& j)
    {
        if (!j.is_number_unsigned())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is uint64_t.";
            return false;
        }
        obj = j.get<uint64_t>();
        return true;
    }

    bool JsonToObject(bool &obj, json& j)
    {
        if (!j.is_boolean())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is bool.";
            return false;
        }
        obj = j.get<bool>();
        return true;
    }

    bool JsonToObject(float &obj, json& j)
    {
        if (!j.is_number_float())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is float.";
            return false;
        }
        obj = j.get<float>();
        return true;
    }

    bool JsonToObject(double &obj, json& j)
    {
        if (!j.is_number_float())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is double.";
            return false;
        }
        obj = j.get<double>();
        return true;
    }

    bool JsonToObject(std::string &obj, json& j)
    {
        obj = "";
        if (j.is_null())
            return true;
        //object or number conver to string
        else if (j.is_object() || j.is_number())
            obj = GetStringFromJsonValue(j);
        else if (!j.is_string())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is string.";
            return false;
        }
        else
            obj = j.get<std::string>();

        return true;
    }

    template <typename TYPE>
    bool JsonToObject(std::vector<TYPE> &obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is std::vector<TYPE>.";
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            TYPE item;
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            obj.push_back(item);
        }
        return true;
    }

    template<typename TYPE, std::size_t N>
    bool JsonToObject(std::array<TYPE, N>& obj, json& j)
    {
        if (!j.is_array())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is std::array<TYPE, N>.";
            return false;
        }

        assert(obj.size() == j.size());
        size_t i = 0;
        for (auto it = j.begin(); it != j.end(); ++it, ++i)
        {
            TYPE& item = obj[i];
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            //obj[i] = item;
        }
        return true;
    }

    template <typename TYPE>
    bool JsonToObject(std::list<TYPE> &obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is std::list<TYPE>.";
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            TYPE item;
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            obj.push_back(item);
        }
        return true;
    }

    template <typename TYPE>
    bool JsonToObject(std::set<TYPE>& obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is std::set<TYPE>.";
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            TYPE item;
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            obj.insert(item);
        }
        return true;
    }

    template <typename TYPE>
    bool JsonToObject(std::map<std::string, TYPE> &obj, json& j)
    {
        obj.clear();
        if (!j.is_object())
        {
            m_message = "json-value is " + std::string(j.type_name()) + " but object is std::map<std::string, TYPE>.";
            return false;
        }

        for (auto it=j.begin(); it!=j.end(); ++it)
        {
            TYPE item;
            if (!JsonToObject(item, it.value()))
            {
                return false;
            }

            obj.insert(std::pair<std::string, TYPE>(it.key(), item));
        }
        return true;
    }

    template<typename TYPE>
    bool JsonToObject(TYPE*& pObj, json& j)
    {
        if (!pObj)
        {
            pObj = new TYPE();
        }
        return JsonToObject(*pObj, j);
    }

    template<typename TYPE>
    bool JsonToObject(std::shared_ptr<TYPE>& spObj, json& j)
    {
        if (!spObj)
        {
            spObj = std::make_shared<TYPE>();
        }

        return JsonToObject(*spObj, j);
    }

public:
    /******************************************************
         * Conver base-type : base-type to json string
         * Contain: int\uint、int64_t\uint64_t、bool、float
         *          double、string、vector、list、map<string,XX>
         *
         ******************************************************/
    bool ObjectToJson(int &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(unsigned int &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(int64_t &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(uint64_t &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(bool &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(float &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(double &obj, json& j)
    {
        j = obj;
        return true;
    }

    bool ObjectToJson(std::string &obj, json& j)
    {
        j = obj;
        return true;
    }

    template<typename TYPE>
    bool JsonPushBack(TYPE& v, json& j)
    {
        json jv;
        if (!ObjectToJson(v, jv))
        {
            return false;
        }

        j.push_back(jv);
        return true;
    }

    template <typename TYPE>
    bool ObjectToJson(std::vector<TYPE> &obj, json& j)
    {
        for (auto& v : obj)
        {
            if (!JsonPushBack(v, j))
            {
                return false;
            }
        }
        return true;
    }

    template<typename TYPE, std::size_t N>
    bool ObjectToJson(std::array<TYPE, N>& obj, json& j)
    {
        for (auto& v : obj)
        {
            if (!JsonPushBack(v, j))
            {
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    bool ObjectToJson(std::list<TYPE> &obj, json& j)
    {
        for (auto& v : obj)
        {
            if (!JsonPushBack(v, j))
            {
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    bool ObjectToJson(std::set<TYPE>& obj, json& j)
    {
        for (auto it = obj.begin(); it != obj.end(); ++it)
        {
            auto v = *it;
            if (!JsonPushBack(v, j))
            {
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    bool ObjectToJson(std::map<std::string, TYPE> &obj, json& j)
    {
        for (auto& p : obj)
        {
            json jv;
            if (!ObjectToJson(p.second, jv))
            {
                return false;
            }

            j[p.first] = jv;
        }

        return true;
    }

    template <typename TYPE>
    bool ObjectToJson(TYPE* pObj, json& j)
    {
        if (!pObj)
        {
            std::cout << "ptr is nullptr." << std::endl;
            return false;
        }

        return ObjectToJson(*pObj, j);
    }

    template <typename TYPE>
    bool ObjectToJson(std::shared_ptr<TYPE>& spObj, json& j)
    {
        if (!spObj)
        {
            std::cout << "spObj is null." << std::endl;
            return false;
        }
        
        return ObjectToJson(*spObj, j);
    }

public:
    std::string m_message;
};

class JsonHelper
{
public:
    /**
         * @brief conver json string to class | struct
         * @param obj : class or struct
         * @param jsonStr : json string 
         * @param keys : the path of the object
         * @param message : printf err message when conver failed
         */
    template <typename T>
    static bool JsonToObject(T &obj, const std::string &jsonStr, const std::vector<std::string> keys = {}, std::string *message = NULL)
    {
        json j = json::parse(jsonStr);
        if (j.is_null())
        {
            if (message)
                *message = "Json string can't parse.";
            return false;
        }

        // Go to the key-path
        // 当keys不为空时，表示只解析具体key-path的json
        std::string path;
        for (int i = 0; i < (int)keys.size(); i++)
        {
            if (!path.empty())
                path += "->";
            path += keys[i];

            auto it = j.find(keys[i]);
            if (!j.is_object() || it == j.end())
            {
                if (message)
                    *message = "Can't parse the path [" + path + "].";
                return false;
            }
            j = *it;
        }

        //Conver
        JsonHelperPrivate handle;
        if (!handle.JsonToObject(obj, j))
        {
            if (message)
                *message = handle.m_message;
            return false;
        }
        return true;
    }

    /**
         * @brief conver json string to class | struct
         * @param jsonStr : json string
         * @param defaultT : default value
         * @param keys : the path of the object
         * @param message : printf err message when conver failed
         */
    template <typename T>
    static T Get(const std::string &jsonStr, T defaultT, const std::vector<std::string> keys = {}, std::string *message = NULL)
    {
        T obj;
        if (JsonToObject(obj, jsonStr, keys, message))
            return obj;

        return defaultT;
    }

    /**
         * @brief conver class | struct to json string 
         * @param errMessage : printf err message when conver failed
         * @param obj : class or struct
         * @param jsonStr : json string 
         */
    template <typename T>
    static bool ObjectToJson(T &obj, std::string &jsonStr, std::string *message = NULL)
    {
        json j;
        //Conver
        JsonHelperPrivate handle;
        if (!handle.ObjectToJson(obj, j))
        {
            if (message)
                *message = handle.m_message;
            return false;
        }

        jsonStr = j.dump(4);
        return true;
    }
};

} // namespace aigc
