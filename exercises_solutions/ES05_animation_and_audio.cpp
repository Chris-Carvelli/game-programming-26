#include <itu_engine.hpp>

const char* const PATH_MUSIC[] =
{
    "data/opengameart.org/music_level_0.ogg",
    "data/opengameart.org/music_level_1.ogg"
};

const char* const PATH_SFXS[] =
{
    "data/kenney/SFX/footstep00.ogg",
    "data/kenney/SFX/footstep01.ogg"
};

struct Player
{
    vec2f position;
    float velocity;
    float direction;

    Transform2D transform;
    Sprite      sprite;

    int    anim_curr; // 0: idle, 1: walk
    Uint64 anim_speed_ms;
    int    anim_curr_frame_idx;
    Uint64 anim_last_flip;
};

struct Npc
{
    Transform2D transform;
    Sprite      sprite;

    vec2f offset;
    vec2f p0;
    vec2f p1;

    Uint64 anim_start;
    Uint64 anim_speed_ms;
    EasingFunction easing;
};

struct GameState
{
    SDL_Texture* tex_atlas_player;
    SDL_Texture* tex_atlas_space;

    MIX_Mixer* mixer;
    MIX_Audio* audio_music[array_size(PATH_MUSIC)];
    MIX_Audio* audio_sfxs[array_size(PATH_SFXS)];

    MIX_Track* track_main_bg;
    MIX_Track* track_sfx;
    float music_gain;
    float master_gain;
    float sfx_gain;

    int music_current;
    int sfx_current;

    Player player;
    Npc npcs[3];
};

SDL_FRect player_rect_idle = { 0, 0, 96, 128 };
SDL_FRect player_rect_walk = { 0, 512, 96, 128 };

void game_init  (EngineContext* context, GameState* state);
void game_reset (EngineContext* context, GameState* state);
void game_update(EngineContext* context, GameState* state);
void game_render(EngineContext* context, GameState* state);
void game_debug (EngineContext* context, GameState* state);

void update_player(EngineContext* context, GameState* state);
void update_npcs  (EngineContext* context, GameState* state);

int main(void)
{
    bool quit = false;
    EngineConfig  config;
    config.camera_pixel_per_unit = 96;
    config.texture_pixels_per_unit = 96;
    EngineContext context = { 0 };
    GameState     state   = { 0 };

    itu_lib_context_init(&config, &context);
    
    itu_lib_imgui_setup(&context, false);


    game_init (&context, &state);
    game_reset(&context, &state);

    itu_lib_context_frame_timing_setup(&context);
    while(!quit)
    {
        quit = itu_lib_input_process_events(&context);

        itu_lib_imgui_frame_begin(&context);

        SDL_SetRenderDrawColor(context.renderer, 0x0C, 0x42, 0xA1, 0x00);
        SDL_RenderClear(context.renderer);

        if(context.btn_isjustpressed[BTN_TYPE_DEBUG_RESET])
            game_reset(&context, &state);

        game_update(&context, &state);
        game_render(&context, &state);

        if(context.debug_ui_show)
            game_debug(&context, &state);

        itu_lib_imgui_frame_end(&context);
        SDL_RenderPresent(context.renderer);

        itu_lib_context_frame_timing_update(&context);
    }

    return 0;
}

void game_init(EngineContext* context, GameState* state)
{
    // input bindings
    itu_lib_input_set_mapping_keyboard(context, SDLK_W    , BTN_TYPE_UP);
    itu_lib_input_set_mapping_keyboard(context, SDLK_S    , BTN_TYPE_DOWN);
    itu_lib_input_set_mapping_keyboard(context, SDLK_A    , BTN_TYPE_LEFT);
    itu_lib_input_set_mapping_keyboard(context, SDLK_D    , BTN_TYPE_RIGHT);
    itu_lib_input_set_mapping_keyboard(context, SDLK_SPACE, BTN_TYPE_SPACE);
    itu_lib_input_set_mapping_keyboard(context, SDLK_F1   , BTN_TYPE_DEBUG_F1);
    itu_lib_input_set_mapping_keyboard(context, SDLK_F2   , BTN_TYPE_DEBUG_F2);
    itu_lib_input_set_mapping_keyboard(context, SDLK_F3   , BTN_TYPE_DEBUG_F3);
    itu_lib_input_set_mapping_keyboard(context, SDLK_TAB  , BTN_TYPE_DEBUG_RESET);

    // load resources
    state->tex_atlas_player = itu_resources_texture_create(
        context,
        "data/kenney/character_femalePerson_sheet.png",
        SDL_SCALEMODE_LINEAR
    );
    state->tex_atlas_space = itu_resources_texture_create(
        context,
        "data/kenney/simpleSpace_tilesheet_2.png",
        SDL_SCALEMODE_LINEAR
    );

    SDL_VALIDATE(MIX_Init());
    SDL_VALIDATE(state->mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL));
    SDL_VALIDATE(MIX_SetMixerGain(state->mixer, 0.25f));

    for(Uint64 i = 0; i < array_size(PATH_MUSIC); ++i)
        SDL_VALIDATE(state->audio_music[i] = MIX_LoadAudio(state->mixer, PATH_MUSIC[i], false));

    for(Uint64 i = 0; i < array_size(PATH_SFXS); ++i)
        SDL_VALIDATE(state->audio_sfxs[i] = MIX_LoadAudio(state->mixer, PATH_SFXS[i], false));

    SDL_VALIDATE(state->track_main_bg = MIX_CreateTrack(state->mixer));
    SDL_VALIDATE(state->track_sfx = MIX_CreateTrack(state->mixer));
}

