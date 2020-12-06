#pragma once

// Oriented Bounding Box Collision Detection
// https://stackoverflow.com/a/52010428

#include <glm/glm.hpp>

#include <algorithm>
#include <string>
#include <vector>

struct OrientedBoundingBox
{
    std::string name;
    glm::vec3 position, rotation_x, rotation_y, rotation_z, half_size;
};

class OBBCollisionDetection
{
  private:
    std::vector<OrientedBoundingBox> _bboxes;
    std::vector<OrientedBoundingBox> _interaction_bboxes;
    OrientedBoundingBox _observer_bbox;

  public:
    OBBCollisionDetection(
        std::vector<OrientedBoundingBox> bboxes, std::vector<OrientedBoundingBox> interaction_bboxes,
        OrientedBoundingBox observer)
        : _bboxes(bboxes), _interaction_bboxes(interaction_bboxes), _observer_bbox(observer)
    {
    }
    const std::vector<OrientedBoundingBox>& bounding_boxes() const
    {
        return _bboxes;
    }
    const std::vector<OrientedBoundingBox>& interaction_boxes() const
    {
        return _interaction_bboxes;
    }
    const OrientedBoundingBox& observer_box() const
    {
        return _observer_bbox;
    }
    void update_box(const OrientedBoundingBox& new_box)
    {
        std::replace_if(
            _bboxes.begin(), _bboxes.end(), [name = new_box.name](const auto& b) { return b.name == name; }, new_box);
    }
    void update_observer_position(glm::vec3 delta)
    {
        _observer_bbox.position += delta;
    }
    bool has_collisions() const;
    std::optional<OrientedBoundingBox> interaction_collision() const;

    static bool check_collision(const OrientedBoundingBox& box1, const OrientedBoundingBox& box2);
};
