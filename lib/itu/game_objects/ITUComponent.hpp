#ifndef ITU_COMPONENT_HPP
#define ITU_COMPONENT_HPP

#include <itu_engine.hpp>

#include <itu_game_objects.hpp>

class ITUScene;
class ITUGameObject;
struct EngineContext;
class ITUComponent;

typedef ITUComponent* (*fn_component_constructor_t)(ITUScene* scene, ITUGameObjectId owner_id);

class ITUComponent
{
    friend class ITUGameObject;

    struct ComponentKVP { Uint64 key; fn_component_constructor_t value; };
    static ComponentKVP* hm_component_factory;
public:
     // ~ITUComponent() = 0;
private:
    ITUGameObject* ParentGet();


public:
    ITUComponent(ITUScene* scene, ITUGameObjectId owner_id) : scene(scene), owner_id(owner_id), invalidated(false) { };
    virtual ~ITUComponent() { }
public:
    ITUGameObject*  OwnerGet();
    ITUGameObjectId OwnerIdGet();
    EngineContext*  ContextGet();
    ITUScene*       SceneGet();

    virtual void Init(const rapidjson::Value& config) = 0;
    virtual void Update(float delta) { };
    virtual void UpdateFixed()       { };
    virtual void Render()            { };

// TMP 
public:
// protected:
    static void RegisterComponent(Uint64 component_id, fn_component_constructor_t constructor)
    {
        stbds_hmput(hm_component_factory, component_id, constructor);
    }

    static ITUComponent* CreateComponentOfType(Uint64 component_type, ITUScene* scene, ITUGameObjectId owner_id);


private:
    ITUScene* scene;
    ITUGameObjectId owner_id;
    bool invalidated;
};
#endif // ITU_COMPONENT_HPP