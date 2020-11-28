#pragma once

#include <glm/glm.hpp>

class Camera
{
  private:
    float _field_of_view;
    float _aspect_ratio;
    glm::vec3 _camera_world_pos;
    glm::vec3 _camera_direction{0, 0, 0};
    float _last_x, _last_y;
    float _view_yaw{0}, _view_pitch{0};
    bool _mouse_captured{false};

  public:
    Camera(int view_width, int view_height);
    glm::mat4 vp_matrix() const;
    void on_mouse_movement(double xpos, double ypos);
};
