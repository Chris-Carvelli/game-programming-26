#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

ITUComponent::ComponentKVP* ITUComponent::hm_component_factory = NULL;

// void ITUComponent::Init()
// {
//     static int id_count = 100;
//     id = ++id_count;
//     if(parent)
//         SDL_Log("%s::ITUComponent::init()\n", parent->DebugNameGet().c_str());
// }

ITUGameObject* ITUComponent::OwnerGet()
{
    return scene->GameObjectPtrGet(owner_id);
}

ITUGameObjectId ITUComponent::OwnerIdGet()
{
    return owner_id;
}

EngineContext* ITUComponent::ContextGet()
{
    return scene->ContextGet();
}

ITUScene* ITUComponent::SceneGet()
{
    return scene;
}

ITUComponent* ITUComponent::CreateComponentOfType(Uint64 component_type, ITUScene* scene, ITUGameObjectId owner_id)
{
    int loc = stbds_hmgeti(hm_component_factory, component_type);
    if (loc == -1)
        return NULL;

    fn_component_constructor_t fn_create = hm_component_factory[loc].value;

    return fn_create(scene, owner_id);;
}