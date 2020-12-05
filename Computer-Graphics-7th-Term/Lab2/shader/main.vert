#version 450 core

in layout (location = 0) vec3 position;
in layout (location = 1) vec3 normal;
in layout (location = 2) vec2 uv;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 model_normal;
layout (location = 2) uniform mat4 view_proj;
layout (location = 3) uniform mat4 shadow_tex_view_proj;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;
out vec4 frag_shadow_pos;

void main()
{
    frag_pos = vec3(model * vec4(position, 1.0));
    frag_normal = mat3(model_normal) * normal;
    frag_uv = uv;
    frag_shadow_pos = shadow_tex_view_proj * vec4(frag_pos, 1.0);

    gl_Position = view_proj * vec4(frag_pos, 1.0);
}
