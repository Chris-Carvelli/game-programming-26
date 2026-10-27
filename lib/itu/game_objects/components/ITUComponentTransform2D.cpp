#include <itu_game_objects.hpp>

void ITUComponentTransform2D::Init(const rapidjson::Value& config)
{
    transform.position = itu_deserialize_vec2f(config["position"]);
    transform.rotation = config["rotation"].GetFloat();
    transform.scale    = itu_deserialize_vec2f(config["scale"]);
}
