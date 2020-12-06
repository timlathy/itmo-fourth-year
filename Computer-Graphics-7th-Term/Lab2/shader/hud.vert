#version 450 core

in vec3 position;
in vec2 uv;

uniform mat4 mvp;

out vec2 frag_uv;

void main()
{
    frag_uv = uv;
    gl_Position = mvp * vec4(position, 1.0);
}
