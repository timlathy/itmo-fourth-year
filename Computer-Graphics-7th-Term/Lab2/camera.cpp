#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

const float MOUSE_SENSITIVITY = 0.05f;
const float MOVEMENT_SPEED = 0.16f;

Camera::Camera(int view_width, int view_height, const glm::vec3& position)
{
    _field_of_view = glm::radians(45.0f);
    _aspect_ratio = (float)view_width / (float)view_height;
    _camera_world_pos = position;
    _camera_direction = {0, 0, 0};
}

glm::mat4 Camera::vp_matrix() const
{
    glm::vec3 camera_pos = _camera_world_pos;
    if (_animation_frame == -1)
        camera_pos.y += 0.12f * sinf(_t_head_movement);

    glm::mat4 view = glm::lookAt(
        camera_pos,                     // camera position in world coordinates
        camera_pos + _camera_direction, // camera is centered at this point
        CAMERA_UP                       // vertical axis
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
    if (_animation_frame != -1)
        return;

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

    float pitch = glm::asin(_camera_direction.y);
    float yaw = glm::atan(_camera_direction.z, _camera_direction.x);

    pitch += glm::radians(offset_y * MOUSE_SENSITIVITY);
    pitch = glm::clamp(pitch, glm::radians(-89.0f), glm::radians(89.0f));

    yaw += glm::radians(offset_x * MOUSE_SENSITIVITY);

    _camera_direction.x = cos(pitch) * cos(yaw);
    _camera_direction.y = sin(pitch);
    _camera_direction.z = cos(pitch) * sin(yaw);
    _camera_direction = glm::normalize(_camera_direction);
}

glm::vec3 Camera::movement_delta(glm::vec3 direction) const
{
    if (_animation_frame != -1)
        return glm::vec3(0.0f);

    glm::vec3 horiz_axis = glm::normalize(glm::cross(_camera_direction, CAMERA_UP));
    direction *= MOVEMENT_SPEED;

    glm::vec3 delta(0.0f);
    delta.x = (direction.z * _camera_direction.x) + (direction.x * horiz_axis.x);
    delta.z = (direction.z * _camera_direction.z) + (direction.x * horiz_axis.z);

    return delta;
}

void Camera::update_position(glm::vec3 delta)
{
    _t_head_movement += 0.16f;
    _camera_world_pos += delta;
}

bool Camera::animate_position(
    const std::vector<AnimationFrame>& frames, const std::vector<AnimationFrame>& frames_direction)
{
    if (++_animation_frame >= frames.size())
    {
        _animation_frame = -1;
        return false;
    }

    auto& pos = frames[_animation_frame].transformation[3];
    _camera_world_pos = glm::vec3(pos.x, pos.y, pos.z);

    auto& dir_pos = frames_direction[_animation_frame].transformation[3];
    glm::vec3 dir_pos3 = glm::vec3(dir_pos.x, dir_pos.y, dir_pos.z);
    _camera_direction = glm::normalize(dir_pos3 - _camera_world_pos);

    return true;
}
