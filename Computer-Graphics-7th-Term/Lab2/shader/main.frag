#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;
in vec4 frag_shadow_pos[2];

layout (location = 5) uniform vec3 light_position[2];
layout (location = 7) uniform vec3 light_ambient_color[2];
layout (location = 9) uniform vec3 light_diffuse_color[2];
layout (location = 11) uniform vec3 spotlight_direction[2];
layout (location = 13) uniform vec3 spotlight_attenuation[2]; // (constant, linear, quadratic)
layout (location = 15) uniform vec2 spotlight_cutoff_cos[2]; // (outer, inner)
layout (location = 17) uniform vec3 object_light_override;

uniform sampler2D object_texture;
uniform sampler2DShadow shadow_maps[2];

out layout (location = 0) vec4 out_color;

float shadow_light(sampler2DShadow shadow_map, vec4 shadow_pos, float incoming_light_angle)
{
    float bias = clamp(0.005 * (1.0 - incoming_light_angle), 0, 0.01);
    vec3 coord = shadow_pos.xyz / shadow_pos.w;
    return 0.4 + (0.6 * texture(shadow_map, vec3(coord.xy, coord.z - bias)));
}

float light_source_intensity(int i, vec3 light_direction)
{
    float intensity = 1;

    // Attenuation > 0? This is a spotlight
    if (spotlight_attenuation[i].x > 0)
    {
        float dist = length(light_position[i] - frag_pos);
        float attenuation = spotlight_attenuation[i].x; // constant
        attenuation += spotlight_attenuation[i].y * dist; // linear
        attenuation += spotlight_attenuation[i].z * dist * dist; // quadratic
        float spot_intensity = 1.0 / attenuation;

        vec3 spot_direction = normalize(-spotlight_direction[i]);
        float angle = dot(light_direction, spot_direction);
        if (angle < spotlight_cutoff_cos[i].x) // outer cutoff
        {
            spot_intensity = 0;
        }
        else
        {
            float spot = smoothstep(spotlight_cutoff_cos[i].x, spotlight_cutoff_cos[i].y, angle);
            spot_intensity *= pow(spot, 2);
        }

        intensity *= spot_intensity;
    }

    return intensity;
}

void main()
{
    vec3 normal = normalize(frag_normal);

    vec3 total_reflected = vec3(0, 0, 0);

    if (object_light_override.x > 0)
    {
        total_reflected = object_light_override;
    }
    else
    {
        for (int i = 0; i < 2; ++i)
        {
            vec3 light_direction = normalize(light_position[i] - frag_pos);
            float incoming_light_angle = dot(normal, light_direction);

            float intensity = light_source_intensity(i, light_direction);

            vec3 reflected = light_ambient_color[i];
            reflected += intensity * light_diffuse_color[i] * clamp(incoming_light_angle, 0, 1);
            reflected *= intensity * shadow_light(shadow_maps[i], frag_shadow_pos[i], incoming_light_angle);

            total_reflected += reflected;
        }
    }

    vec3 color = total_reflected * texture(object_texture, frag_uv).rgb;
    out_color = vec4(color, 1.0);
}
