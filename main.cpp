#include "AIGCJson/AIGCJson.hpp"
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
    Popole master;
    AIGC_JSON_HELPER(Name, test, students, master) //成员注册
    AIGC_JSON_HELPER_DEFAULT("test=123")
};

string sjson = R"({
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
    return 0;
}


