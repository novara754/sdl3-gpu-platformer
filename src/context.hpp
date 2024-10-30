#pragma once

#include <SDL3/SDL.h>

#include "audio.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "registry.hpp"
#include "texture.hpp"

struct Context
{
    SDL_Window *window;
    SDL_GPUDevice *device;
    Registry<GPUTexture> texture_registry;
    Input input;
    Physics physics;
    Audio audio;
};
