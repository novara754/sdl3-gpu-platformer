#pragma once

#include <vector>

#include <SDL3/SDL_gpu.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "context.hpp"

struct Sprite
{
    size_t texture_id;
    glm::mat4 model;
};

class SpriteRenderPass
{
    struct Uniforms
    {
        glm::mat4 camera, model;
    };

    Context *m_context;
    SDL_GPUGraphicsPipeline *m_pipeline{nullptr};

    SpriteRenderPass() = delete;
    SpriteRenderPass(const SpriteRenderPass &) = delete;
    SpriteRenderPass &operator=(const SpriteRenderPass &) = delete;
    SpriteRenderPass(SpriteRenderPass &&) = delete;
    SpriteRenderPass &operator=(SpriteRenderPass &&) = delete;

  public:
    SpriteRenderPass(Context *context) : m_context(context)
    {
    }

    ~SpriteRenderPass()
    {
        if (m_pipeline)
        {
            SDL_ReleaseGPUGraphicsPipeline(m_context->device, m_pipeline);
            spdlog::trace("SpriteRenderPass::~SpriteRenderPass: released sprite render pipeline");
        }
    }

    [[nodiscard]] bool init(SDL_GPUTextureFormat swapchain_texture_format);

    void render(
        SDL_GPUCommandBuffer *cmd_buffer, SDL_GPUTexture *target_texture, glm::mat4 &camera,
        const std::vector<Sprite> &sprites
    ) const;
};
