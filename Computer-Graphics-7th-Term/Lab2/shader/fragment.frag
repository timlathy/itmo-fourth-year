#version 450 core

in vec2 frag_uv;

uniform sampler2D tex;

out layout (location = 0) vec4 out_color;

void main()
{
    out_color = texture(tex, frag_uv);
}
