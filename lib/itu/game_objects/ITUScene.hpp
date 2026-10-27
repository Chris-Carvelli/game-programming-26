#ifndef ITU_SCENE_HPP
#define ITU_SCENE_HPP

#include <itu_game_objects.hpp>

class ITUGameObject;
struct EngineContext;

class ITUScene
{
private:
    // maximum number of GameObjects that can be active at the same time
    const size_t MAX_GAME_OBJECTS = 2048;

public:
    ITUScene(EngineContext* context, const char* name);
    ~ITUScene();

    ITUGameObjectId GameObjectCreate(ITUGameObjectId parent, const char* name);
    bool            GameObjectRemove(ITUGameObjectId game_object);
    ITUGameObject*  GameObjectPtrGet(ITUGameObjectId game_object);

    EngineContext* ContextGet();

    void Init(rapidjson::Document& scene);
    void Reset();
    void Update(float delta);
    void UpdateFixed();
    void Render();
private:
    const char* name;
    EngineContext* context;
    ITUGameObjectId root;
    rapidjson::Document config;
    
    std::vector<ITUGameObject> game_objects;
    std::vector<ITUGameObjectId> game_objects_free_list;

};

#endif // ITU_SCENE_HPP