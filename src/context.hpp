#pragma once

#include <SDL3/SDL.h>

#include "physics.hpp"
#include "registry.hpp"
#include "texture.hpp"

struct Context
{
    SDL_Window *window;
    SDL_GPUDevice *device;
    Registry<GPUTexture> texture_registry;
    bool key_states[SDL_SCANCODE_COUNT]{};
    Physics physics;
};
