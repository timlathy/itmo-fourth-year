#include "obbcd.hpp"

bool separating_plane(
    const glm::vec3& rel_pos, const glm::vec3& plane, const OrientedBoundingBox& box1, const OrientedBoundingBox& box2)
{
    return fabs(glm::dot(rel_pos, plane)) > (fabs(glm::dot(box1.rotation_x * box1.half_size.x, plane)) +
                                             fabs(glm::dot(box1.rotation_y * box1.half_size.y, plane)) +
                                             fabs(glm::dot(box1.rotation_z * box1.half_size.z, plane)) +
                                             fabs(glm::dot(box2.rotation_x * box2.half_size.x, plane)) +
                                             fabs(glm::dot(box2.rotation_y * box2.half_size.y, plane)) +
                                             fabs(glm::dot(box2.rotation_z * box2.half_size.z, plane)));
}

bool OBBCollisionDetection::check_collision(const OrientedBoundingBox& box1, const OrientedBoundingBox& box2)
{
    glm::vec3 rel_pos = box2.position - box1.position;

    return !(
        separating_plane(rel_pos, box1.rotation_x, box1, box2) ||
        separating_plane(rel_pos, box1.rotation_y, box1, box2) ||
        separating_plane(rel_pos, box1.rotation_z, box1, box2) ||
        separating_plane(rel_pos, box2.rotation_x, box1, box2) ||
        separating_plane(rel_pos, box2.rotation_y, box1, box2) ||
        separating_plane(rel_pos, box2.rotation_z, box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_x, box2.rotation_x), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_x, box2.rotation_y), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_x, box2.rotation_z), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_y, box2.rotation_x), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_y, box2.rotation_y), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_y, box2.rotation_z), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_z, box2.rotation_x), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_z, box2.rotation_y), box1, box2) ||
        separating_plane(rel_pos, glm::cross(box1.rotation_z, box2.rotation_z), box1, box2));
}

bool OBBCollisionDetection::has_collisions() const
{
    for (const auto& bbox : _bboxes)
    {
        if (check_collision(bbox, _observer_bbox))
            return true;
    }
    return false;
}

std::optional<OrientedBoundingBox> OBBCollisionDetection::interaction_collision() const
{
    for (const auto& bbox : _interaction_bboxes)
    {
        if (check_collision(bbox, _observer_bbox))
            return bbox;
    }
    return {};
}
