#include "serialization/JsonSerializationHelper.hpp"
#include <memory>

using namespace std;
using namespace cbim;

class Popole
{
public:
    string name;
    string age;

    CBIM_JSON_HELPER(name, age)
};

class Student : Popole
{
public:
    string depart;
    int grade;
    CBIM_JSON_HELPER(depart, grade)
    CBIM_JSON_HELPER_BASE((Popole*)this)
};

class TheClass
{
public:
    string Name;
    int test;
    std::list<Student> students;
    Popole* master = nullptr;
    std::shared_ptr<std::string> spContent;
    std::unordered_set<float> arr;
    std::unordered_map<int, int> mapKV;
    glm::vec2 v2;
    CBIM_JSON_HELPER(Name, test, students, master, spContent, arr, mapKV, v2)
    CBIM_JSON_HELPER_DEFAULT("test=123")
};

string sjson = R"({
    "v2": [
        1.0,
        2.0
    ],
    "arr": [
        1.0,
        1.1,
        1.2
    ],
    "mapKV": [
        {
            "key": 0,
            "value": 1
        },
        {
            "key": 1,
            "value": 2
        }
    ],
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

struct Geo
{
    std::unordered_map<std::string, size_t> people;
    CBIM_JSON_HELPER(people)
};

struct Table
{
    std::vector<float> vecNames;
    CBIM_JSON_HELPER(vecNames)
};

int main()
{
    //std::cout << sjson << std::endl;
    TheClass my;
    JsonSerializationHelper::JsonToObject(my, sjson);

    std::string str;
    JsonSerializationHelper::ObjectToJson(my, str, 4);
    std::cout << str << std::endl;
    std::cout << str.size() << std::endl;

//    std::vector<uint8_t> buf;
//    JsonHelper::ObjectToJsonCbor(my, buf);
//    std::cout << buf.size() << std::endl;

//    TheClass mm;
//    JsonHelper::JsonCborToObject(mm, buf);
//    std::cout << mm.Name << std::endl;

//    Geo g;
////    g.name = "大家";
//    JsonSerializationHelper::ObjectToJson(g, str, 4);
//    std::cout << str << std::endl;

//    Geo g0;
//    JsonSerializationHelper::JsonToObject(g0, str);

//    Geo g;
//    g.people = {{"Ann",25},{"Bill", 46}, {"Jack", 77}, {"Jack", 32},{"Jill", 32}, {"Ann", 35} };
//    JsonSerializationHelper::ObjectToJson(g, str, 4);


//    Table tb;
//    tb.vecNames = {1, 2};
//    JsonSerializationHelper::ObjectToJson(tb, str, 4);
//    std::cout << str << std::endl;

    return 0;
}
