#include "sprite_render_pass.hpp"

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

#include "SDL3/SDL_gpu.h"
#include "ecs.hpp"
#include "read_file.hpp"

bool SpriteRenderPass::init(SDL_GPUTextureFormat swapchain_texture_format)
{
    std::vector<uint8_t> vertex_shader_code, fragment_shader_code;
    try
    {
        vertex_shader_code = read_file("../src/sprite.vert.bin");
        fragment_shader_code = read_file("../src/sprite.frag.bin");
    }
    catch (std::exception &e)
    {
        spdlog::error("SpriteRenderPass::init: failed to read shader code: {}", e.what());
        return false;
    }
    spdlog::trace("SpriteRenderPass::init: read vertex and fragment shader code");

    SDL_GPUShaderCreateInfo vertex_shader_create_info{
        .code_size = vertex_shader_code.size(),
        .code = vertex_shader_code.data(),
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 2,
        .props = 0,
    };
    SDL_GPUShader *vertex_shader =
        SDL_CreateGPUShader(m_context->device, &vertex_shader_create_info);
    if (!vertex_shader)
    {
        spdlog::error("SpriteRenderPass::init: failed to create vertex shader");
        return false;
    }
    spdlog::trace("SpriteRenderPass::init: created vertex shader module");

    SDL_GPUShaderCreateInfo fragment_shader_create_info{
        .code_size = fragment_shader_code.size(),
        .code = fragment_shader_code.data(),
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
        .num_samplers = 1,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0,
        .props = 0,
    };
    SDL_GPUShader *fragment_shader =
        SDL_CreateGPUShader(m_context->device, &fragment_shader_create_info);
    if (!fragment_shader)
    {
        spdlog::error("SpriteRenderPass::init: failed to create fragment shader");
        return false;
    }
    spdlog::trace("SpriteRenderPass::init: created fragment shader module");

    SDL_GPUColorTargetDescription color_target_description{
        .format = swapchain_texture_format,
        .blend_state =
            {
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = SDL_GPU_BLENDOP_ADD,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
                .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                .color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
                                    SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A,
                .enable_blend = true,
                .enable_color_write_mask = false,
                .padding1 = 0,
                .padding2 = 0,
            },
    };
    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info{
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
    m_pipeline = SDL_CreateGPUGraphicsPipeline(m_context->device, &pipeline_create_info);
    if (!m_pipeline)
    {
        spdlog::error(
            "SpriteRenderPass::init: failed to create graphics pipeline: {}",
            SDL_GetError()
        );
        return false;
    }
    spdlog::trace("SpriteRenderPass::init: created graphics pipeline");

    SDL_ReleaseGPUShader(m_context->device, vertex_shader);
    SDL_ReleaseGPUShader(m_context->device, fragment_shader);

    return true;
}

void SpriteRenderPass::render(
    SDL_GPUCommandBuffer *cmd_buffer, SDL_GPUTexture *target_texture, glm::mat4 &camera,
    const entt::registry &entities
) const
{
    SDL_GPUColorTargetInfo color_target_info{
        .texture = target_texture,
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
        SDL_BeginGPURenderPass(cmd_buffer, &color_target_info, 1, nullptr);
    assert(render_pass);
    {
        SDL_BindGPUGraphicsPipeline(render_pass, m_pipeline);

        auto sprites = entities.view<const Transform, const Sprite>();
        for (const auto [entity, transform, sprite] : sprites.each())
        {
            Uniforms uniforms{
                .camera = camera,
                .model = transform.to_matrix(),
            };
            SDL_PushGPUVertexUniformData(cmd_buffer, 0, &uniforms, sizeof(uniforms));
            SDL_GPUTextureSamplerBinding texture_sampler_binding =
                m_context->texture_registry.get(sprite.texture_id).get_binding();
            SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);
            SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
        }
    }
    SDL_EndGPURenderPass(render_pass);
}
