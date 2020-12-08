#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "camera.hpp"
#include "glprogram.hpp"
#include "glwindow.hpp"
#include "hud_renderer.hpp"
#include "obb_renderer.hpp"
#include "obbcd.hpp"
#include "obbcd_data.hpp"
#include "scene.hpp"
#include "shadow_map_renderer.hpp"

const int width = 1600;
const int height = 1200;

int main()
{
    GlWindow window(width, height);

    TextureLoader tex_loader("../data");
    Scene scene("../data/lains-room.fbx", tex_loader);
    scene.instantiate_meshes();

    glm::vec3 camera_position = {-0.8912374973297119, 15.0902, -5.087483882904053}; // scene["Camera"].position();
    Camera camera(width, height, camera_position);

    GlProgram program({{"../shader/main.vert", GL_VERTEX_SHADER}, {"../shader/main.frag", GL_FRAGMENT_SHADER}});

    const glm::vec3 lights[2] = {scene["Moonlight"].position(), scene["Mac Screen Light"].position()};
    const glm::vec3 light_ambient_colors[2] = {glm::vec3(0.2f, 0.2f, 0.25f), glm::vec3(0.15f, 0.15f, 0.15f)};
    const glm::vec3 light_diffuse_colors[2] = {glm::vec3(0.8f, 0.8f, 0.85f), glm::vec3(1.0f, 1.0f, 1.0f)};
    const glm::vec3 spotlight_direction[2] = {
        glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(-(
                                         scene["Mac Screen Light"].position() -
                                         scene["Mac Screen Light Direction"].position()))}; //::vec3(1.0f, 0.0f, 0.0f)};
    const glm::vec2 spotlight_cutoff_cos[2] = {
        glm::vec2(0.0f),
        glm::vec2(glm::cos(glm::radians(85.0f)) /* outer */, glm::cos(glm::radians(12.0f)) /* inner */)};
    const glm::vec3 spotlight_attenuation[2] = {
        glm::vec3(0.0f), glm::vec3(1.0f /* constant */, 0.01f /* linear */, 0.005f /* quadratic */)};

    program.set_uniform_array("light_position", lights, 2);
    program.set_uniform_array("light_ambient_color", light_ambient_colors, 2);
    program.set_uniform_array("light_diffuse_color", light_diffuse_colors, 2);
    program.set_uniform_array("spotlight_direction", spotlight_direction, 2);
    program.set_uniform_array("spotlight_attenuation", spotlight_attenuation, 2);
    program.set_uniform_array("spotlight_cutoff_cos", spotlight_cutoff_cos, 2);

    const glm::mat4 moonlight_projection =
        glm::ortho(-10.0f, 14.0f, -9.0f, 15.0f, 1.0f /* near plane */, 100.0f /* far plane */);
    const glm::mat4 moonlight_view = glm::lookAt(scene["Moonlight"].position(), glm::vec3(0.0f), CAMERA_UP);
    const glm::mat4 moonlight_vp = moonlight_projection * moonlight_view;

    const glm::mat4 spotlight_projection =
        glm::perspective(40.0f, 1.0f /* shadow map textures are square */, 1.0f, 100.0f);
    const glm::mat4 spotlight_view =
        glm::lookAt(scene["Mac Screen Light"].position(), scene["Mac Screen Light Direction"].position(), CAMERA_UP);
    const glm::mat4 spotlight_vp = spotlight_projection * spotlight_view;

    ShadowMapRenderer shadow_maps({{lights[0], moonlight_vp, 2048}, {lights[1], spotlight_vp, 2048}});
    const auto& shadow_tex_vps = shadow_maps.shadow_tex_vp_matrices();
    program.set_uniform_array("shadow_tex_vps", shadow_tex_vps.data(), shadow_tex_vps.size());

    OBBCollisionDetection obbcd(BOUNDING_BOXES, INTERACTION_BOUNDING_BOXES, BB_OBSERVER);
    OBBRenderer obb_renderer;
    HUDRenderer hud_renderer;

    bool obb_view = false;
    bool obb_view_button_pressed = false;

    bool door_opening = false;
    bool door_opened = false;

    bool mac_on = true;
    int mac_power_cycle_frame = -1;

    glm::vec3 mac_screen_light(0.8f);

    glEnable(GL_DEPTH_TEST);
    window.setup_event_loop();
    while (!window.closed())
    {
        window.begin_frame();
        while (window.animation_tick()) // runs at no more than 60 fps
        {
            if (door_opening)
            {
                for (auto& m : scene.models())
                {
                    if (m.name() == "Door")
                        m.play_animation(scene.animation_keys("Door|D Door"));
                    else if (m.name() == "Handles")
                        m.play_animation(scene.animation_keys("Handles|H Door"));
                    else if (m.name() == "Handle Levers")
                        m.play_animation(scene.animation_keys("Handle Levers|HL Door"));
                }

                door_opening = camera.animate_position(
                    scene.animation_keys("Camera|C Door"), scene.animation_keys("Camera Direction|CD Door"));
                if (!door_opening)
                {
                    door_opened = true;
                    obbcd.update_box(BB_DOOR_OPEN);
                    obbcd.update_observer_position(BB_OBSERVER_DOOR_OPEN);
                }
            }
            if (mac_power_cycle_frame != -1)
            {
                const float step = 1.0f / 60.0f;
                const float on_screen_light = 0.8f;
                const float off_screen_light = 0.001f; // see the fragment shader

                float t = step * mac_power_cycle_frame;

                bool turning_off = mac_on;
                float t_light = turning_off ? (1.0f - t) : t;
                float t_no_light = 1.0f - t_light;

                float color = on_screen_light * t_light + off_screen_light * t_no_light;
                mac_screen_light = glm::vec3(color);

                glm::vec3 ambient = light_ambient_colors[1] * t_light;
                glm::vec3 diffuse = light_diffuse_colors[1] * t_light;

                program.set_uniform("light_ambient_color[1]", ambient);
                program.set_uniform("light_diffuse_color[1]", diffuse);

                if (mac_power_cycle_frame++ == 60)
                {
                    mac_on = !mac_on;
                    mac_power_cycle_frame = -1;
                }
            }
        }

        shadow_maps.draw(scene);

        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (obb_view)
        {
            obb_renderer.draw(obbcd, camera.vp_matrix());
        }
        else
        {
            program.use();
            program.set_uniform("vp", camera.vp_matrix());

            program.set_uniform("object_texture", 0); // GL_TEXTURE0

            int num_shadow_maps = shadow_maps.textures().size();
            GLint shadow_samplers[num_shadow_maps];
            for (int i = 0; i < num_shadow_maps; ++i)
            {
                shadow_samplers[i] = 1 + i; // GL_TEXTURE1 + i
                glBindTextureUnit(1 + i, shadow_maps.textures()[i]);
            }
            program.set_uniform_array("shadow_maps", shadow_samplers, num_shadow_maps);

            program.set_uniform("object_light_override", glm::vec3(0.0f));

            for (const auto& m : scene.models())
            {
                program.set_uniform("model", m.transform());
                program.set_uniform("model_normal", m.normal_transformation());

                if (m.name() == "Mac Screen")
                {
                    program.set_uniform("object_light_override", mac_screen_light);
                    m.draw();
                    program.set_uniform("object_light_override", glm::vec3(0.0f));
                }
                else
                {
                    m.draw();
                }
            }
        }

        if (auto interaction{obbcd.interaction_collision()})
        {
            if (!door_opened && !door_opening && interaction->name == "BB Interaction Door")
            {
                hud_renderer.draw(tex_loader.load_texture("tex-hud-door-open.png"));
                if (window.key_pressed(GLFW_KEY_E))
                    door_opening = true;
            }
            if (mac_power_cycle_frame == -1 && interaction->name == "BB Interaction Mac")
            {
                if (mac_on)
                {
                    hud_renderer.draw(tex_loader.load_texture("tex-hud-mac-turn-off.png"));
                }
                else
                {
                    hud_renderer.draw(tex_loader.load_texture("tex-hud-mac-turn-on.png"));
                }
                if (window.key_pressed(GLFW_KEY_E))
                    mac_power_cycle_frame = 0;
            }
        }
        if (!obb_view_button_pressed && window.key_pressed(GLFW_KEY_F))
        {
            obb_view_button_pressed = true;
            obb_view = !obb_view;
        }
        if (obb_view_button_pressed && !window.key_pressed(GLFW_KEY_F))
        {
            obb_view_button_pressed = false;
        }

        window.submit_frame();

        glm::vec3 movement{0, 0, 0};
        if (window.key_pressed(GLFW_KEY_W))
            movement += CAMERA_FWD;
        if (window.key_pressed(GLFW_KEY_S))
            movement -= CAMERA_FWD;
        if (window.key_pressed(GLFW_KEY_D))
            movement += CAMERA_RIGHT;
        if (window.key_pressed(GLFW_KEY_A))
            movement -= CAMERA_RIGHT;
        if (glm::length(movement) > 0.0f)
        {
            glm::vec3 delta = camera.movement_delta(movement);
            obbcd.update_observer_position(delta);
            if (obbcd.has_collisions())
                obbcd.update_observer_position(-delta);
            else
                camera.update_position(delta);
        }

        double mouse_x, mouse_y;
        window.cursor_position(&mouse_x, &mouse_y);
        camera.on_mouse_movement(mouse_x, mouse_y);
    }
}
