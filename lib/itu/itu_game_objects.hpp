#ifndef ITU_GAME_OBJECTS_HPP
#define ITU_GAME_OBJECTS_HPP

#include <vector>

#include <game_objects/itu_util_serialization.hpp>

#include <game_objects/ITUGameObjectId.hpp>
#include <game_objects/ITUResourceIndex.hpp>
#include <game_objects/ITUScene.hpp>
#include <game_objects/ITUGameObject.hpp>
#include <game_objects/ITUComponent.hpp>
#include <game_objects/components/ITUComponentTransform2D.hpp>
#include <game_objects/components/ITUComponentTransform3D.hpp>
#include <game_objects/components/ITUComponentSprite.hpp>
#include <game_objects/components/ITUComponentModel3D.hpp>


#ifndef IN_IDE
    #include <game_objects/ITUGameObjectId.cpp>
    #include <game_objects/ITUResourceIndex.cpp>
    #include <game_objects/ITUScene.cpp>
    #include <game_objects/ITUGameObject.cpp>
    #include <game_objects/ITUComponent.cpp>
    #include <game_objects/components/ITUComponentTransform2D.cpp>
    #include <game_objects/components/ITUComponentTransform3D.cpp>
    #include <game_objects/components/ITUComponentSprite.cpp>
    #include <game_objects/components/ITUComponentModel3D.cpp>

#endif // IN_IDE
#endif // ITU_GAME_OBJECTS_HPP