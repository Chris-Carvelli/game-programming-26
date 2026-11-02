#define ITU_SYS_RENDER_3D_IMPLEMENTATION

// additional rapidjson include to format error codes as strings
#include <rapidjson/error/en.h>

#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

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

    // lock mouse
    // NOTE might no be available on your system!
    {
        SDL_VALIDATE(SDL_SetWindowMouseGrab(context->window, true));
        SDL_VALIDATE(SDL_SetWindowRelativeMouseMode(context->window, true));
        SDL_Rect r = { 0, 0, 10, 10, };
        SDL_Log("SDL_SetWindowMouseRect:");
        SDL_VALIDATE(SDL_SetWindowMouseRect(context->window, &r));
    }

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

    GameState state;
    raw_init(context, &state);


    bool quit = false;
    itu_lib_context_frame_timing_setup(context);
    while(!quit)
    {
        quit = itu_lib_input_process_events(context);

        SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);
        SDL_RenderClear(context->renderer);

        itu_lib_imgui_frame_begin(context);
        itu_sys_render3d_frame_begin(context->ctx_rendering);

        raw_update(context, &state);

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

    itu_sys_render3d_set_camera_proj(context->ctx_rendering, glm::perspective(PI_HALF, (float)context->config.window_w/(float)context->config.window_h, 0.01f, 10.0f));
    state->cam_pos = glm::vec3(0, 0, 2);
    state->cam_rot = glm::vec3(0, 0, 0);
}

void raw_update(EngineContext* context, GameState* state)
{
    // const float REVOLUTIONS_PER_SECS = 0.25f;
    // state->model_transform = state->model_transform * glm::eulerAngleY(TAU * REVOLUTIONS_PER_SECS * context->delta);


    const float CAM_SPEED_MOV = 2.5f;
    const float CAM_SPEED_ROT = 0.5f;

    // input
    {
        // 1. get the input movement
        //    (save it in a separate variable, we'll need to manipulate it before storing it)
        glm::vec4 cam_mov = glm::vec4(0, 0, 0, 1);
        if(context->btn_isdown[BTN_TYPE_UP])    cam_mov.z -= CAM_SPEED_MOV * context->delta;
        if(context->btn_isdown[BTN_TYPE_DOWN])  cam_mov.z += CAM_SPEED_MOV * context->delta;
        if(context->btn_isdown[BTN_TYPE_LEFT])  cam_mov.x -= CAM_SPEED_MOV * context->delta;
        if(context->btn_isdown[BTN_TYPE_RIGHT]) cam_mov.x += CAM_SPEED_MOV * context->delta;
        if(context->btn_isdown[BTN_TYPE_ACTION_0]) cam_mov.y -= CAM_SPEED_MOV * context->delta;
        if(context->btn_isdown[BTN_TYPE_ACTION_1]) cam_mov.y += CAM_SPEED_MOV * context->delta;
        // 2. get the input rotation
        //    (no manupulation needed, we can store it directly)
        state->cam_rot.x += CAM_SPEED_ROT * context->mouse_delta.y * context->delta;
        state->cam_rot.y += CAM_SPEED_ROT * context->mouse_delta.x * context->delta;

        // 3. when the player moves "forward", it's relative to the camera rotation
        //    BUT, we are modeling a person moving on a flat plane!
        //    (rotate movement in the Y axis only)
        glm::vec4 cam_translate = glm::eulerAngleY(state->cam_rot.y) * cam_mov;
        state->cam_pos += glm::vec3(cam_translate);
    }

    // 4. position the camera in global space
    //    (first we rotate, then we translate)
    //    NOTE: order of rotations is important! If we swap it, the camera will be tilted on the Z axis too
    glm::mat4 mat_cam_global = glm::translate(state->cam_pos) * glm::eulerAngleYX(state->cam_rot.y, state->cam_rot.x);
    // (alternative, more explicit notation)
    // glm::mat4 mat_view = glm::translate(state->cam_pos) * glm::eulerAngleY(state->cam_rot.y) * glm::eulerAngleX(state->cam_rot.x);

    // 5. after we have the correct camera position in global space, the view transform is simply its inverse
    glm::mat4 mat_cam_view = glm::inverse(mat_cam_global);
    itu_sys_render3d_set_camera_view(context->ctx_rendering, mat_cam_view);

    InstanceData instance_data;
    instance_data.transform_global = state->model_transform;
    itu_sys_render3d_model3d_render(context->ctx_rendering, state->model_data, instance_data);
}
