#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;
in vec4 frag_shadow_pos;

uniform vec3 light_position;
uniform sampler2D tex;
uniform sampler2D shadow_map;

out layout (location = 0) vec4 out_color;

vec3 ambient_light()
{
    return vec3(0.15, 0.15, 0.2);
}

vec3 diffuse_light()
{
    vec3 normal = normalize(frag_normal);
    vec3 light_direction = normalize(light_position - frag_pos);
    float diffuse = max(dot(normal, light_direction), 0.0);
    return vec3(0.8, 0.8, 0.85) * diffuse;
}

float shadow()
{
    float visibility = 1.0;
    float closest_depth_to_light_source = texture(shadow_map, frag_shadow_pos.xy).z;
    float current_depth = frag_shadow_pos.z;
    return (current_depth < closest_depth_to_light_source) ? 0.0 : 0.5;
}

void main()
{
    vec3 lighting = ambient_light() + diffuse_light();
    vec3 color = (1.0 - shadow()) * lighting * texture(tex, frag_uv).rgb;
    out_color = vec4(color, 1.0);
}
