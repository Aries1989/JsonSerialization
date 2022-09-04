#if 1
#include "AIGCJson/AIGCJson.hpp"
#include <memory>

using namespace std;
using namespace aigc;

class Popole
{
public:
    string name;
    string age;

    AIGC_JSON_HELPER(name, age) //成员注册
};
class Student : Popole
{
public:
    string depart;
    int grade;
    AIGC_JSON_HELPER(depart, grade) //成员注册
    AIGC_JSON_HELPER_BASE((Popole*)this) //基类注册
};
class Class
{
public:
    string Name;
    int test;
    std::list<Student> students;
    std::shared_ptr<Popole> master;
    std::shared_ptr<std::string> spContent;
    AIGC_JSON_HELPER(Name, test, students, master, spContent) //成员注册
    AIGC_JSON_HELPER_DEFAULT("test=123")
};

string sjson = R"({
    "spContent": "this is sp content",
    "Name": "yaronzz",
    "master" : {
        "name" : "mr liu",
        "age" : 35
    },
    "students" : [
        {
            "name" : "zhangxiaoming",
            "age" : 5,
            "grade" : 3,
            "depart" : "xiaoxue"
        },
        {
            "name" : "zhangxiaohong",
            "age" : 7,
            "grade" : 3,
            "depart" : "xiaoxue"
        }
    ]
})";

int main()
{
    std::cout << sjson << std::endl;
    Class my;
    JsonHelper::JsonToObject(my, sjson);

    std::string str;
    JsonHelper::ObjectToJson(my, str);
    std::cout << str << std::endl;

    return 0;
}

#else
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

int main()
{
    std::map<std::string, int> c_map{ {"one", 1}, {"two", 2}, {"three", 3} };

    json j;
    for (auto p : c_map)
    {
        json jv = p.second;
        j[p.first] = jv;
    }
    std::cout << j << std::endl;

    //json j(c_map);
    //std::cout << j.dump(4) << std::endl;
    //
    //for (auto it=j.begin(); it!=j.end(); ++it)
    //{
    //    std::cout << it.key() << "\t" << it.value() << std::endl;
    //}

    //json j = c_map;
    //json j;
    //std::list obj = { 1, 2, 3, 4, 5 };
    //for (const auto& v : obj)
    //{
    //    json jv = v;

    //    j.push_back(jv);
    //}
    //std::cout << j << std::endl;

    return 1;
}

#endif
