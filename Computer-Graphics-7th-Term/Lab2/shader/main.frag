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
    float sample_bias_spread = 1.0 / 600.0;
    vec2 sample_bias[8] = vec2[](
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870),
        vec2(0.34495938, 0.29387760),
        vec2(-0.91588581, 0.45771432),
        vec2(-0.81544232, -0.87912464),
        vec2(-0.38277543, 0.27676845),
        vec2(0.97484398, 0.75648379)
    );

    float bias = clamp(0.005 * (1.0 - incoming_light_angle), 0, 0.01);

    float light = 0;
    float sample_contribution = 0.6 / 8;
    for (int i = 0; i < 8; ++i)
    {
        vec3 coord = vec3(shadow_pos.xy + sample_bias[i] * sample_bias_spread, shadow_pos.z - bias) / shadow_pos.w;
        float s = texture(shadow_map, coord);
        light += sample_contribution * s;
    }
    return 0.4 + light;
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
