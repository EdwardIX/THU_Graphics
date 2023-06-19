#ifndef BVHTREE_H
#define BVHTREE_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"

class Mesh; // forward declare

struct TriangleInfo {
    Triangle *t;
    Vector3f center, mi, mx;
    TriangleInfo(Triangle &tri);
};

struct BVHNode {
    BVHNode *l, *r;
    Vector3f mi, mx;
    std::vector<Triangle*> ts;
    ~BVHNode() {
        if(l) delete l;
        if(r) delete r;
    }
};

class BVHTree {
private:
    BVHNode *rt;
    std::vector<Triangle> tri;
    void build_recursive(std::vector<TriangleInfo> t, BVHNode *p);
    bool intersect_recursive(const Ray &r, Hit &h, float tmin, BVHNode *p);
public:
    ~BVHTree() {if(rt) delete rt;}
    void build(Mesh &mesh);
    bool intersect(const Ray &r, Hit &h, float tmin);
};

#endif