#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;
in vec4 frag_shadow_pos;

layout (location = 4) uniform vec3 light_position[2];
layout (location = 6) uniform vec3 light_ambient_color[2];
layout (location = 8) uniform vec3 light_diffuse_color[2];
layout (location = 10) uniform vec3 spotlight_direction[2];
layout (location = 12) uniform vec3 spotlight_attenuation[2]; // (constant, linear, quadratic)
layout (location = 14) uniform vec2 spotlight_cutoff_cos[2]; // (outer, inner)

uniform sampler2D object_texture;
uniform sampler2DShadow shadow_map[2];

out layout (location = 0) vec4 out_color;

float shadow_light(sampler2DShadow map, float incoming_light_angle)
{
    float bias = clamp(0.005 * (1.0 - incoming_light_angle), 0, 0.01);
    vec3 coord = frag_shadow_pos.xyz / frag_shadow_pos.w;
    return 0.4 + (0.6 * texture(map, vec3(coord.xy, coord.z - bias)));
}

void main()
{
    vec3 normal = normalize(frag_normal);

    vec3 total_reflected = vec3(0, 0, 0);

    for (int i = 1; i < 2; ++i)
    {
        vec3 light_direction = normalize(light_position[i] - frag_pos);

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

        float incoming_light_angle = dot(normal, light_direction);

        vec3 reflected = light_ambient_color[i];
        reflected += intensity * light_diffuse_color[i] * clamp(incoming_light_angle, 0, 1);
        //reflected *= shadow_light(shadow_map[i], incoming_light_angle);

        total_reflected += reflected;
    }

    vec3 color = total_reflected * texture(object_texture, frag_uv).rgb;
    out_color = vec4(color, 1.0);
}
