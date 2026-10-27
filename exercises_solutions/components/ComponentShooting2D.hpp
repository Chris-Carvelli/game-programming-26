#ifndef COMPONENT_SHOOTING_HPP
#define COMPONENT_SHOOTING_HPP

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

class ComponentShooting : public ITUComponent
{
    using ITUComponent::ITUComponent;

public:
    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ComponentShooting(scene, owner_id); }

private:
    const int BULLET_MAX = 1024;
    std::vector<Transform2D> bullet_transforms;
    std::vector<Uint64>      bullet_spawn_times;
    int    bullets_alive;
    Sprite bullet_sprite;
    float  speed;
    Sint32 pool_size;
    Uint64 alive_ms;
    float  cooldown_ms;
    Uint64 time_last_spawn;

    void Init(const rapidjson::Value& config) override
    {
        vec2f pos  = itu_deserialize_vec2f(config["sprite_position"]);
        vec2f size = itu_deserialize_vec2f(config["sprite_size"]);
        ResourceIndexId tex_id = config["tex_id"].GetInt64();
        speed       = config["speed"].GetFloat();
        pool_size   = config["pool_size"].GetInt();
        alive_ms    = config["alive_secs"].GetFloat() * 1000;
        cooldown_ms = config["cooldown_secs"].GetFloat() * 1000;

        SDL_assert(pool_size <= BULLET_MAX);
        bullet_transforms.resize(pool_size);
        bullet_spawn_times.resize(pool_size);

        SDL_Texture* tex;
        if(!ResourceIndex<SDL_Texture*>::Get(tex_id, tex))
        {
            SDL_LogError(0, "texture %lu not loaded!", tex_id);
            tex = NULL;
        }
        itu_lib_sprite_init(&bullet_sprite, tex, { pos.x, pos.y, size.x, size.y });
        bullets_alive = 0;
        time_last_spawn = 0;
    };

    void Update(float delta) override
    {
        EngineContext* context;
        if(!(context = ContextGet()))
                return;

        // despawn
        Uint64 time_curr = SDL_GetTicks();
        for(int i = 0; i < bullets_alive; ++i)
        {
            if(time_curr - bullet_spawn_times[i] > alive_ms)
            {
                // do despawn
                std::swap(bullet_spawn_times[i], bullet_spawn_times[bullets_alive-1]);
                std::swap(bullet_transforms[i], bullet_transforms[bullets_alive-1]);
                --i;
                --bullets_alive;
            }
        }
        // spawn
        // NOTE we are making a conscious decision to ignore user input if our object pool is full
        //      this is probably the WORST way to do this. There are other (ie, replace the oldest
        //      alive instance) which can be better for particles or other stuff, but for gameplay
        //      there is no good solution.
        if(context->btn_isdown[BTN_TYPE_SPACE] && time_curr - time_last_spawn > cooldown_ms && bullets_alive < pool_size)
        {
            ITUGameObject* owner = OwnerGet();
            // NOTE a component can't exist without an owner
            SDL_assert(owner);
            ITUComponentTransform2D* owner_transform = owner->ComponentGet<ITUComponentTransform2D>();

            if(owner_transform)
            {
                bullet_transforms[bullets_alive] = owner_transform->transform;
                bullet_spawn_times[bullets_alive] = SDL_GetTicks();
                ++bullets_alive;
                time_last_spawn = time_curr;
            }
        }

        for(int i = 0; i < bullets_alive; ++i)
        {
            Transform2D* transform = &bullet_transforms[i];
            transform->position = transform->position + rotate(VEC2F_UP, transform->rotation) * speed * delta;
        }
    }

    void Render() override
    {
        EngineContext* context;

        if(!(context = ContextGet()))
            return;

        for(int i = 0; i < bullets_alive; ++i)
        {
            itu_lib_sprite_render(context, &bullet_sprite, &bullet_transforms[i]);
        }
    }
};

#endif // COMPONENT_SHOOTING_HPP
