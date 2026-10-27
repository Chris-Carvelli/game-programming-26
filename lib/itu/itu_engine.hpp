// unity build header
// include this only once, and no other header

#ifndef ITU_ENGINE_HPP
#define ITU_ENGINE_HPP

#define ITU_UNITY_BUILD

// SDL: https://www.libsdl.org/
//      https://wiki.libsdl.org/SDL3/FrontPage#satellite-libraries
// "cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

// stb libraries: https://github.com/nothings/stb
// "single-file public domain (or MIT licensed) libraries for C/C++"
#define STB_DS_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_ds.h>
#include <stb_image.h>

// imgui: https://github.com/ocornut/imgui
// "bloat-free graphical user interface library for C++"
// NOTE: intended and designe for **debug** UI and tools (although it's possible to hack for for in-game UI)
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>
#include <imgui/imgui_impl_sdlgpu3.h>
#include <imgui/imgui_impl_sdlgpu3_shaders.h>

// glm: https://github.com/g-truc/glm
// "header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications"
// (ie: similar syntax to the one used in certain shaders, implementation makes it more straightforward to move data
// between CPU and GPU)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>                        // E08
#include <glm/gtc/matrix_transform.hpp>       // E08
#include <glm/gtx/euler_angles.hpp>           // E08
#include <glm/gtx/quaternion.hpp>             // E08
#include <glm/gtx/transform.hpp>              // E08
#include <glm/gtx/matrix_decompose.hpp>       // E08

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>

#include <box2d/box2d.h>

// low level libraries (no engine or memory allocation involved)
#include <itu_common.hpp>
#include <itu_lib_render_screen.hpp>
#include <itu_lib_overlaps.hpp>
#include <itu_lib_transform2d.hpp>

// context libraries (slightly higher-level functionality)
#include <itu_lib_context.hpp>
#include <utils/itu_utils_box2d.hpp>
#include <itu_lib_render2d.hpp>
#include <itu_lib_imgui.hpp>

// NOTE: some static code analysis will throw an error here mentioning "file cannot be included
//       recursively", or something similar. This is not a real error that will happen when we
//       compile (it might have implications on compile times, but we're gonna ignore that here)
//       If it happens to you and that annoys you, I've wrapped these .cpp inclusions to make it
//       easy to silence this. Just define THE `IN_IDE` preprocessor symbol IN THE TOOL ONLY!
#ifndef IN_IDE
	#include <itu_lib_context.cpp>
	#include <utils/itu_utils_box2d.cpp>
	#include <itu_lib_render2d.cpp>
	#include <itu_lib_imgui.cpp>
#endif // IN_IDE

#endif // ITU_ENGINE_HPP