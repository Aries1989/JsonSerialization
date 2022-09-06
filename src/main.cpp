#include "serialization/JsonSerializationHelper.hpp"

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
    glm::quat rotate;
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
            m.rotate = glm::quat(1.0f, 2.0f, 3.0f, 4.0f);
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

    Scene s2;
    JsonSerializationHelper::JsonToObject(s2, strJson);

    return 0;
}
