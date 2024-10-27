#include "texture.hpp"

#include "SDL3/SDL_gpu.h"
#include <spdlog/spdlog.h>
#include <stb_image.h>

bool copy_to_texture(
    SDL_GPUDevice *device, void *src_data, uint32_t src_data_len, SDL_GPUTexture *dst_texture,
    uint32_t dst_texture_width, uint32_t dst_texture_height
);

GPUTexture GPUTexture::from_file(SDL_GPUDevice *device, const std::string &path)
{
    int img_width, img_height, img_channels;
    unsigned char *img_data = stbi_load(path.c_str(), &img_width, &img_height, &img_channels, 4);

    if (!img_data)
    {
        spdlog::error("GPUTexture::from_file: failed to open image file `{}`", path);
        throw std::runtime_error("failed to open image file");
    }

    SDL_GPUTextureCreateInfo texture_create_info{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(img_width),
        .height = static_cast<Uint32>(img_height),
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0,
    };
    SDL_GPUTexture *texture = SDL_CreateGPUTexture(device, &texture_create_info);
    if (!texture)
    {
        spdlog::error("GPUTexture::from_file: failed to create texture: {}", SDL_GetError());
        throw std::runtime_error("failed to create texture");
    }

    if (!copy_to_texture(
            device,
            img_data,
            img_width * img_height * img_channels,
            texture,
            img_width,
            img_height
        ))
    {
        spdlog::error("GPUTexture::from_file: failed to copy image data to texture");
        SDL_ReleaseGPUTexture(device, texture);
        throw std::runtime_error("failed to copy image data to texture");
    }

    stbi_image_free(img_data);

    SDL_GPUSamplerCreateInfo sampler_create_info{
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .mip_lod_bias = 0,
        .max_anisotropy = 0,
        .compare_op = SDL_GPU_COMPAREOP_NEVER,
        .min_lod = 0,
        .max_lod = 0,
        .enable_anisotropy = false,
        .enable_compare = false,
        .padding1 = 0,
        .padding2 = 0,
        .props = 0,
    };

    SDL_GPUSampler *sampler = SDL_CreateGPUSampler(device, &sampler_create_info);
    if (!sampler)
    {
        spdlog::error("GPUTexture::from_file: failed to create sampler: {}", SDL_GetError());
        SDL_ReleaseGPUTexture(device, texture);
        throw std::runtime_error("failed to create sampler");
    }

    return GPUTexture(device, texture, sampler, path);
}

[[nodiscard]] SDL_GPUTextureSamplerBinding GPUTexture::get_binding() const noexcept
{
    return SDL_GPUTextureSamplerBinding{
        .texture = m_texture,
        .sampler = m_sampler,
    };
}

bool copy_to_texture(
    SDL_GPUDevice *device, void *src_data, uint32_t src_data_len, SDL_GPUTexture *dst_texture,
    uint32_t dst_texture_width, uint32_t dst_texture_height
)
{
    SDL_GPUTransferBufferCreateInfo transfer_buf_create_info{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = src_data_len,
        .props = 0,
    };
    SDL_GPUTransferBuffer *transfer_buf =
        SDL_CreateGPUTransferBuffer(device, &transfer_buf_create_info);
    if (!transfer_buf)
    {
        spdlog::error("copy_to_texture: failed to create transfer buffer: {}", SDL_GetError());
        return false;
    }
    void *transfer_buf_ptr = SDL_MapGPUTransferBuffer(device, transfer_buf, false);
    if (!transfer_buf_ptr)
    {
        spdlog::error("copy_to_texture: failed to map transfer buffer: {}", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transfer_buf);
        return false;
    }
    std::memcpy(transfer_buf_ptr, src_data, src_data_len);
    SDL_UnmapGPUTransferBuffer(device, transfer_buf);

    SDL_GPUCommandBuffer *copy_cmd_buf = SDL_AcquireGPUCommandBuffer(device);
    if (!copy_cmd_buf)
    {
        spdlog::error("copy_to_texture: failed to create init command buffer: {}", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device, transfer_buf);
        return false;
    }
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(copy_cmd_buf);
    {
        SDL_GPUTextureTransferInfo transfer_info{
            .transfer_buffer = transfer_buf,
            .offset = 0,
            .pixels_per_row = 0,
            .rows_per_layer = 0,
        };
        SDL_GPUTextureRegion destination_info{
            .texture = dst_texture,
            .mip_level = 0,
            .layer = 0,
            .x = 0,
            .y = 0,
            .z = 0,
            .w = dst_texture_width,
            .h = dst_texture_height,
            .d = 1,
        };
        SDL_UploadToGPUTexture(copy_pass, &transfer_info, &destination_info, false);
    }
    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(copy_cmd_buf);

    SDL_ReleaseGPUTransferBuffer(device, transfer_buf);

    return true;
}
