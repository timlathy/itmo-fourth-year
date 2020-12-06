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
    OrientedBoundingBox _observer_bbox;

  public:
    OBBCollisionDetection(const OrientedBoundingBox* bboxes, size_t num_bboxes, OrientedBoundingBox observer);
    const std::vector<OrientedBoundingBox>& bounding_boxes() const
    {
        return _bboxes;
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
};
