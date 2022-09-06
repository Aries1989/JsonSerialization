#pragma once
#include <set>
#include <list>
#include <map>
#include <array>
#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "cbim_type_traits.hpp"

#ifdef SUPPORT_GLM_SERIALIZATION
#include "glm/glm.hpp"
#include <glm/gtc/epsilon.hpp>
#endif

#define UN_USED(V) (void)(V)

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void LOG(const std::string& log)
{
    std::cout << log << std::endl;
}
/******************************************************
 * Register class or struct members
 * eg:
 * struct Test
 * {
 *      string A;
 *      string B;
 *      CBIM_JSON_HELPER(A, B)
 * };         
 * 反序列化
 * jsonValue：最后返回的json对象
 * names: 表示使用CBIM_JSON_HELPER_RENAME宏，重命名后的成员变量名
 * standardNames：表示将成员变量名称作为name的情况
 ******************************************************/
#define CBIM_JSON_HELPER(...)                                                            \
    friend cbim::JsonHelperPrivate;                                                      \
    std::map<std::string, std::string> __defaultValues;                                  \
    bool CbimJsonToObject(cbim::JsonHelperPrivate &handle,                               \
                          json& j,                                                       \
                          std::vector<std::string> &names)                               \
    {                                                                                    \
        std::vector<std::string> standardNames = handle.GetMembersNames(#__VA_ARGS__);   \
        if (names.size() <= standardNames.size())                                        \
        {                                                                                \
            for (size_t i = names.size(); i < standardNames.size(); i++)                 \
                names.push_back(standardNames[i]);                                       \
        }                                                                                \
        return handle.SetMembers(names, 0, j, __defaultValues, __VA_ARGS__);             \
    }                                                                                    \
    bool CbimObjectToJson(cbim::JsonHelperPrivate &handle,                               \
                          json& j,                                                       \
                          std::vector<std::string> &names) const                         \
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
 *      CBIM_JSON_HELPER(A, B)
 *      CBIM_JSON_HELPER_RENAME("a", "b")
 * };         
 ******************************************************/
#define CBIM_JSON_HELPER_RENAME(...)                                            \
    std::vector<std::string> CbimRenameMembers(cbim::JsonHelperPrivate &handle) \
    {                                                                           \
        return handle.GetMembersNames(#__VA_ARGS__);                            \
    }

/******************************************************
 * Register base-class
 * eg:
 * struct Base
 * {
 *      string name;
 *      CBIM_JSON_HELPER(name)
 * };  
 * struct Test : Base
 * {
 *      string A;
 *      string B;
 *      CBIM_JSON_HELPER(A, B)
 *      CBIM_JSON_HELPER_BASE((Base*)this)
 * };         
 ******************************************************/
#define CBIM_JSON_HELPER_BASE(...)                                           \
    bool CbimBaseJsonToObject(cbim::JsonHelperPrivate &handle, json& j)      \
    {                                                                        \
        return handle.SetBase(j, __VA_ARGS__);                               \
    }                                                                        \
    bool CbimBaseObjectToJson(cbim::JsonHelperPrivate &handle, json& j) const\
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
 *      CBIM_JSON_HELPER(name, age)
 *      CBIM_JSON_HELPER_DEFAULT(age=18)
 * };  
 * 新增成员变量后，如果想使用已有的json字符串反序列化对象，
 * 新增的成员变量肯定是无法解析到，此时，可以使用改宏，为
 * 新增的变量设置默认值。
 ******************************************************/
