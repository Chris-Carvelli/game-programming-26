#ifndef ITU_COMPONENT_TRANSFORM_2D_HPP
#define ITU_COMPONENT_TRANSFORM_2D_HPP

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

class ITUComponentTransform2D : public ITUComponent
{
public:
    using ITUComponent::ITUComponent;

    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ITUComponentTransform2D(scene, owner_id); }

    void Init(const rapidjson::Value& config) override;
    // ~ITUComponentTransform() override {}

    Transform2D transform;
};

#endif //ITU_COMPONENT_TRANSFORM_2D_HPP