#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        c = center;
        r = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &ray, Hit &hit, float tmin) override {
        float t1 = Vector3f::dot(c - ray.getOrigin(), ray.getDirection()); 
        Vector3f x = ray.pointAtParameter(t1); // the projection of center on ray
        Vector3f d = c - x; // dist between center and ray
        if(d.squaredLength() >= r*r) return false; // no intersect: dist >= r
        float t2 = sqrt(r*r - d.squaredLength());
        if(t1 - t2 > tmin && t1 - t2 < hit.getT()) {
            Vector3f p = ray.pointAtParameter(t1 - t2);
            hit = Hit(t1 - t2, material, p - c, p);
            return true;
        }
        if(t1 + t2 > tmin && t1 + t2 < hit.getT()) {
            Vector3f p = ray.pointAtParameter(t1 + t2);
            hit = Hit(t1 + t2, material, p - c, p);
            return true;
        }
        return false;
    } 

protected:
    Vector3f c;
    float r;
};


#endif
