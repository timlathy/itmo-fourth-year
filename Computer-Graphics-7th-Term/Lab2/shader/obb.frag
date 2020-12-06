#version 450 core

in vec3 frag_pos;

uniform vec3 color_filter;

out layout (location = 0) vec4 out_color;

void main()
{
    vec3 color = vec3(clamp(frag_pos.y, 0.0, 1.0) * 0.5 + 0.2);
    color *= color_filter;
    out_color = vec4(color, 1.0);
}