void game_reset(EngineContext* context, GameState* state)
{
    // reset context
    context->camera_default.world_position = VEC2F_ZERO;

    state->player.transform.scale = VEC2F_ONE;
    state->player.transform.position = { 2, -2 };
    itu_lib_sprite_init(&state->player.sprite, state->tex_atlas_player, player_rect_idle);
    state->player.anim_speed_ms = 100;
    state->player.anim_curr_frame_idx = 0;
    state->player.anim_last_flip = SDL_GetTicks();

    Uint64 anim_start = SDL_GetTicks();
    for(int i = 0; i < 3; ++i)
    {
        Npc* npc = &state->npcs[i];
        npc->transform.scale = VEC2F_ONE * 0.5f;
        npc->transform.position = { 0, (float)i };
        itu_lib_sprite_init(&npc->sprite, state->tex_atlas_space, { 5*128.0f, i*128.0f, 128.0f, 128.0f });
        npc->offset = { 3, 0 };

        npc->p0 = npc->transform.position;
        npc->p1 = npc->transform.position + npc->offset;
        npc->anim_speed_ms = 1000;
        npc->anim_start = anim_start;
        npc->easing = EASING_LINEAR;
    }

    state->master_gain = 0.5f;
    state->sfx_gain    = 0.1f;
    state->music_gain  = 0.2f;
    state->music_current = 0;
    state->sfx_current   = 0;

    SDL_VALIDATE(MIX_SetTrackAudio(state->track_main_bg, state->audio_music[state->music_current]));
    SDL_VALIDATE(MIX_SetTrackGain(state->track_main_bg, state->music_gain));
    SDL_VALIDATE(MIX_PlayTrack(state->track_main_bg, 0));

    SDL_VALIDATE(MIX_SetTrackAudio(state->track_sfx, state->audio_sfxs[state->sfx_current]));
    SDL_VALIDATE(MIX_SetTrackGain(state->track_sfx, state->sfx_gain));
}

void game_update(EngineContext* context, GameState* state)
{
    update_player(context, state);
    update_npcs  (context, state);
}

void game_render(EngineContext* context, GameState* state)
{
    itu_lib_sprite_render(context, &state->player.sprite, &state->player.transform);

    for(int i = 0; i < 3; ++i)
    {
        Npc* npc = &state->npcs[i];
        itu_lib_sprite_render(context, &npc->sprite, &npc->transform);
    }
}

