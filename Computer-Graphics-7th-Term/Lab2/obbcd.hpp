#pragma once

// Oriented Bounding Box Collision Detection
// https://stackoverflow.com/a/52010428

#include <glm/glm.hpp>

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
    OBBCollisionDetection(const OrientedBoundingBox* bboxes, size_t num_bboxes, std::string observer_box_name);
    const std::vector<OrientedBoundingBox>& bounding_boxes() const
    {
        return _bboxes;
    }
    const OrientedBoundingBox& obserer_box() const
    {
        return _observer_bbox;
    }
    void update_observer_position(glm::vec3 delta)
    {
        _observer_bbox.position += delta;
    }
    bool has_collisions() const;
};
