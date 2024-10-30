#pragma once

#include <SDL3/SDL.h>

#include "audio.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "renderer.hpp"

struct Systems
{
    Renderer renderer;
    Input input;
    Physics physics;
    Audio audio;
};
