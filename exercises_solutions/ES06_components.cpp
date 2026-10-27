// additional rapidjson include to format error codes as strings
#include <rapidjson/error/en.h>

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

#include "components/ComponentController2DAsteroids.hpp"
#include "components/ComponentShooting2D.hpp"

int main(void)
{
    EngineConfig config;
    EngineContext* context = new EngineContext();

    context = new EngineContext();
    SDL_memset(context, 0, sizeof(EngineContext));


    ResourceIndex<SDL_Texture*>::Init(context);

    itu_lib_context_init(&config, context);
    itu_lib_imgui_setup(context, false);

    rapidjson::Document d;
    if(!itu_load_json("data/e06_data.json", d))
    {
        rapidjson::ParseErrorCode a = d.GetParseError();
        const char* err = rapidjson::GetParseError_En(a);
        SDL_LogError(0, "error parsing json data: [%d] %s", a, err);
    }

    // still perfectly valid to register mappings in code
    itu_lib_input_set_mapping_keyboard(context, SDLK_TAB, BTN_TYPE_DEBUG_RESET);

    if(d.HasMember("keybindings"))
        itu_lib_input_set_mappings(context, d["keybindings"]);

    if(d.HasMember("textures"))
        ResourceIndex<SDL_Texture*>::LoadAll(d["textures"]);

    // register components
    ITUComponent::RegisterComponent(-1, ITUComponentTransform2D::Create);
    ITUComponent::RegisterComponent(-2, ITUComponentSprite::Create);
    ITUComponent::RegisterComponent(1, ComponentController::Create);
    ITUComponent::RegisterComponent(2, ComponentShooting::Create);

    ITUScene scene(context, "scene");
    scene.Init(d);
    scene.Reset();

    bool quit = false;
    itu_lib_context_frame_timing_setup(context);
    while(!quit)
    {
        quit = itu_lib_input_process_events(context);

        if(context->btn_isjustpressed[BTN_TYPE_DEBUG_RESET])
            scene.Reset();

        SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);
        SDL_RenderClear(context->renderer);

        itu_lib_imgui_frame_begin(context);

        scene.Update(context->delta);
        scene.UpdateFixed();
        scene.Render();

        itu_lib_imgui_frame_end(context);
        SDL_RenderPresent(context->renderer);
        itu_lib_context_frame_timing_update(context);
    }

    delete context;

    return 0;
}