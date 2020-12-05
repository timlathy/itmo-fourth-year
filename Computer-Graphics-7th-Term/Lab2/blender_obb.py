import bpy
from math import *
import mathutils

selected = bpy.context.selected_objects

bb_collection = next((c for c in bpy.data.collections if c.name == "BB"), None)
if bb_collection is None:
    bb_collection = bpy.data.collections.new("BB")
    bpy.context.scene.collection.children.link(bb_collection)
    for obj in selected:
        bpy.ops.object.origin_set(type="ORIGIN_GEOMETRY", center="BOUNDS")
        bpy.ops.mesh.primitive_cube_ad
        bbox = bpy.context.active_object
        bbox.name = "BB " + obj.name
        bbox.dimensions = obj.dimensions
        bbox.location = obj.location
        bbox.rotation_euler = obj.rotation_euler
        bb_collection.objects.link(bbox)

for obj in bb_collection.objects:
    om = obj.matrix_world.to_3x3()

    rot = om.to_euler('XYZ')
    rot.z = -rot.z
    rot = rot.to_matrix()

    posx, posy, posz = obj.location
    posy = -posy

    hsx, hsy, hsz = obj.dimensions / 2

    print(f"{{\"{obj.name}\", " +
        f"glm::vec3({posx}, {posz}, {posy}), " +
        f"glm::vec3({rot[0][0]}, {rot[2][0]}, {rot[1][0]}), " +
        f"glm::vec3({rot[0][2]}, {rot[2][2]}, {rot[1][2]}), " +
        f"glm::vec3({rot[0][1]}, {rot[2][1]}, {rot[1][1]}), " +
        f"glm::vec3({hsx}, {hsz}, {hsy})}},")
