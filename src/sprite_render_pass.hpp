#pragma once

#include <vector>

#include <SDL3/SDL_gpu.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "texture.hpp"

struct Sprite
{
    GPUTexture texture;
    glm::mat4 model;
};

class SpriteRenderPass
{
    struct Uniforms
    {
        glm::mat4 camera, model;
    };

    SDL_GPUDevice *m_device;
    SDL_GPUGraphicsPipeline *m_pipeline{nullptr};

    SpriteRenderPass() = delete;
    SpriteRenderPass(const SpriteRenderPass &) = delete;
    SpriteRenderPass &operator=(const SpriteRenderPass &) = delete;
    SpriteRenderPass(SpriteRenderPass &&) = delete;
    SpriteRenderPass &operator=(SpriteRenderPass &&) = delete;

  public:
    SpriteRenderPass(SDL_GPUDevice *device) : m_device(device)
    {
    }

    ~SpriteRenderPass()
    {
        if (m_pipeline)
        {
            SDL_ReleaseGPUGraphicsPipeline(m_device, m_pipeline);
            spdlog::trace("SpriteRenderPass::~SpriteRenderPass: released sprite render pipeline");
        }
    }

    [[nodiscard]] bool init(SDL_GPUTextureFormat swapchain_texture_format);

    void render(
        SDL_GPUCommandBuffer *cmd_buffer, SDL_GPUTexture *target_texture, glm::mat4 &camera,
        const std::vector<Sprite> &sprites
    ) const;
};
