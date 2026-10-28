#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

void ITUComponentTransform3D::Init(const rapidjson::Value& config)
{
    glm::vec3 pos = itu_deserialize_vec3f(config["position"]);
    glm::vec3 rot = itu_deserialize_vec3f(config["rotation"]);
    glm::vec3 scale = itu_deserialize_vec3f(config["scale"]);
    local = glm::translate(pos) * glm::eulerAngleXYZ(rot.x, rot.y, rot.z) * glm::scale(scale);
}

glm::mat4 ITUComponentTransform3D::LocalGet()
{
    return local;
};

glm::mat4 ITUComponentTransform3D::GlobalGet()
{
    return global;
}

void ITUComponentTransform3D::LocalSet(const glm::mat4& local_new)
{
    local = local_new;

    // recursively apply to all children
    ITUGameObject* ancestor = OwnerGet();
    ITUComponentTransform3D* parent;
    // NOTE first "ancestor" is the transform owner. This should always be valid
    SDL_assert(ancestor);

    // recusively traverse up the tree to find the paent transform
    while((ancestor = ancestor->ParentGet()))
        if((parent = ancestor->ComponentGet<ITUComponentTransform3D>()))
            break;
    if(!parent)
    {
        // this object does not have a parent transform
        global = local;
    }
    else
        GlobalUpdate(parent->global);
}

void ITUComponentTransform3D::GlobalUpdate(const glm::mat4& parent_global)
{
    global = parent_global * local;

    ITUScene* scene = SceneGet();
    ITUGameObject* owner = OwnerGet();
    SDL_assert(scene);
    SDL_assert(owner);

    for(ITUGameObjectId child_id : owner->Children())
    {
        ITUGameObject* child = scene->GameObjectPtrGet(child_id);
        if(!child)
            continue;
        ITUComponentTransform3D* transform = child->ComponentGet<ITUComponentTransform3D>();
        if(!transform)
            continue;

        transform->GlobalUpdate(global);
    }
}
