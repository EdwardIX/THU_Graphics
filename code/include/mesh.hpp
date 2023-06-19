#ifndef MESH_H
#define MESH_H

#include <vector>
#include "bvhtree.hpp"
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"

struct TriangleIndex {
    TriangleIndex() { x[0] = 0; x[1] = 0; x[2] = 0; }

    int &operator[](const int i) { return x[i]; }
    // By Computer Graphics convention, counterclockwise winding is front face
    int x[3]{};
};

class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);
    ~Mesh(){}
    
    std::vector<Vector3f> v;
    std::vector<TriangleIndex> t;
    BVHTree bvh;
    bool intersect(const Ray &r, Hit &h, float tmin) override;
};

#endif
