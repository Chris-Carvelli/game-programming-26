#ifndef COMPONENT_CONTROLLER_SPACESHIP_HPP
#define COMPONENT_CONTROLLER_SPACESHIP_HPP

#include <itu_game_objects.hpp>

class ComponentControllerSpaceship : public ITUComponent
{
    using ITUComponent::ITUComponent;

public:
    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ComponentControllerSpaceship(scene, owner_id); }
private:
    void Init(const rapidjson::Value& config) override
    {

    }

    void Update(float delta) override
    {
        const float REVOLUTIONS_PER_SECS = 0.25f;
        ITUGameObject* obj;
        ITUComponentTransform3D* transform;

        if(!(obj = OwnerGet()))
            return;

        if(!(transform = obj->ComponentGet<ITUComponentTransform3D>()))
            return;

        glm::mat4 local = transform->LocalGet();
        transform->LocalSet(local * glm::eulerAngleY(TAU * REVOLUTIONS_PER_SECS * delta));
    }
};

#endif // COMPONENT_CONTROLLER_3D_FIRSPERSON_HPP