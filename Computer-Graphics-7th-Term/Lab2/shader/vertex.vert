#version 450 core

in layout (location = 0) vec3 position;
in layout (location = 1) vec3 normal;
in layout (location = 2) vec2 uv;

uniform mat4 mvp;

out vec2 frag_uv;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    frag_uv = uv;
}
