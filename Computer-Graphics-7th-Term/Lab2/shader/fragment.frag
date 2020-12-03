#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;
in vec4 frag_shadow_pos;

uniform vec3 light_position;

uniform sampler2D object_texture;
uniform sampler2DShadow shadow_map;

out layout (location = 0) vec4 out_color;

vec3 ambient_light()
{
    return vec3(0.15, 0.15, 0.2);
}

vec3 diffuse_light(float incoming_light_angle)
{
    return vec3(0.8, 0.8, 0.85) * clamp(incoming_light_angle, 0, 1);
}

float shadow_light(float incoming_light_angle)
{
    float bias = clamp(0.005 * (1.0 - incoming_light_angle), 0, 0.01);
    vec3 coord = frag_shadow_pos.xyz / frag_shadow_pos.w;
    return 0.4 + (0.6 * texture(shadow_map, vec3(coord.xy, coord.z - bias)));
}

void main()
{
    vec3 normal = normalize(frag_normal);
    vec3 light_direction = normalize(light_position - frag_pos);
    float incoming_light_angle = dot(normal, light_direction);

    vec3 lighting = ambient_light();
    lighting += diffuse_light(incoming_light_angle);
    lighting *= shadow_light(incoming_light_angle);

    vec3 color = lighting * texture(object_texture, frag_uv).rgb;
    out_color = vec4(color, 1.0);
}