void game_debug(EngineContext* context, GameState* state)
{
    ImGui::Begin("game", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(300, 600));
    ImGui::SetWindowPos(ImVec2(0, 0));

    ImGui::SeparatorText("Music");
    {
        if(ImGui::Combo("Music Track", &state->music_current, PATH_MUSIC, array_size(PATH_MUSIC)))
            SDL_VALIDATE(MIX_SetTrackAudio(state->track_main_bg, state->audio_music[state->music_current]));
        if(ImGui::Combo("SFX Track", &state->sfx_current, PATH_SFXS, array_size(PATH_SFXS)))
            SDL_VALIDATE(MIX_SetTrackAudio(state->track_sfx, state->audio_sfxs[state->sfx_current]));

        if(ImGui::SliderFloat("master gain", &state->master_gain, 0.0f, 1.0f))
            SDL_VALIDATE(MIX_SetMixerGain(state->mixer, state->master_gain));

        ImGui::SliderFloat("music gain", &state->music_gain, 0.0f, 1.0f);
            SDL_VALIDATE(MIX_SetTrackGain(state->track_main_bg, state->music_gain));

        ImGui::SliderFloat("sfx gain", &state->sfx_gain, 0.0f, 1.0f);
            SDL_VALIDATE(MIX_SetTrackGain(state->track_sfx, state->sfx_gain));

    }


    ImGui::SeparatorText("Player");
    {
        ImGui::PushID(-1);
        ImGui::DragFloat2("Pos", &state->player.transform.position.x);
        ImGui::DragFloat ("Rot", &state->player.transform.rotation);
        ImGui::DragFloat2("Scale", &state->player.transform.scale.x);
        ImGui::DragInt   ("Anim Speed", (int*)&state->player.anim_speed_ms);
        ImGui::DragInt   ("Frame Idx", &state->player.anim_curr_frame_idx);
        ImGui::PopID();
    }

    for(int i = 0; i < 3; ++i)
    {
        Npc* npc = &state->npcs[i];
        char buf[32];
        SDL_snprintf(buf, 32, "NPC %d", i);
        ImGui::SeparatorText(buf);
        ImGui::PushID(i);
        ImGui::DragFloat2("Pos"  , &npc->transform.position.x);
        ImGui::DragFloat ("Rot"  , &npc->transform.rotation);
        ImGui::DragFloat2("Scale", &npc->transform.scale.x);
        ImGui::DragFloat2("Anim offset", &npc->offset.x);
        ImGui::DragInt   ("Anim Speed", (int*)&npc->anim_speed_ms);
        ImGui::Combo("Easing", (int*)&npc->easing, easing_names, array_size(easing_names));
        ImGui::PopID();

        vec2f pos_a = itu_lib_context_point_global_to_screen(context, npc->p0);
        vec2f pos_b = itu_lib_context_point_global_to_screen(context, npc->p1);
        itu_lib_render_screen_point(context->renderer, pos_a, 5, COLOR_YELLOW);
        itu_lib_render_screen_point(context->renderer, pos_b, 5, COLOR_YELLOW);
    }

    ImGui::End();
}

void update_player(EngineContext* context, GameState* state)
{
    const float SPEED = 2.0f;
    float dir = 0.0f;
    if(context->btn_isdown[BTN_TYPE_LEFT])  { dir -= 1.0f; state->player.sprite.flip_horizontal = true;  }
    if(context->btn_isdown[BTN_TYPE_RIGHT]) { dir += 1.0f; state->player.sprite.flip_horizontal = false; }

    state->player.direction = dir;
    state->player.velocity = dir * SPEED;

    state->player.transform.position.x += state->player.velocity * context->delta;

    Uint64 ticks = SDL_GetTicks();
    if(!(context->btn_isdown[BTN_TYPE_LEFT] || context->btn_isdown[BTN_TYPE_RIGHT]))
        // idle
       state->player.sprite.rect = player_rect_idle;
    else if(context->btn_isjustpressed[BTN_TYPE_LEFT] || context->btn_isjustpressed[BTN_TYPE_RIGHT])
    {
        state->player.anim_last_flip = ticks;
        state->player.sprite.rect = player_rect_walk;
        state->player.anim_curr_frame_idx = 0;
    }
    else if(ticks - state->player.anim_last_flip > state->player.anim_speed_ms)
    {
        state->player.anim_last_flip = ticks;
        state->player.anim_curr_frame_idx = (state->player.anim_curr_frame_idx + 1) % 8;
        state->player.sprite.rect.x = state->player.anim_curr_frame_idx * 96;

        if(state->player.anim_curr_frame_idx == 1 || state->player.anim_curr_frame_idx == 5)
            SDL_VALIDATE(MIX_PlayTrack(state->track_sfx, 0));
    }
}

void update_npcs(EngineContext* context, GameState* state)
{
    Uint64 ticks = SDL_GetTicks();
    for(int i = 0; i < 3; ++i)
    {
        Npc* npc = &state->npcs[i];

        float dur = ticks - npc->anim_start;
        float t = dur / npc->anim_speed_ms;
        float v = easing(t, npc->easing);
        npc->transform.position = lerp(npc->p0, npc->p1, v);

        if(dur > npc->anim_speed_ms)
        {
            npc->anim_start = ticks;
            vec2f tmp = npc->p0;
            npc->p0 = npc->p1;
            npc->p1 = tmp;
        }
    }
}