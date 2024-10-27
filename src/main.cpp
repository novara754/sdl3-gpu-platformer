#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>

#include <SDL3/SDL.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

std::vector<uint8_t> read_file(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        spdlog::error("read_file: failed to open file {}", path);
        throw std::runtime_error("failed to open file");
    }
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(file_size, 0);
    file.read(reinterpret_cast<char *>(data.data()), file_size);

    return data;
}

int main()
{
    spdlog::set_level(spdlog::level::trace);

    SDL_SetAppMetadata("Platformer", "0.1", nullptr);
    spdlog::trace("set sdl app metadata");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        spdlog::error("failed to initialize sdl video subsystem: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("initialized sdl video subsystem");

    SDL_Window *window = SDL_CreateWindow("Platformer", 1280, 720, 0);
    if (!window)
    {
        spdlog::error("failed to create window and renderer: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("created sdl window");

    SDL_GPUDevice *device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device)
    {
        spdlog::error("failed to create gpu device: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }
    spdlog::trace("created sdl gpu device");
    spdlog::info("using graphics backend: {}", SDL_GetGPUDeviceDriver(device));

    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        spdlog::error("failed to claim window for gpu device: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("claimed window for gpu device");

    std::vector<uint8_t> vertex_shader_code, fragment_shader_code;
    try
    {
        vertex_shader_code = read_file("../src/triangle.vert.bin");
        fragment_shader_code = read_file("../src/triangle.frag.bin");
    }
    catch (std::exception &e)
    {
        spdlog::error("failed to read shader code: {}", e.what());
        return 1;
    }
    spdlog::trace("read vertex and fragment shader code");

    SDL_GPUShaderCreateInfo vertex_shader_create_info{
        .code_size = vertex_shader_code.size(),
        .code = vertex_shader_code.data(),
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0,
        .props = 0,
    };
    SDL_GPUShader *vertex_shader = SDL_CreateGPUShader(device, &vertex_shader_create_info);
    if (!vertex_shader)
    {
        spdlog::error("failed to create vertex shader");
        return 1;
    }
    spdlog::trace("created vertex shader module");

    SDL_GPUShaderCreateInfo fragment_shader_create_info{
        .code_size = fragment_shader_code.size(),
        .code = fragment_shader_code.data(),
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0,
        .props = 0,
    };
    SDL_GPUShader *fragment_shader = SDL_CreateGPUShader(device, &fragment_shader_create_info);
    if (!fragment_shader)
    {
        spdlog::error("failed to create fragment shader");
        return 1;
    }
    spdlog::trace("created fragment shader module");

    SDL_GPUTextureFormat swapchain_texture_format =
        SDL_GetGPUSwapchainTextureFormat(device, window);
    SDL_GPUColorTargetDescription color_target_description{
        .format = swapchain_texture_format,
        .blend_state =
            {
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_COLOR,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_DST_COLOR,
                .color_blend_op = SDL_GPU_BLENDOP_ADD,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA,
                .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                .color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
                                    SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A,
                .enable_blend = false,
                .enable_color_write_mask = false,
                .padding1 = 0,
                .padding2 = 0,
            },
    };
    SDL_GPUGraphicsPipelineCreateInfo create_info{
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .vertex_input_state =
            {
                .vertex_buffer_descriptions = nullptr,
                .num_vertex_buffers = 0,
                .vertex_attributes = nullptr,
                .num_vertex_attributes = 0,
            },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state =
            {
                .fill_mode = SDL_GPU_FILLMODE_FILL,
                .cull_mode = SDL_GPU_CULLMODE_NONE,
                .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
                .depth_bias_constant_factor = 0.0,
                .depth_bias_clamp = 0.0,
                .depth_bias_slope_factor = 0.0,
                .enable_depth_bias = false,
                .enable_depth_clip = false,
                .padding1 = 0,
                .padding2 = 0,
            },
        .multisample_state = {},
        .depth_stencil_state = {},
        .target_info =
            {
                .color_target_descriptions = &color_target_description,
                .num_color_targets = 1,
                .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID,
                .has_depth_stencil_target = false,
                .padding1 = 0,
                .padding2 = 0,
                .padding3 = 0,
            },
        .props = 0,
    };
    SDL_GPUGraphicsPipeline *triangle_pipeline =
        SDL_CreateGPUGraphicsPipeline(device, &create_info);
    if (!triangle_pipeline)
    {
        spdlog::error("failed to create triangle graphics pipeline: {}", SDL_GetError());
        return 1;
    }
    spdlog::trace("created triangle graphics pipeline");

    spdlog::trace("entering main loop");
    while (true)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                break;
            }
        }

        SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(device);
        if (!cmd_buf)
        {
            spdlog::error("failed to acquire gpu command buffer: {}", SDL_GetError());
            return 1;
        }

        SDL_GPUTexture *swapchain_texture;
        if (!SDL_AcquireGPUSwapchainTexture(cmd_buf, window, &swapchain_texture, nullptr, nullptr))
        {
            spdlog::error("failed to acquire gpu swapchain texture: {}", SDL_GetError());
            return 1;
        }

        SDL_GPUColorTargetInfo color_target_info{
            .texture = swapchain_texture,
            .mip_level = 0,
            .layer_or_depth_plane = 0,
            .clear_color = {.r = 0.0, .g = 0.0, .b = 0.0, .a = 1.0},
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .resolve_texture = nullptr,
            .resolve_mip_level = 0,
            .resolve_layer = 0,
            .cycle = false,
            .cycle_resolve_texture = false,
            .padding1 = 0,
            .padding2 = 0,
        };
        SDL_GPURenderPass *render_pass =
            SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, nullptr);
        assert(render_pass);
        {
            SDL_BindGPUGraphicsPipeline(render_pass, triangle_pipeline);
            SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
        }
        SDL_EndGPURenderPass(render_pass);

        SDL_SubmitGPUCommandBuffer(cmd_buf);
    }
    spdlog::trace("exited main loop");

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);

    spdlog::trace("process terminating...");
    return 0;
}
