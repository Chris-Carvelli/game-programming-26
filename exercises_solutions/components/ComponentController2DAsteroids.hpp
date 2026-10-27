#ifndef COMPONENT_CONTROLLER_HPP
#define COMPONENT_CONTROLLER_HPP

#include <itu_game_objects.hpp>

class ComponentController : public ITUComponent
{
    using ITUComponent::ITUComponent;

public:
    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ComponentController(scene, owner_id); }
private:
    float speed_mov;
    float speed_rot;

    void Init(const rapidjson::Value& config) override
    {
        speed_mov = config["speed_mov"].GetFloat();
        speed_rot = config["speed_rot"].GetFloat();
    }

    void Update(float delta) override
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

        float rot = 0;
        float mov = 0;
        if(context->btn_isdown[BTN_TYPE_LEFT])
            rot += 1.0f;
        if(context->btn_isdown[BTN_TYPE_RIGHT])
            rot -= 1.0f;
        if(context->btn_isdown[BTN_TYPE_UP])
            mov += 1.0f;
        if(context->btn_isdown[BTN_TYPE_DOWN])
            mov -= 1.0f;

        t->transform.rotation += rot * speed_rot * delta;
        vec2f offset = rotate(VEC2F_UP, t->transform.rotation) * mov;

        t->transform.position += offset * speed_mov * delta;
    }
};

#endif // COMPONENT_CONTROLLER_HPP