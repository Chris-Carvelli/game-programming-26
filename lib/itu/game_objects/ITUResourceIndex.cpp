#include <itu_engine.hpp>
#include <itu_game_objects.hpp>

// template specializations
// TODO move those in specific files, we don't want to include dozens of unrelated files here


template<>
SDL_Texture* ResourceIndex<SDL_Texture*>::DoLoad_Impl(const char* path)
{
    // TODO pass extra data to loader (scalemode)
    const SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR;
    SDL_Log("DoLoad_Impl::SDL_Texture: %s", path);
    return itu_resources_texture_create(context, path, scale_mode);
}

template<>
RawTextureData ResourceIndex<RawTextureData>::DoLoad_Impl(const char* path)
{
    // TODO pass extra data to loader (channels_requested)
    const int channels_requested = 4;
    SDL_Log("DoLoad_Impl::RawTextureData: %s", path);
    RawTextureData texture;
    texture.data = itu_resources_texture_load_raw(
        path,
        channels_requested,
        &texture.width,
        &texture.height,
        &texture.channels
    );
    return texture;
}

template<>
Model3D* ResourceIndex<Model3D*>::DoLoad_Impl(const char* path)
{
    SDL_Log("DoLoad_Impl::Model3D: %s", path);
    return itu_sys_render3d_load_model3d(context->ctx_rendering, path);
}