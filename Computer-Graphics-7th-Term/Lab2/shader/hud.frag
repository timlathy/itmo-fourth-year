#version 450 core

layout (location = 0) out vec4 out_color;

in vec2 frag_uv;

uniform sampler2D tex;

void main()
{
    vec4 t = texture(tex, frag_uv);
    if (t.a == 0.0)
        discard;
    out_color = t;
}
