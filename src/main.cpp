#include "serialization/JsonSerializationHelper.hpp"

using namespace std;
using namespace cbim;

// Base类
// 目的：只要子类继承Base类，生成json时，就会带上子类的类型信息，
//      是否需要继承，用户是可选的
template<typename T>
struct JBase {
public:
    JBase() : type_name(typeid(T).name()){}

private:
    std::string type_name;

    // list需要序列化的成员变量
    CBIM_JSON_HELPER(type_name)
    // 将默认key重命名为用户自定义的名称
    CBIM_JSON_HELPER_RENAME(type_name->"type")
};

struct Geometry : public JBase<Geometry>
{
    std::vector<glm::vec3> vtxs;
    std::vector<glm::vec4> colors;

    // list需要序列化的成员变量
    CBIM_JSON_HELPER(vtxs, colors)
    // list需要序列化的基类，这样在生成json时，才会有当前类的类型信息
    CBIM_JSON_HELPER_BASE((JBase*)this)
};

struct Model : public JBase<Model>
{
    std::string uuid;
    std::shared_ptr<Geometry> geo;
    glm::vec3 translate;
    glm::quat rotate;
    glm::vec3 scale;

    // list需要序列化的成员变量
    CBIM_JSON_HELPER(uuid, geo, translate, rotate, scale)
    // list需要序列化的基类，这样在生成json时，才会有当前类的类型信息
    CBIM_JSON_HELPER_BASE((JBase*)this)
    // 将默认key重命名为用户自定义的名称
    CBIM_JSON_HELPER_RENAME(
        rotate->"Rotate",
        geo->"Geometry", 
        uuid->"Uuid"
    )
};

class Scene : public JBase<Scene>
{
public:
    Scene()
    {
        for (int i=0; i < 2; ++i)
        {
            Model m;
            m.uuid = "uuid_" + std::to_string(i);
            m.translate = glm::vec3(i*1.0f);
            m.rotate = glm::quat(1.0f, 2.0f, 3.0f, 4.0f);
            m.scale = glm::vec3(i*3*1.0f);
            m.geo = std::make_shared<Geometry>();

            for (int j=0; j<2; ++j)
            {
                m.geo->vtxs.push_back(glm::vec3(j*1.0f));
                m.geo->colors.push_back(glm::vec4(j*1.0f/2.0f));
            }

            this->models.push_back(std::move(m));
        }

        for (int i=0; i<this->models.size(); ++i)
        {
            this->modelHight.insert({this->models[i].uuid, i*4.0f});
        }

        this->test = {1, {1.0, 2.0, 3.0}};
    }

private:
    std::vector<Model> models;
    std::map<std::string, float> modelHight;
    std::tuple<int, std::vector<float>> test;

    // list需要序列化的成员变量
    CBIM_JSON_HELPER(models, modelHight, test)
    // list需要序列化的基类，这样在生成json时，才会有当前类的类型信息
    CBIM_JSON_HELPER_BASE((JBase*)this)
};

int main()
{
    Scene scene;

    std::string strJson;
    // 序列化为json
    JsonSerializationHelper::ObjectToJson(scene, strJson, 4);
    LOG(strJson);

    Scene s2;
    // 反序列化为对象
    JsonSerializationHelper::JsonToObject(s2, strJson);

    return 0;
}
