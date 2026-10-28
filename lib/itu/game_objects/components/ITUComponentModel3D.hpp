#ifndef ITU_COMPONENT_MODEL_3D_HPP
#define ITU_COMPONENT_MODEL_3D_HPP

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

class ITUComponentModel3D : public ITUComponent
{
public:
    using ITUComponent::ITUComponent;

    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ITUComponentModel3D(scene, owner_id); }

    void Init(const rapidjson::Value& config) override;
    void Render() override;

    ResourceIndexId model_id;
};

#endif // ITU_COMPONENT_MODEL_3D