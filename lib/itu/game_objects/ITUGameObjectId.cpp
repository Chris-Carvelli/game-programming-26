#include <itu_game_objects.hpp>

ITUGameObjectId ITUGameObjectId::NULL_ID = { (Uint32)-1, (Uint32)-1 };

ITUGameObjectId::ITUGameObjectId(const rapidjson::Value& v)
{
    generation = 0;
    index = v.GetInt();
}
