#include <cassert>

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "engine.hpp"

int main()
{
    spdlog::set_level(spdlog::level::trace);

    SDL_SetAppMetadata("Platformer", "0.1", nullptr);
    spdlog::trace("main: set sdl app metadata");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        spdlog::error("main: failed to initialize sdl video subsystem: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("main: initialized sdl video subsystem");

    SDL_Window *window = SDL_CreateWindow("Platformer", WIDTH, HEIGHT, 0);
    if (!window)
    {
        spdlog::error("main: failed to create window and renderer: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("main: created sdl window");

    SDL_GPUDevice *device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device)
    {
        spdlog::error("main: failed to create gpu device: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }
    spdlog::trace("main: created sdl gpu device");
    spdlog::info("main: using graphics backend: {}", SDL_GetGPUDeviceDriver(device));

    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        spdlog::error("main: failed to claim window for gpu device: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("main: claimed window for gpu device");

    {
        Engine engine(window, device);
        if (engine.init())
        {
            engine.run();
        }
        else
        {
            spdlog::error("main: failed to initialize engine");
        }
    }

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);

    spdlog::trace("main: process terminating...");
    return 0;
}
