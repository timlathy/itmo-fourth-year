#version 450 core

out layout (location = 0) float out_depth;

void main()
{
    out_depth = gl_FragCoord.z;
}
