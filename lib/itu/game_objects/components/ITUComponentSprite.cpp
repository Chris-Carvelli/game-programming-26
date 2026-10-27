#include <itu_game_objects.hpp>

void ITUComponentSprite::Init(const rapidjson::Value& config)
{
    EngineContext* context;

    if(!(context = ContextGet()))
        return;

    vec2f pos  = itu_deserialize_vec2f(config["position"]);
    vec2f size = itu_deserialize_vec2f(config["size"]);
    ResourceIndexId tex_id = config["tex_id"].GetInt64();

    SDL_Texture* tex;
    if(!ResourceIndex<SDL_Texture*>::Get(tex_id, tex))
    {
        SDL_LogError(0, "texture %lu not loaded!", tex_id);
        tex = NULL;
    }
    itu_lib_sprite_init(&sprite, tex, { pos.x, pos.y, size.x, size.y });
}

void ITUComponentSprite::Render()
{
    ITUGameObject* owner;
    EngineContext* context;
    ITUComponentTransform2D* t;

    if(!(owner = OwnerGet()))
        return;

    if(!(context = ContextGet()))
        return;


    if(!(t = owner->ComponentGet<ITUComponentTransform2D>()))
        return;

    itu_lib_sprite_render(context, &sprite, &t->transform);
}
