#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

#include "camera.hpp"
#include "glprogram.hpp"
#include "obb_renderer.hpp"
#include "obbcd.hpp"
#include "obbcd_data.hpp"
#include "scene.hpp"
#include "shadow_map_renderer.hpp"

const int width = 1600;
const int height = 1200;

std::unique_ptr<Camera> camera;

bool obb_view = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->on_mouse_movement(xpos, ypos);
}

// Process keys that don't depend on the press duration
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_F:
            obb_view = !obb_view;
            break;
        }
    }
}


int main()
{
    if (glfwInit() != GLFW_TRUE)
        return -1;

    // Use OpenGL 4.5 (forward compatibility removes deprecated functionality)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Computer Graphics Lab 2", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; // use GL 3.2+ core context
    if (auto err{glewInit()}; err != GLEW_OK)
    {
        std::cerr << "glewInit() error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    TextureLoader tex_loader("../data");
    Scene scene("../data/scene1.fbx", tex_loader);
    scene.instantiate_meshes();

    glm::vec3 camera_position = scene["Camera"].position();
    camera = std::make_unique<Camera>(width, height, camera_position);

    GlProgram program({{"../shader/main.vert", GL_VERTEX_SHADER}, {"../shader/main.frag", GL_FRAGMENT_SHADER}});

    glm::vec3 lights[2] = {scene["Moonlight"].position(), scene["Mac Screen Light"].position()};
    glm::vec3 light_ambient_colors[2] = {glm::vec3(0.15f, 0.15f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f)};
    glm::vec3 light_diffuse_colors[2] = {glm::vec3(0.8f, 0.8f, 0.85f), glm::vec3(1.0f, 1.0f, 1.0)};
    glm::vec3 spotlight_direction[2] = {
        glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(-(
                                         scene["Mac Screen Light"].position() -
                                         scene["Mac Screen Light Direction"].position()))}; //::vec3(1.0f, 0.0f, 0.0f)};
    glm::vec2 spotlight_cutoff_cos[2] = {
        glm::vec2(0.0f),
        glm::vec2(glm::cos(glm::radians(85.0f)) /* outer */, glm::cos(glm::radians(12.0f)) /* inner */)};
    glm::vec3 spotlight_attenuation[2] = {
        glm::vec3(0.0f), glm::vec3(1.0f /* constant */, 0.01f /* linear */, 0.01f /* quadratic */)};

    program.use();
    program.set_uniform_array("light_position", lights, 2);
    program.set_uniform_array("light_ambient_color", light_ambient_colors, 2);
    program.set_uniform_array("light_diffuse_color", light_diffuse_colors, 2);
    program.set_uniform_array("spotlight_direction", spotlight_direction, 2);
    program.set_uniform_array("spotlight_attenuation", spotlight_attenuation, 2);
    program.set_uniform_array("spotlight_cutoff_cos", spotlight_cutoff_cos, 2);

    const glm::mat4 moonlight_projection =
        glm::ortho(-16.5f, 32.0f, -11.0f, 34.5f, 1.0f /* near plane */, 100.0f /* far plane */);
    const glm::mat4 moonlight_view = glm::lookAt(scene["Moonlight"].position(), glm::vec3(0.0f), CAMERA_UP);
    const glm::mat4 moonlight_vp = moonlight_projection * moonlight_view;

    const glm::mat4 spotlight_projection =
        glm::perspective(40.0f, 1.0f /* shadow map textures are square */, 1.0f, 100.0f);
    const glm::mat4 spotlight_view =
        glm::lookAt(scene["Mac Screen Light"].position(), scene["Mac Screen Light Direction"].position(), CAMERA_UP);
    const glm::mat4 spotlight_vp = spotlight_projection * spotlight_view;

    ShadowMapRenderer shadow_maps({{lights[0], moonlight_vp}, {lights[1], spotlight_vp}});
    const auto& shadow_tex_vps = shadow_maps.shadow_tex_vp_matrices();
    program.set_uniform_array("shadow_tex_vps", shadow_tex_vps.data(), shadow_tex_vps.size());

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    OBBCollisionDetection obbcd(BOUNDING_BOXES, NUM_BOUNDING_BOXES, "BB Observer");
    OBBRenderer obb_renderer;

    int handle_k = 0;
    int handle_k_scaler = 0;

    while (!glfwWindowShouldClose(window))
    {
        shadow_maps.draw(scene);

        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (obb_view)
        {
            obb_renderer.draw(obbcd, camera->vp_matrix());
        }
        else
        {
            program.use();
            program.set_uniform("vp", camera->vp_matrix());

            program.set_uniform("object_texture", 0); // GL_TEXTURE0

            int num_shadow_maps = shadow_maps.textures().size();
            GLint shadow_samplers[num_shadow_maps];
            for (int i = 0; i < num_shadow_maps; ++i)
            {
                shadow_samplers[i] = 1 + i; // GL_TEXTURE1 + i
                glActiveTexture(GL_TEXTURE0 + shadow_samplers[i]);
                glBindTexture(GL_TEXTURE_2D, shadow_maps.textures()[i]);
            }
            program.set_uniform_array("shadow_maps", shadow_samplers, num_shadow_maps);

            program.set_uniform("object_light_override", glm::vec3(0.0f));

            for (const auto& m : scene.models())
            {
                program.set_uniform("model", m.transform());
                program.set_uniform("model_normal", m.normal_transformation());

                if (m.name() == "Mac Screen")
                {
                    program.set_uniform("object_light_override", glm::vec3(0.8f));
                    m.draw();
                    program.set_uniform("object_light_override", glm::vec3(0.0f));
                }
                else if (m.name() == "Door" || m.name() == "Handles" || m.name() == "Handle Levers")
                {
                    if (++handle_k_scaler == 2)
                    {
                        handle_k++;
                        handle_k_scaler = 0;
                    }

                    const auto& animation = m.animations()[0];
                    if (handle_k >= animation.transformation_keys.size())
                        handle_k = 0;

                    const auto& frame = animation.transformation_keys[handle_k];

                    program.set_uniform("model", frame);
                    m.draw();
                }
                else
                {
                    m.draw();
                }
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        glm::vec3 movement{0, 0, 0};
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            movement += CAMERA_FWD;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            movement -= CAMERA_FWD;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            movement += CAMERA_RIGHT;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            movement -= CAMERA_RIGHT;
        if (glm::length(movement) > 0.0f)
        {
            glm::vec3 delta = camera->update_position(movement);
            obbcd.update_observer_position(delta);
            if (obbcd.has_collisions())
            {
                glm::vec3 rev_delta = camera->update_position(-movement);
                obbcd.update_observer_position(rev_delta);
            }
        }
    }

    glfwTerminate();
}
