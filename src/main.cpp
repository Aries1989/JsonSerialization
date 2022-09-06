#include "serialization/JsonSerializationHelper.hpp"
#include <memory>

using namespace std;
using namespace cbim;

template<typename T>
struct Base{
    std::string type;

    Base() : type(typeid(T).name()){}
    CBIM_JSON_HELPER(type)
};

struct Geometry : public Base<Geometry>
{
    std::vector<glm::vec3> vtxs;
    std::vector<glm::vec4> colors;

    CBIM_JSON_HELPER(vtxs, colors)
    CBIM_JSON_HELPER_BASE((Base*)this)
};

struct Model : public Base<Model>
{
    std::string uuid;
    std::shared_ptr<Geometry> geo;
    glm::vec3 translate;
    glm::vec3 rotate;
    glm::vec3 scale;

    CBIM_JSON_HELPER(uuid, geo, translate, rotate, scale)
    CBIM_JSON_HELPER_BASE((Base*)this)
};

class Scene : public Base<Scene>
{
public:
    Scene()
    {
        for (int i=0; i < 2; ++i)
        {
            Model m;
            m.uuid = "uuid_" + std::to_string(i);
            m.translate = glm::vec3(i*1.0f);
            m.rotate = glm::vec3(i*2*1.0f);
            m.scale = glm::vec3(i*3*1.0f);
            m.geo = std::make_shared<Geometry>();

            for (int j=0; j<2; ++j)
            {
                m.geo->vtxs.push_back(glm::vec3(j*1.0f));
                m.geo->colors.push_back(glm::vec4(j/2.0));
            }

            this->models.push_back(std::move(m));
        }

        for (int i=0; i<this->models.size(); ++i)
        {
            this->modelHight.insert({this->models[i].uuid, i*4});
        }
    }
private:
    std::vector<Model> models;
    std::map<std::string, float> modelHight;

    CBIM_JSON_HELPER(models, modelHight)
    CBIM_JSON_HELPER_BASE((Base*)this)
};

int main()
{
    Scene scene;

    std::string strJson;
    JsonSerializationHelper::ObjectToJson(scene, strJson, 4);
    LOG(strJson);
}

#if 0
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
    std::vector<unsigned short> vecNames;
    std::tuple<int, std::string> tp;
    std::string city;
    std::pair<int, Geo> p;
    CBIM_JSON_HELPER(vecNames, tp, city, p)
};


template<std::size_t I, class T>
void PrintXXX()
{

}

int main()
{
    //std::cout << sjson << std::endl;
    //    TheClass my;
    //    JsonSerializationHelper::JsonToObject(my, sjson);

    std::string str;
    //    JsonSerializationHelper::ObjectToJson(my, str, 4);
    //    std::cout << str << std::endl;
    //    std::cout << str.size() << std::endl;

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


    //    std::tuple<int, std::string> v = std::make_tuple(1, "xyz");
    //    size_t size = std::tuple_size_v<decltype(v)>;
    //    for (size_t i=0; i<size; ++i)
    //    {
    //        std::tuple_element_t<i, decltype(v)> x = std::get<i>(v);
    //    }


    //    std::cout << "integral_constant<int, 5> == "
    //              << std::integral_constant<int, 5>::value << std::endl;
    //    std::cout << "integral_constant<bool, false> == " << std::boolalpha
    //              << std::integral_constant<bool, false>::value << std::endl;

    //    return 0;

    Geo g;
    g.people = {{"Ann",25},{"Bill", 46}, {"Jack", 77}, {"Jack", 32},{"Jill", 32}, {"Ann", 35} };
    //    JsonSerializationHelper::ObjectToJson(g, str, 4);


    Table tb;
    tb.vecNames = {1, 2};
    tb.tp = std::make_tuple(1, "abc");
    tb.city = "beijing";
    tb.p = std::make_pair(19, g);
    JsonSerializationHelper::ObjectToJson(tb, str, 4);
    std::cout << str << std::endl;

    JsonSerializationHelper::JsonToObject(tb, str);

    //    const auto t = std::make_tuple(1.0, std::string{"string"}, 42, std::vector<int> {0, 1});
    //    json j(t);

    //    assert(j.type() == json::value_t::array);
    //    assert(j.size() == 4);
    //    assert(j.get<decltype(t)>() == t);
    //    assert(j[0] == std::get<0>(t));
    //    assert(j[1] == std::get<1>(t));
    //    assert(j[2] == std::get<2>(t));
    //    assert(j[3][0] == 0);
    //    assert(j[3][1] == 1);

    //    std::pair<int, std::string> p = std::make_pair(19, "bj");
    //    json j(p);
    //    std::cout << j.dump(4) << std::endl;


    return 0;
}
#endif
