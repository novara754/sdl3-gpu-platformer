#include "renderer.hpp"
#include "texture.hpp"

#include <SDL3/SDL_video.h>
#include <spdlog/spdlog.h>

bool Renderer::init(SDL_Window *window)
{
    m_window = window;

    m_gpu_context.device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!m_gpu_context.device)
    {
        spdlog::error("Renderer::init: failed to create gpu device: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }
    spdlog::trace("Renderer::init: created sdl gpu device");
    spdlog::info(
        "Renderer::init: using graphics backend: {}",
        SDL_GetGPUDeviceDriver(m_gpu_context.device)
    );

    if (!SDL_ClaimWindowForGPUDevice(m_gpu_context.device, window))
    {
        spdlog::error("Renderer::init: failed to claim window for gpu device: {}", SDL_GetError());
        return false;
    }
    spdlog::trace("Renderer::init: claimed window for gpu device");

    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);
    if (!m_sprite_render_pass
             .init(SDL_GetGPUSwapchainTextureFormat(m_gpu_context.device, m_window), width, height))
    {
        spdlog::error("Renderer::init: failed to initialize sprite render pass");
        return false;
    }
    spdlog::trace("Renderer::init: initialized sprite render pass");

    return true;
}

void Renderer::render(const entt::registry &entities)
{
    SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(m_gpu_context.device);
    if (!cmd_buf)
    {
        spdlog::error("Renderer::render: failed to acquire gpu command buffer: {}", SDL_GetError());
        return;
    }

    SDL_GPUTexture *swapchain_texture;
    if (!SDL_AcquireGPUSwapchainTexture(cmd_buf, m_window, &swapchain_texture, nullptr, nullptr))
    {
        spdlog::error(
            "Renderer::render: failed to acquire gpu swapchain texture: {}",
            SDL_GetError()
        );
        return;
    }

    m_sprite_render_pass.render(cmd_buf, swapchain_texture, m_camera, entities);

    SDL_SubmitGPUCommandBuffer(cmd_buf);
}

[[nodiscard]] size_t Renderer::new_texture_from_file(const std::string &path)
{
    return m_gpu_context.textures.add(GPUTexture::from_file(m_gpu_context.device, path));
}
