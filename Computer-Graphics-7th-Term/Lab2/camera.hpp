#pragma once

#include <glm/glm.hpp>

#include "model.hpp"

// Y is the vertical axis (X is horizontal and Z is perpendicular to the screen)
static const glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
static const glm::vec3 CAMERA_FWD = glm::vec3(0.0f, 0.0f, 1.0f);
static const glm::vec3 CAMERA_RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);

class Camera
{
  private:
    float _field_of_view;
    float _aspect_ratio;
    glm::vec3 _camera_world_pos;
    glm::vec3 _camera_direction{0, 0, 0};
    float _last_x, _last_y;
    bool _mouse_captured{false};
    int _animation_frame{-1};

  public:
    Camera(int view_width, int view_height, const glm::vec3& position);
    glm::mat4 vp_matrix() const;
    void on_mouse_movement(double xpos, double ypos);
    glm::vec3 update_position(glm::vec3 direction);

    bool animate_position(
        const std::vector<AnimationFrame>& frames, const std::vector<AnimationFrame>& frames_direction);
};
