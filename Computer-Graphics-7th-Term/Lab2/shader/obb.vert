#version 450 core

in vec3 position;

uniform mat4 vp;
uniform mat4 model;

out vec3 frag_pos;

void main()
{
    frag_pos = position;
    gl_Position = vp * model * vec4(frag_pos, 1.0);
}
