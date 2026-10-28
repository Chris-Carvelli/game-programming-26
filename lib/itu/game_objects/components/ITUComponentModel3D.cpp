#include <itu_game_objects.hpp>

void ITUComponentModel3D::Init(const rapidjson::Value& config)
{
    EngineContext* context;

    if(!(context = ContextGet()))
        return;

    model_id = config["model_id"].GetInt64();
}

void ITUComponentModel3D::Render()
{
    ITUGameObject* owner;
    EngineContext* context;
    ITUComponentTransform3D* t;

    if(!(owner = OwnerGet()))
        return;

    if(!(context = ContextGet()))
        return;

    if(!(t = owner->ComponentGet<ITUComponentTransform3D>()))
        return;

    InstanceData instance_data;
    instance_data.transform_global = t->GlobalGet();
    Model3D* data;
    if(ResourceIndex<Model3D*>::Get(model_id, data))
        itu_sys_render3d_model3d_render(context->ctx_rendering, data, instance_data);
}
