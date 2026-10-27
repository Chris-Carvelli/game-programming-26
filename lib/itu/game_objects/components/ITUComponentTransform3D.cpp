#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

void ITUComponentTransform3D::Init(const rapidjson::Value& config)
{
    glm::vec3 pos = itu_deserialize_vec3f(config["position"]);
    glm::vec3 rot = itu_deserialize_vec3f(config["rotation"]);
    glm::vec3 scale = itu_deserialize_vec3f(config["scale"]);
    local = glm::translate(pos) * glm::eulerAngleXYZ(rot.x, rot.y, rot.z) * glm::scale(scale);
}
