// TODO NEXT:
// - ~~[ ] add gameobject serialization? (would need to check out how convert runtime ids to serialized ids)~~
// - [x] test gameObject creation/destruction
// - [ ] integrate 3D rendering from '25
//      - [ ] change ResourceStorage to allow for holding of actual data too (rather than simply handles to stuff managed by SDL)
// - [x] hide/wrap file reading and engine data initializatino (from main to somewhere more reasonable. Maybe the scene? Maybe ANOTHER class?...)
// - [x] clean up includes
// - [ ] review bullet pool code

#define ITU_SYS_RENDER_3D_IMPLEMENTATION

// #define USE_SCENE

// additional rapidjson include to format error codes as strings
#include <rapidjson/error/en.h>

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

#include "components/ComponentControllerSpaceship.hpp"

struct GameState
{
    glm::vec3 cam_pos;
    glm::vec3 cam_rot;

    Model3D*  model_data;
    glm::mat4 model_transform;
};

void raw_init(EngineContext* context, GameState* state);
void raw_update(EngineContext* context,GameState* state);

int main(void)
{
    EngineConfig config;
    config.type_renderer = TYPE_RENDERER_3D;
    EngineContext* context = new EngineContext();

    context = new EngineContext();
    SDL_memset(context, 0, sizeof(EngineContext));

    ResourceIndex<SDL_Texture*>::Init(context);
    ResourceIndex<RawTextureData>::Init(context);
    ResourceIndex<Model3D*>::Init(context);

    itu_lib_context_init(&config, context);
    itu_sys_render3d_init(context);
    itu_lib_imgui_setup(context, false);

    itu_sys_render3d_set_camera_proj(context->ctx_rendering, glm::perspective(PI_HALF, (float)context->config.window_w/(float)context->config.window_h, 0.01f, 10.0f));
    itu_sys_render3d_set_camera_view(context->ctx_rendering, glm::translate(glm::vec3(0, 0, -2)));

    rapidjson::Document d;
    if(!itu_load_json("data/e08_data.json", d))
    {
        rapidjson::ParseErrorCode a = d.GetParseError();
        const char* err = rapidjson::GetParseError_En(a);
        SDL_LogError(0, "error parsing json data: [%d] %s", a, err);
    }
    if(d.HasMember("textures_raw"))
        ResourceIndex<RawTextureData>::LoadAll(d["textures_raw"]);
    if(d.HasMember("models"))
        ResourceIndex<Model3D*>::LoadAll(d["models"]);

    Model3D* model_data;
    SDL_assert(ResourceIndex<Model3D*>::Get(0, model_data));

    // still perfectly valid to register mappings in code
    itu_lib_input_set_mapping_keyboard(context, SDLK_TAB, BTN_TYPE_DEBUG_RESET);

    if(d.HasMember("keybindings"))
        itu_lib_input_set_mappings(context, d["keybindings"]);

    // register components
    // NOTE "engine" components should be registered by the engine. We are being pretty lazy here
    ITUComponent::RegisterComponent(-3, ITUComponentTransform3D::Create);
    ITUComponent::RegisterComponent(-4, ITUComponentModel3D::Create);

#ifdef USE_SCENE
    ITUScene scene(context, "scene");
    scene.Init(d);
    scene.Reset();

        // setup reference object
    ITUGameObjectId obj_id = scene.GameObjectCreate(ITUGameObjectId::NULL_ID, "reference_object");
    {
        ITUGameObject* obj = scene.GameObjectPtrGet(obj_id);
        ITUComponentTransform3D* transform = obj->ComponentAdd<ITUComponentTransform3D>();
        transform->LocalSet(glm::identity<glm::mat4>());
        ITUComponentModel3D* model = obj->ComponentAdd<ITUComponentModel3D>();
        model->model_id = 0;

        obj->ComponentAdd<ComponentControllerSpaceship>();
    }
#else // USE_SCENE
    GameState state;
    raw_init(context, &state);
#endif // USE_SCENE


    bool quit = false;
    itu_lib_context_frame_timing_setup(context);
    while(!quit)
    {
        quit = itu_lib_input_process_events(context);

        SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);
        SDL_RenderClear(context->renderer);

        itu_lib_imgui_frame_begin(context);
        itu_sys_render3d_frame_begin(context->ctx_rendering);

#ifdef USE_SCENE
        scene.Update(context->delta);
        scene.UpdateFixed();
        scene.Render();
#else // USE_SCENE
        raw_update(context, &state);
#endif // USE_SCENE

        itu_sys_render3d_frame_end(context);
        itu_lib_imgui_frame_end(context);
        itu_sys_render3d_frame_submit(context->ctx_rendering);

        itu_lib_context_frame_timing_update(context);
    }

    delete context;

    return 0;
}

void raw_init(EngineContext* context, GameState* state)
{
    const ResourceIndexId MODEL_ID = 0;
    if(!ResourceIndex<Model3D*>::Get(MODEL_ID, state->model_data))
    {
        SDL_LogError(0, "model %lu not loaded", MODEL_ID);
        return;
    }
    state->model_transform = glm::identity<glm::mat4>();
}

void raw_update(EngineContext* context, GameState* state)
{
    const float REVOLUTIONS_PER_SECS = 0.25f;

    state->model_transform = state->model_transform * glm::eulerAngleY(TAU * REVOLUTIONS_PER_SECS * context->delta);
    InstanceData instance_data;
    instance_data.transform_global = state->model_transform;

    itu_sys_render3d_model3d_render(context->ctx_rendering, state->model_data, instance_data);
}
