#version 450 core

in layout (location = 0) vec3 position;
in layout (location = 1) vec3 normal;
in layout (location = 2) vec2 uv;

uniform mat4 model;
uniform mat4 model_normal;
uniform mat4 view_proj;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;

void main()
{
    frag_pos = vec3(model * vec4(position, 1.0));
    frag_normal = mat3(model_normal) * normal;
    frag_uv = uv;

    gl_Position = view_proj * vec4(frag_pos, 1.0);
}
