#include <itu_game_objects.hpp>

ITUScene::ITUScene(EngineContext* context, const char* name)
    : name(name)
    , context(context)
    , root(ITUGameObjectId::NULL_ID)
{
    game_objects.reserve(MAX_GAME_OBJECTS);
    game_objects.resize(1);
    game_objects[0].scene = this;
    game_objects[0].id = { 0, 0 };
    game_objects[0].parent_id = ITUGameObjectId::NULL_ID;
    game_objects[0].name = SDL_strdup("_ROOT");
    root = game_objects[0].id;
}

ITUScene::~ITUScene()
{
}

ITUGameObjectId ITUScene::GameObjectCreate(ITUGameObjectId parent, const char* name)
{
    if(!parent.IsValid())
        parent = root;

    ITUGameObjectId id { 0, 0 };
    if(game_objects_free_list.size() > 0)
    {
        id = game_objects_free_list[game_objects_free_list.size() - 1];
        id.generation++;
    }
    else
    {
        SDL_assert(game_objects.size() < game_objects.capacity());

        id.index = game_objects.size();
        game_objects.emplace_back(ITUGameObject());
        game_objects[id.index].scene = this;
        game_objects[id.index].id = id;
    }
    SDL_free(game_objects[id.index].name);
    game_objects[id.index].parent_id = parent;
    game_objects[id.index].name = SDL_strdup(name);

    game_objects[parent.index].children.push_back(id);
    return id;
}

bool ITUScene::GameObjectRemove(ITUGameObjectId game_object)
{
    ITUGameObject* obj = GameObjectPtrGet(game_object);

    if(!obj)
        return false;

    obj->id.Invalidate();
    obj->Clear();

    return true;
}

ITUGameObject* ITUScene::GameObjectPtrGet(ITUGameObjectId game_object)
{

    if(!game_object.IsValid())
        return nullptr;

    // NOTE we are never shrinking `game_objects`, so if we should never have a valid id that
    //      points to a out-of-bounds location
    assert(game_object.index < game_objects.size());

    if(game_object != game_objects[game_object.index].id)
        return nullptr;

    return &game_objects[game_object.index];
}

EngineContext* ITUScene::ContextGet()
{
    return context;
}

void ITUScene::Init(rapidjson::Document& scene)
{
    // store a copy of the scene config for reset
    config.CopyFrom(scene, scene.GetAllocator());
}

void ITUScene::Reset()
{
    context->uptime = 0;
    game_objects_free_list.clear();

    for(size_t i = 1; i < game_objects.size(); ++i)
    {
        game_objects[i].id.Invalidate();
        game_objects[i].Clear();
    }

    ITUGameObject* ptr_root = GameObjectPtrGet(root);
    assert(ptr_root);

    const rapidjson::Value& objects_config = config["objects"].GetArray();
    size_t object_configs = objects_config.Size();

    // NOTE vector::reserve allocates enough memory to hold `object_configs + 1` objects but
    //      doesn't call their constructors (like `resize` does)
    game_objects.reserve(object_configs + 1);

    for(size_t i = 0; i < object_configs; ++i)
    {
        const rapidjson::Value& curr_object_config = objects_config[i];
        ITUGameObjectId parent_id(curr_object_config["parent_id"]);
        ITUGameObjectId new_id = GameObjectCreate(parent_id, "");
        // TODO make object creation return pointer and id, and make the public API a wrapper
        //      that returns the Id only

        game_objects[new_id.index].Init(objects_config[i]);
        game_objects[new_id.index].id = new_id;
        game_objects[new_id.index].parent_id = parent_id;
    }
}

void ITUScene::Update(float delta)
{
    ITUGameObject* ptr_root = GameObjectPtrGet(root);
    assert(ptr_root);
    ptr_root->Update(delta);
}

void ITUScene::UpdateFixed()
{
    ITUGameObject* ptr_root = GameObjectPtrGet(root);
    assert(ptr_root);
    ptr_root->UpdateFixed();
}

void ITUScene::Render()
{
    ITUGameObject* ptr_root = GameObjectPtrGet(root);
    assert(ptr_root);
    ptr_root->Render();
}