#define CBIM_JSON_HELPER_DEFAULT(...)                                  \
    void CbimDefaultValues(cbim::JsonHelperPrivate &handle)            \
    {                                                                  \
        __defaultValues = handle.GetMembersValueMap(#__VA_ARGS__); \
    }

namespace cbim
{
class JsonHelperPrivate
{
public:
    /******************************************************
         *
         * enable_if
         *
         ******************************************************/
    template <bool, class TYPE = void>
    struct enable_if
    {
    };

    template <class TYPE>
    struct enable_if<true, TYPE>
    {
        typedef TYPE type;
    };

public:
    /******************************************************
         * 
         * Check Interface
         *      If class or struct add CBIM_JSON_HELPER\CBIM_JSON_HELPER_RENAME\CBIM_JSON_HELPER_BASE,
         *      it will go to the correct conver function.
         *
         ******************************************************/
    template <typename T>
    struct HasConverFunction
    {
        template <typename TT>
        static char func(decltype(&TT::CbimJsonToObject));

        template <typename TT>
        static int func(...);

        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T>
    struct HasRenameFunction
    {
        template <typename TT>
        static char func(decltype(&TT::CbimRenameMembers));
        template <typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T>
    struct HasBaseConverFunction
    {
        template <typename TT>
        static char func(decltype(&TT::CbimBaseJsonToObject));
        template <typename TT>
        static int func(...);
        const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
    };

    template <typename T>
    struct HasDefaultValueFunction
    {
        template <typename TT>
        static char func(decltype(&TT::CbimDefaultValues));
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
        return obj.CbimJsonToObject(*this, j, names);
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

        LOG("unsupported this type.");
        return false;
    }

    // 序列化
    template <typename T, typename enable_if<HasConverFunction<T>::has, int>::type = 0>
    bool ObjectToJson(const T &obj, json& j)
    {
        // 基类序列化
        if (!BaseConverObjectToJson(obj, j))
            return false;

        std::vector<std::string> names = LoadRenameArray(obj);
        // 类对象obj的序列化
        return obj.CbimObjectToJson(*this, j, names);
    }

    // 枚举类型的序列化进行单独处理
    // 先转为int，再进行序列化
    template <typename T, typename enable_if<!HasConverFunction<T>::has, int>::type = 0>
    bool ObjectToJson(const T &obj, json& j)
    {
        if (std::is_enum<T>::value)
        {
            // ★★★ 编译时，如果此处报错，可将此处2行注释，通过后续的打印来查看到底是什么类型不支持
            int ivalue = static_cast<int>(obj);
            return ObjectToJson(ivalue, j);
        }

        LOG("unsupported this type." + std::string(typeid(obj).name()));

        return false;
    }

    /******************************************************
         *
         * Interface of LoadRenameArray
         *
         ******************************************************/
    // 获取重命名的成员名称
    template <typename T, typename enable_if<HasRenameFunction<T>::has, int>::type = 0>
    std::vector<std::string> LoadRenameArray(const T &obj)
    {
        return obj.CbimRenameMembers(*this);
    }

    template <typename T, typename enable_if<!HasRenameFunction<T>::has, int>::type = 0>
    std::vector<std::string> LoadRenameArray(const T &obj)
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
        return obj.CbimBaseJsonToObject(*this, j);
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
    bool BaseConverObjectToJson(const T &obj, json& j)
    {
        return obj.CbimBaseObjectToJson(*this, j);
    }

    // 过滤!HasBaseConverFunction的对象,直接返回true
    template <typename T, typename enable_if<!HasBaseConverFunction<T>::has, int>::type = 0>
    bool BaseConverObjectToJson(const T &obj, json& j)
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
        obj.CbimDefaultValues(*this);
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
    std::vector<std::string> GetMembersNames(const std::string& membersStr)
    {
        std::vector<std::string> array = StringSplit(membersStr);
        StringTrim(array);
        return array;
    }

    std::map<std::string, std::string> GetMembersValueMap(const std::string& valueStr)
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
            //m_message = "[" + names[index] + "] " + m_message;
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
        json &j, const TYPE &arg, const TYPES &...args)
    {
        // index对应的字段参数的序列化
        if (!GetMembers(names, index, j, arg))
            return false;

        // 递归调用自己
        return GetMembers(names, ++index, j, args...);
    }

    template <typename TYPE>
    bool GetMembers(const std::vector<std::string> &names, int index,
        json &j, const TYPE &arg)
    {
        json jv;
        // 基于arg的具体类型，调用相应的ObjectToJson，实现具体参数的序列化
        bool check = ObjectToJson(arg, jv);
        if (!check)
        {
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
    void StringToObject(TYPE &obj, const std::string &value)
    {
        UN_USED(value);
        UN_USED(obj);

        return;
    }

    void StringToObject(std::string &obj, const std::string &value)
    {
        obj = value;
    }

    void StringToObject(int &obj, const std::string &value)
    {
        obj = atoi(value.c_str());
    }

    void StringToObject(unsigned int &obj, const std::string &value)
    {
        char *end;
        obj = strtoul(value.c_str(), &end, 10);
    }

    void StringToObject(int64_t &obj, const std::string &value)
    {
        char *end;
        obj = strtoll(value.c_str(), &end, 10);
    }

    void StringToObject(uint64_t &obj, const std::string &value)
    {
        char *end;
        obj = strtoull(value.c_str(), &end, 10);
    }

    void StringToObject(bool &obj, const std::string &value)
    {
        obj = (value == "true");
    }

    void StringToObject(float &obj, const std::string &value)
    {
        obj = (float)atof(value.c_str());
    }

    void StringToObject(double &obj, const std::string &value)
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
#define LOG_TYPE_ERROR(obj, j) \
    LOG("json-value is " + std::string(j.type_name()) + " but object is " + typeid(obj).name() + ".");

    template<typename T>
    inline bool GetJsonValue(T& obj, json& j)
    {
        obj = j.get<T>();
        return true;
    }

    inline bool JsonToObject(short &obj, json& j)
    {
        if (!j.is_number_integer())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(unsigned short &obj, json& j)
    {
        if (!j.is_number_integer())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(int &obj, json& j)
    {
        if (!j.is_number_integer())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(unsigned int &obj, json& j)
    {
        if (!j.is_number_unsigned())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(int64_t &obj, json& j)
    {
        if (!j.is_number_integer())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(uint64_t &obj, json& j)
    {
        if (!j.is_number_unsigned())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(bool &obj, json& j)
    {
        if (!j.is_boolean())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(float &obj, json& j)
    {
        if (!j.is_number_float())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(double &obj, json& j)
    {
        if (!j.is_number_float())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        return GetJsonValue(obj, j);
    }

    inline bool JsonToObject(std::string &obj, json& j)
    {
        obj = "";

        if (j.is_null())
            return true;
        else if (j.is_object() || j.is_number())
            obj = GetStringFromJsonValue(j);
        else if (!j.is_string())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }
        else
            GetJsonValue(obj, j);

        return true;
    }

    template <typename T>
    inline bool JsonToObject(std::vector<T> &obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            T item;
            if (!JsonToObject(item, *it)) return false;

            obj.push_back(std::move(item));
        }
        return true;
    }

    template<typename T, std::size_t N>
    bool JsonToObject(std::array<T, N>& obj, json& j)
    {
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        assert(obj.size() == j.size());
        size_t i = 0;
        for (auto it = j.begin(); it != j.end(); ++it, ++i)
        {
            T& item = obj[i];
            if (!JsonToObject(item, *it)) return false;
        }
        return true;
    }

    template <typename T>
    inline bool JsonToObject(std::list<T> &obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            T item;
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            obj.push_back(std::move(item));
        }
        return true;
    }

    template <typename T>
    inline bool JsonToObject(std::set<T>& obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            T item;
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            obj.insert(std::move(item));
        }
        return true;
    }

    template <typename T>
    inline bool JsonToObject(std::unordered_set<T>& obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            T item;
            if (!JsonToObject(item, *it))
            {
                return false;
            }
            obj.insert(std::move(item));
        }
        return true;
    }

    template<typename T>
    inline bool GetValueByKey(json& j, const std::string& key, T& v)
    {
        auto it = j.find(key);

        if (it == j.end()) return false;
        if (!JsonToObject(v, it.value())) return false;

        return true;
    }

    template <typename KEY, typename VALUE>
    inline bool JsonToObject(std::map<KEY, VALUE> &obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it=j.begin(); it!=j.end(); ++it)
        {
            json jPair = *it;

            KEY k;
            if (!GetValueByKey(jPair, "k", k)) return false;

            VALUE v;
            if (!GetValueByKey(jPair, "v", v)) return false;

            obj.insert(std::pair<KEY, VALUE>(std::move(k), std::move(v)));
        }
        return true;
    }

    template <typename KEY, typename VALUE>
    inline bool JsonToObject(std::unordered_map<KEY, VALUE>& obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            json jPair = *it;

            KEY k;
            if (!GetValueByKey(jPair, "k", k)) return false;

            VALUE v;
            if (!GetValueByKey(jPair, "v", v)) return false;


            obj.insert(std::pair<KEY, VALUE>(std::move(k), std::move(v)));
        }
        return true;
    }

    template <typename KEY, typename VALUE>
    inline bool JsonToObject(std::multimap<KEY, VALUE>& obj, json& j)
    {
        obj.clear();
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j)
            return false;
        }

        for (auto it = j.begin(); it != j.end(); ++it)
        {
            json jPair = *it;

            KEY k;
            if (!GetValueByKey(jPair, "k", k)) return false;

            VALUE v;
            if (!GetValueByKey(jPair, "v", v)) return false;


            obj.insert(std::pair<KEY, VALUE>(std::move(k), std::move(v)));
        }
        return true;
    }


    template<typename T>
    inline bool JsonToObject(T*& pObj, json& j)
    {
        if (!pObj)
        {
            pObj = new T();
        }
        return JsonToObject(*pObj, j);
    }

    template<typename T>
    inline bool JsonToObject(std::shared_ptr<T>& spObj, json& j)
    {
        if (!spObj)
        {
            spObj = std::make_shared<T>();
        }

        return JsonToObject(*spObj, j);
    }

    template<typename A, typename B>
    inline bool JsonToObject(std::pair<A, B>& obj, json& j)
    {
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j);
            return false;
        }

        assert(2 == j.size());

        auto& a = obj.first;
        if (!JsonToObject(a, j[0])) return false;
        auto& b = obj.second;
        if (!JsonToObject(b, j[1])) return false;

        return true;
    }

    template<typename... Args>
    inline bool JsonToObject(std::tuple<Args...>& obj, json& j)
    {
        if (!j.is_array()){
            LOG_TYPE_ERROR(obj, j);
            return false;
        }

        obj = j.get<std::tuple<Args...>>();
        return true;
    }



#ifdef SUPPORT_GLM_SERIALIZATION
    template <typename T, int N, glm::precision P>
    inline bool JsonToObject(glm::vec<N, T, P>& v, json& j)
    {
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(v, j)
            return false;
        }

        assert(N == j.size());
        for (int i=0; i<j.size(); ++i)
        {
            if (!JsonToObject(v[i], j[i])) return false;
        }

        return true;
    }

    template<int C, int R, typename T, glm::precision P>
    inline bool JsonToObject(glm::mat<C, R, T, P>& m, json& j)
    {
        static std::vector<std::string> cols = {"c0", "c1", "c2", "c3", "c4"};
        if (!j.is_object())
        {
            LOG_TYPE_ERROR(m, j)
            return false;
        }

        assert(C <= cols.size());

        for (int i=0; i<C; ++i)
        {
            auto it = j.find(cols[i]);
            if (it == j.end())
            {
                LOG("can not find column " + cols[i]);
                return false;
            }

            auto& c = m[i];
            if (!JsonToObject(c, *it)) return false;
        }

        return true;
    }
    template<typename T, glm::precision P>
    inline bool JsonToObject(glm::tquat<T, P>& obj, json& j)
    {
        if (!j.is_array())
        {
            LOG_TYPE_ERROR(obj, j);
            return false;
        }

        assert(4 == j.size());
        for (int i=0; i<j.size(); ++i)
        {
            if (!JsonToObject(obj[i], j[i])) return false;
        }

        return true;
    }
#endif

public:
    /******************************************************
         * Conver base-type : base-type to json string
         * Contain: int\uint、int64_t\uint64_t、bool、float
         *          double、string、vector、list、map<string,XX>
         *
         ******************************************************/
    template<typename T>
    inline bool SetJsonValue(json& j, const T& v)
    {
        j = v;
        return true;
    }

    inline bool ObjectToJson(const short &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const unsigned short &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const int &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const unsigned int &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const int64_t &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const uint64_t &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const bool &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const float &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const double &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    inline bool ObjectToJson(const std::string &obj, json& j)
    {
        return SetJsonValue(j, obj);
    }

    template<typename LIST>
    inline bool ListObjectToJson(const LIST& obj, json& j)
    {
        for (const auto& v : obj)
        {
            json jv;
            if (!ObjectToJson(v, jv)) return false;

            j.push_back(std::move(jv));
        }
        return true;
    }

    template <typename TYPE>
    inline bool ObjectToJson(const std::vector<TYPE> &obj, json& j)
    {
        return ListObjectToJson(obj, j);
    }

    template<typename TYPE, std::size_t N>
    inline bool ObjectToJson(const std::array<TYPE, N>& obj, json& j)
    {
        return ListObjectToJson(obj, j);
    }

    template <typename TYPE>
    inline bool ObjectToJson(const std::list<TYPE> &obj, json& j)
    {
        return ListObjectToJson(obj, j);
    }

    template <typename TYPE>
    inline bool ObjectToJson(const std::set<TYPE>& obj, json& j)
    {
        return ListObjectToJson(obj, j);
    }

    template <typename TYPE>
    inline bool ObjectToJson(const std::unordered_set<TYPE>& obj, json& j)
    {
        return ListObjectToJson(obj, j);
    }

    template<typename MAP>
    inline bool MapObjectToJson(const MAP& obj, json& j)
    {
        for (const auto& p : obj)
        {
            json jPair;

            json jk;
            if (!ObjectToJson(p.first, jk)) return false;
            jPair["k"] = jk;

            json jv;
            if (!ObjectToJson(p.second, jv)) return false;
            jPair["v"] = jv;

            j.push_back(jPair);
        }

        return true;
    }

    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool ObjectToJson(const std::map<KEY_TYPE, VALUE_TYPE> &obj, json& j)
    {
        return MapObjectToJson(obj, j);
    }

    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline bool ObjectToJson(const std::unordered_map<KEY_TYPE, VALUE_TYPE>& obj, json& j)
    {
        return MapObjectToJson(obj, j);
    }

    template <typename KEY, typename VALUE>
    inline bool ObjectToJson(const std::multimap<KEY, VALUE>& obj, json& j)
    {
        return MapObjectToJson(obj, j);
    }

    template <typename TYPE>
    inline bool ObjectToJson(TYPE* pObj, json& j)
    {
        if (!pObj)
        {
            LOG("pObj is nullptr.");
            return false;
        }

        return ObjectToJson(*pObj, j);
    }

    template <typename TYPE>
    inline bool ObjectToJson(const std::shared_ptr<TYPE>& spObj, json& j)
    {
        if (!spObj)
        {
            LOG("spObj is nullptr.");
            return false;
        }
        
        return ObjectToJson(*spObj, j);
    }

    template<typename A, typename B>
    inline bool ObjectToJson(const std::pair<A, B>& obj, json& j)
    {
        json ja;
        if(!ObjectToJson(obj.first, ja)) return false;
        json jb;
        if(!ObjectToJson(obj.second, jb)) return false;

        j.push_back(ja);
        j.push_back(jb);

        return true;
    }

    template<typename... T>
    inline bool ObjectToJson(const std::tuple<T...>& obj, json& j)
    {
        j = json(obj);
        return true;
    }

#ifdef SUPPORT_GLM_SERIALIZATION
    template <typename T, int N>
    inline bool ObjectToJson(const glm::vec<N, T, glm::highp>& v, json& j)
    {
        for (int i = 0; i < N; ++i)
            j.push_back(v[i]);

        return true;
    }

    template<int C, int R, typename T>
    inline bool ObjectToJson(const glm::mat<C, R, T, glm::highp>& m, json& j)
    {
        static std::vector<std::string> cols = {"c0", "c1", "c2", "c3", "c4"};
        assert(C <= cols.size());

        for (int i=0; i<C; ++i)
        {
            const auto& col_v = m[i];

            json jc;
            if (!ObjectToJson(col_v, jc)) return false;

            j[cols[i]] = std::move(jc);
        }

        return true;
    }

    template<typename T, glm::precision P>
    inline bool ObjectToJson(const glm::tquat<T, P>& obj, json& j)
    {
        for (int i = 0; i < obj.length(); ++i)
            j.push_back(obj[i]);

        return true;
    }

#endif
};

class JsonSerializationHelper
{
public:
    template <typename T>
    static bool JsonToObject(T &obj, json& j, const std::vector<std::string> keys = {})
    {
        if (j.is_null())
        {
            LOG("invalid json string.");
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
                LOG("Can't parse the path [" + path + "].");
                return false;
            }
            j = *it;
        }

        JsonHelperPrivate handle;
        if (!handle.JsonToObject(obj, j))
        {
            return false;
        }
        return true;
    }
    /**
         * @brief conver json string to class | struct
         * @param obj : class or struct
         * @param jsonStr : json string 
         * @param keys : the path of the object
         */
    template <typename T>
    static bool JsonToObject(T &obj, const std::string &jsonStr, const std::vector<std::string> keys = {})
    {
        json j = json::parse(jsonStr);
        return JsonToObject(obj, j, keys);
    }

    /**
         * @brief conver json string to class | struct
         * @param jsonStr : json string
         * @param defaultT : default value
         * @param keys : the path of the object
         */
    template <typename T>
    static T Get(const std::string &jsonStr, T defaultT, const std::vector<std::string> keys = {})
    {
        T obj;
        if (JsonToObject(obj, jsonStr, keys))
            return obj;

        return defaultT;
    }

    /**
         * @brief conver class | struct to json string 
         * @param obj : class or struct
         * @param jsonStr : json string 
         * @param indent ：json string indent
         */
    template <typename T>
    static bool ObjectToJson(const T &obj, std::string &jsonStr, const int indent=-1)
    {
        json j;
        JsonHelperPrivate handle;
        if (!handle.ObjectToJson(obj, j))
        {
            return false;
        }

        jsonStr = j.dump(indent);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    static bool JsonCborToObject(T& obj, const std::vector<uint8_t>& buf, const std::vector<std::string> keys = {})
    {
        json j = json::from_cbor(buf);
        return JsonToObject(obj, j, keys);
    }

    template<typename T>
    static bool ObjectToJsonCbor(const T& obj, std::vector<uint8_t>& buf)
    {
        json j;
        JsonHelperPrivate handle;
        if (!handle.ObjectToJson(obj, j)) return false;

        json::to_cbor(j, buf);
        return true;
    }
};

} // namespace cbim
