#ifndef ITU_GAME_OBJECT_ID_HPP
#define ITU_GAME_OBJECT_ID_HPP

#include <itu_engine.hpp>

class ITUGameObject;

class ITUGameObjectId
{
    friend class ITUScene;
public:
    ITUGameObjectId() : generation(-1), index(-1) { };
    ITUGameObjectId(const rapidjson::Value& v);
    static ITUGameObjectId NULL_ID;
    bool IsValid() { return generation != (Uint32)-1 && index != (Uint32)-1; }
    bool operator==(ITUGameObjectId& other) { return generation == other.generation && index == other.index; }
    bool operator!=(ITUGameObjectId& other) { return generation != other.generation || index != other.index; }

    template<typename Writer>
    void Serialize(Writer& writer)
    {
         writer.BeginArray();
            writer.WriteUint(generation);
            writer.WriteUint(index);
         writer.EndArray();
    }
private:
    Uint32 generation;
    Uint32 index;

    ITUGameObjectId(Uint32 gen, Uint32 idx) : generation(gen), index(idx) { }
    void Invalidate() { generation = (Uint32)-1; }
};


#endif // ITU_GAME_OBJECT_ID_HPP