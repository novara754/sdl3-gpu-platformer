#version 450

layout(location = 0) in vec2 tex_coords;

layout(set = 2, binding = 0) uniform sampler2D tex_sampler;

layout(location = 0) out vec4 out_color;

void main() {
    vec4 color = texture(tex_sampler, tex_coords);
    if (color.a == 0.0) discard;
    out_color = color;
}
