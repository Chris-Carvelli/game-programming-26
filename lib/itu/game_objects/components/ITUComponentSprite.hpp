#ifndef ITU_COMPONENT_SPRITE_HPP
#define ITU_COMPONENT_SPRITE_HPP

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

class ITUComponentSprite : public ITUComponent
{
public:
    using ITUComponent::ITUComponent;

    static ITUComponent* Create(ITUScene* scene, ITUGameObjectId owner_id) { return new ITUComponentSprite(scene, owner_id); }

    void Init(const rapidjson::Value& config) override;
    void Render() override;

    Sprite sprite;

// private:
    // ITUComponentSprite(ITUGameObject& game_object) : ITUComponent(game_object) { };

};

#endif // ITU_COMPONENT_SPRITE_HPP