#pragma once

#include <string>

#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>

struct GPUTexture
{
    SDL_GPUDevice *device{nullptr};
    SDL_GPUTexture *texture{nullptr};
    SDL_GPUSampler *sampler{nullptr};

  public:
    [[nodiscard]] static GPUTexture from_file(SDL_GPUDevice *device, const std::string &path);

    [[nodiscard]] static GPUTexture
    depth_target(SDL_GPUDevice *device, uint32_t width, uint32_t height);

    [[nodiscard]] SDL_GPUTextureSamplerBinding get_binding() const noexcept
    {
        return SDL_GPUTextureSamplerBinding{
            .texture = this->texture,
            .sampler = this->sampler,
        };
    }

    void release(SDL_GPUDevice *device)
    {
        if (this->sampler != nullptr)
        {
            SDL_ReleaseGPUSampler(device, this->sampler);
        }
        if (this->texture != nullptr)
        {
            SDL_ReleaseGPUTexture(device, this->texture);
        }
    }
};
