#include <itu_game_objects.hpp>

ITUGameObject* ITUGameObject::ParentGet()
{
    return scene->GameObjectPtrGet(parent_id);
}

ITUGameObjectId ITUGameObject::ParentIdGet()
{
    return parent_id;
}

bool ITUGameObject::ParentSet(ITUGameObjectId new_parent_id)
{
    ITUGameObject* p_new = scene->GameObjectPtrGet(new_parent_id);
    ITUGameObject* p_cur = scene->GameObjectPtrGet(parent_id);

    if(p_cur)
    {
        size_t i;
        for(i = 0; i < p_cur->children.size();++i)
            if(p_cur->children[i] == id)
                break;
        if(i < p_cur->children.size())
            p_cur->children.erase(p_cur->children.begin() + i);
    }

    parent_id = new_parent_id;

    if(new_parent_id.IsValid())
    {
        assert(p_new); // if we received a valid parent id the pointer should never be NULL!
        p_new->children.push_back(id);
    }

    return true;
}


void ITUGameObject::Init(const rapidjson::Value& config)
{
    name = SDL_strdup(config["name"].GetString());
    children.clear();

    components.reserve(config["components"].Size());
    for(const rapidjson::Value& component_config : config["components"].GetArray())
    {
        Uint64 component_id = component_config["type"].GetInt64();
        ITUComponent* comp = ITUComponent::CreateComponentOfType(component_id, scene, id);
        comp->Init(component_config);
        components.push_back(comp);
    }
}

void ITUGameObject::Update(float delta)
{
    // NOTE store initial component count. Newly created components will not be updated
    size_t components_count = components.size();
    for(size_t i = 0; i < components_count; ++i)
        if(!components[i]->invalidated)
            components[i]->Update(delta);

    for(size_t i = 0; i < children.size(); ++i)
    {
        ITUGameObject* child = scene->GameObjectPtrGet(children[i]);
        if(child)
            child->Update(delta);
    }
}

void ITUGameObject::UpdateFixed()
{
    // NOTE store initial component count. Newly created components will not be updated
    size_t components_count = components.size();
    for(size_t i = 0; i < components_count; ++i)
        if(!components[i]->invalidated)
            components[i]->UpdateFixed();

    // remove invalidated components
    // (ding it in fixed update to avoid re-iterating too many times)
    for(size_t i = 0; i < components.size(); ++i)
    {
        ITUComponent* component = components[i];
        if(component->invalidated)
        {
            delete component;
            // NOTE moving the last component in place of the current one would be faster, but component
            //      order execution could be important! We decided to keep the array sorted,
            //      but in reality the desired order of execution is not always the same as the
            //      creation order, so we might actually want to implement more refined strategies
            //      (ie, see Unity's script execution order)
            // components[i] = components[components.size()-1];
            // components.resize(components.size()-1);
            components.erase(components.begin() + i);
            --i;
        }
    }

    for(size_t i = 0; i < children.size(); ++i)
    {
        ITUGameObject* child = scene->GameObjectPtrGet(children[i]);
        if(child)
            child->UpdateFixed();
    }

    // remove invalidated children
    // (ding it in fixed update to avoid re-iterating too many times)
    for(size_t i = 0; i < children.size(); ++i)
    {
        if(!children[i].IsValid())
        {
            children.erase(children.begin() + i);
            --i;
        }
    }
}

void ITUGameObject::Render()
{
    // NOTE store initial component count. Newly created components will not be rendered
    size_t components_count = components.size();
    for(size_t i = 0; i < components_count; ++i)
        if(!components[i]->invalidated)
            components[i]->Render();

    for(size_t i = 0; i < children.size(); ++i)
    {
        ITUGameObject* child = scene->GameObjectPtrGet(children[i]);
        if(child)
            child->Render();
    }
}

void ITUGameObject::Clear()
{
    ParentSet(ITUGameObjectId::NULL_ID);
    children.clear();
    for(size_t i = 0; i < components.size(); ++i)
        delete  components[i];
    components.clear();
}

const char* ITUGameObject::DebugNameGet()
{
    return name;
}

const std::vector<ITUGameObjectId>& ITUGameObject::Children()
{
    return children;
}
