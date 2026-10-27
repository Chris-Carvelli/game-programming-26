#ifndef ITU_UTIL_SERIALIZATION_HPP
#define ITU_UTIL_SERIALIZATION_HPP

#include <itu_engine.hpp>

inline bool itu_load_json(const char* path, rapidjson::Document& d)
{
    SDL_IOStream* fp = SDL_IOFromFile(path, "r");
    size_t size = SDL_SeekIO(fp, 0, SDL_IO_SEEK_END);
    SDL_SeekIO(fp, 0, SDL_IO_SEEK_SET);
    char* data = (char*)SDL_malloc(size + 1);
    SDL_ReadIO(fp, data, size);
    SDL_CloseIO(fp);
    data[size] = 0;
    d.Parse(data);
    SDL_free(data);

    d.GetParseError();
    return !d.HasParseError();
}

inline void itu_lib_input_set_mappings(EngineContext* context, rapidjson::Value& config)
{
    rapidjson::Value& keyboard = config["keyboard"];
    rapidjson::Value& mouse = config["mouse"];
    for(rapidjson::Value& binding : keyboard.GetArray())
    {
        int k = binding.GetArray()[0].GetUint();
        int b = binding.GetArray()[1].GetInt();
        itu_lib_input_set_mapping_keyboard(context, k, (BtnType)b);
    }

    for(rapidjson::Value& binding : mouse.GetArray())
    {
        int k = binding.GetArray()[0].GetUint();
        int b = binding.GetArray()[1].GetInt();
        itu_lib_input_set_mapping_mouse(context, k, (BtnType)b);
    }
}

template<typename Writer>
inline void itu_serialize_vec2f(Writer& writer, vec2f v)
{
    writer.BeginArray();
    writer.WriteDouble(v.x);
    writer.WriteDouble(v.y);
    writer.EndArray();
}

inline vec2f itu_deserialize_vec2f(const rapidjson::Value& value)
{
    assert(value.IsArray());
    assert(value.GetArray().Size() == 2);

    return { value[0].GetFloat(), value[1].GetFloat() };
}

template<typename Writer>
inline void itu_serialize_vec3f(Writer& writer, glm::vec3 v)
{
    writer.BeginArray();
    writer.WriteDouble(v.x);
    writer.WriteDouble(v.y);
    writer.WriteDouble(v.z);
    writer.EndArray();
}

inline glm::vec3 itu_deserialize_vec3f(const rapidjson::Value& value)
{
    assert(value.IsArray());
    assert(value.GetArray().Size() == 3);

    return { value[0].GetFloat(), value[1].GetFloat(), value[2].GetFloat() };
}

#endif // ITU_UTIL_SERIALIZATION_HPP