#ifndef ITU_COMPONENT_TRANSFORM_3D_HPP
#define ITU_COMPONENT_TRANSFORM_3D_HPP

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

class ITUComponentTransform3D : public ITUComponent
{
public:
    using ITUComponent::ITUComponent;

    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ITUComponentTransform2D(scene, owner_id); }

    void Init(const rapidjson::Value& config) override;

    glm::mat4 local;
    glm::mat4 global;
};

#endif //ITU_COMPONENT_TRANSFORM_HPP