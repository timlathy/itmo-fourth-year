#version 450 core

in vec3 frag_pos;

out layout (location = 0) vec4 out_color;

void main()
{
    out_color = vec4(clamp(frag_pos, 0.0, 1.0) * 0.5, 1.0);
}
