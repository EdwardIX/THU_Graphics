#ifndef MESH_H
#define MESH_H

#include <vector>
#include "bvhtree.hpp"
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"

class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);
    ~Mesh(){}
    
    std::vector<Triangle> tri;
    BVHTree bvh;
    bool intersect(const Ray &r, Hit &h, float tmin) override;
};

#endif
