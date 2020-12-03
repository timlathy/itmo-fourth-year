#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

#include "camera.hpp"
#include "glprogram.hpp"
#include "scene.hpp"
#include "shadow_map_renderer.hpp"

const int width = 1600;
const int height = 1200;

std::unique_ptr<Camera> camera;

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
    Scene scene("../data/scene.fbx", tex_loader);
    scene.instantiate_meshes();

    glm::vec3 camera_position = scene["Camera"].position();
    camera = std::make_unique<Camera>(width, height, camera_position);

    GlProgram program({{"../shader/main.vert", GL_VERTEX_SHADER}, {"../shader/main.frag", GL_FRAGMENT_SHADER}});

    glm::vec3 lights[2] = {scene["Moonlight"].position(), scene["Mac Screen"].position()};
    glm::vec3 light_ambient_colors[2] = {glm::vec3(0.15, 0.15, 0.2), glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 light_diffuse_colors[2] = {glm::vec3(0.8, 0.8, 0.85), glm::vec3(0.0f, 0.0f, 0.0)};
    bool light_is_directional[2] = {false, true};

    program.use();
    program.set_uniform_array("light_position", lights, 2);
    program.set_uniform_array("light_ambient_color", light_ambient_colors, 2);
    program.set_uniform_array("light_diffuse_color", light_diffuse_colors, 2);
    program.set_uniform_array("light_is_directional", light_is_directional, 2);

    const glm::mat4 moonlight_projection =
        glm::ortho(-16.5f, 32.0f, -11.0f, 34.5f, 1.0f /* near plane */, 100.0f /* far plane */);
    ShadowMapRenderer shadow_maps({{lights[0], moonlight_projection}});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        shadow_maps.draw(scene);

        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
        program.set_uniform("view_proj", camera->vp_matrix());
        program.set_uniform("shadow_tex_view_proj", shadow_maps.shadow_tex_vp_matrices()[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadow_maps.textures()[0]);

        program.set_uniform("object_texture", 0); // GL_TEXTURE0
        program.set_uniform("shadow_map", 1);     // GL_TEXTURE1

        for (const auto& m : scene.models())
        {
            program.set_uniform("model", m.transform());
            program.set_uniform("model_normal", m.normal_transform());

            m.draw();
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
            camera->on_key_movement(movement);
    }

    glfwTerminate();
}
