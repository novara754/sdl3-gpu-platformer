#version 450

layout(set = 1, binding = 0) uniform UBO {
	mat4 camera;
	mat4 model;
} ubo;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 tex_coords;

vec2 positions[6] = vec2[](
    vec2(1.0, 1.0),   // top-right
    vec2(-1.0, 1.0),  // top-left
    vec2(1.0, -1.0),  // bottom-right
    vec2(-1.0, 1.0),  // top-left
    vec2(-1.0, -1.0), // bottom-left
    vec2(1.0, -1.0)   // bottom-right
);

vec3 color[6] = vec3[](
    vec3(1.0, 1.0, 1.0), // top-right
    vec3(1.0, 0.0, 0.0), // top-left
    vec3(0.0, 1.0, 0.0), // bottom-right
    vec3(1.0, 0.0, 1.0), // top-left
    vec3(0.0, 0.0, 0.0), // bottom-left
    vec3(0.0, 1.0, 1.0)  // bottom-right
);

vec2 uv[6] = vec2[](
    vec2(1.0, 0.0), // top-right
    vec2(0.0, 0.0), // top-left
    vec2(1.0, 1.0), // bottom-right
    vec2(0.0, 0.0), // top-left
    vec2(0.0, 1.0), // bottom-left
    vec2(1.0, 1.0)  // bottom-right
);

void main() {
    gl_Position = ubo.camera * ubo.model * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    frag_color = color[gl_VertexIndex];
    tex_coords = uv[gl_VertexIndex];
}
