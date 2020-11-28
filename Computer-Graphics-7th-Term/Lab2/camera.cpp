#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

const float MOUSE_SENSITIVITY = 0.05f;

Camera::Camera(int view_width, int view_height)
{
    _field_of_view = glm::radians(45.0f);
    _aspect_ratio = (float)view_width / (float)view_height;
    _camera_world_pos = glm::vec3(4.0f, 3.0f, 1.0f);
    _view_pitch = -40;
    _view_yaw = -170;
}

glm::mat4 Camera::vp_matrix() const
{
    // Y is the vertical axis (X is horizontal and Z is perpendicular to the screen)
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 horiz = glm::normalize(glm::cross(_camera_direction, up));
    glm::vec3 camera_up = glm::normalize(glm::cross(horiz, _camera_direction));

    glm::mat4 view = glm::lookAt(
        _camera_world_pos,                     // camera position in world coordinates
        _camera_world_pos + _camera_direction, // camera is centered at this point
        camera_up                              // vertical axis
    );
    glm::mat4 projection = glm::perspective(
        _field_of_view, _aspect_ratio,
        0.1f,  // near plane (a vertex that is closer to camera is clipped)
        100.0f // far plane (a vertex that is further from camera is clipped)
    );
    return projection * view;
}

void Camera::on_mouse_movement(double xpos, double ypos)
{
    if (!_mouse_captured)
    {
        _mouse_captured = true;
        _last_x = xpos;
        _last_y = ypos;
    }

    float offset_x = xpos - _last_x;
    float offset_y = _last_y - ypos;

    _last_x = xpos;
    _last_y = ypos;

    _view_yaw += offset_x * MOUSE_SENSITIVITY;
    _view_pitch += offset_y * MOUSE_SENSITIVITY;
    _view_pitch = std::clamp(_view_pitch, -89.0f, 89.0f);

    _camera_direction.x = cos(glm::radians(_view_yaw)) * cos(glm::radians(_view_pitch));
    _camera_direction.y = sin(glm::radians(_view_pitch));
    _camera_direction.z = sin(glm::radians(_view_yaw)) * cos(glm::radians(_view_pitch));
    _camera_direction = glm::normalize(_camera_direction);
}
