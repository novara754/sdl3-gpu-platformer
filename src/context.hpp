#pragma once

#include <SDL3/SDL_gpu.h>

#include "registry.hpp"
#include "texture.hpp"

struct Context
{
    SDL_Window *window;
    SDL_GPUDevice *device;
    Registry<GPUTexture> texture_registry;
};