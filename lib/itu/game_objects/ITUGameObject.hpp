#ifndef ITU_GAME_OBJECT_HPP
#define ITU_GAME_OBJECT_HPP

#include <itu_game_objects.hpp>

class ITUScene;
class ITUComponent;

class ITUGameObject
{
    friend ITUScene;

public:
    
    template<class T>
    T* ComponentAdd();

    template<class T>
    bool ComponentRemove();

    template<class T>
    T* ComponentGet();

    ITUGameObject* ParentGet();
    ITUGameObjectId ParentIdGet();
    bool ParentSet(ITUGameObjectId new_parent_id);
    const char* DebugNameGet();

private:
    void Init(const rapidjson::Value& config);
    void Update(float delta);
    void UpdateFixed();
    void Render();

    void Clear();

    ITUGameObjectId id;
    ITUGameObjectId parent_id;
    ITUScene* scene;
    char* name;
    std::vector<ITUGameObjectId> children;
    std::vector<ITUComponent*> components;
};

template<typename T>
T* ITUGameObject::ComponentAdd()
{
    T* ret = new T(scene, id);
    components.push_back(ret);
    return ret;
}


template<typename T>
T* ITUGameObject::ComponentGet()
{
    for(ITUComponent* c : components)
    {
        T* ret = dynamic_cast<T*>(c);
        if(ret)
            return ret;
    }
    return nullptr;
}

template<typename T>
bool ITUGameObject::ComponentRemove()
{
    for(ITUComponent* c : components)
    {
        T* component = dynamic_cast<T*>(c);
        if(component)
        {
            component->invalidated = true;
            return true;
        }
    }
    return false;
}

#endif // ITU_GAME_OBJECT_HPP