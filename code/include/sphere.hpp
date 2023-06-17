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
        Vector3f link = c - ray.getOrigin();
        float dist = Vector3f::dot(link, link);
        Hit nhit;
        if(dist < r*r) { // inside sphere
            float trt = Vector3f::dot(link, ray.getDirection()) \
                    / Vector3f::dot(ray.getDirection(), ray.getDirection());
            Vector3f rt = ray.pointAtParameter(trt);
            Vector3f d = c - rt;
            float t2 = sqrt(r*r - Vector3f::dot(d, d));

            float thit = trt + t2;
            Vector3f normhit = c - ray.pointAtParameter(thit);
            nhit.set(thit, material, normhit.normalized());
        } else if(dist > r*r) { // outside sphere
            float trt = Vector3f::dot(link, ray.getDirection()) \
                    / Vector3f::dot(ray.getDirection(), ray.getDirection());
            Vector3f rt = ray.pointAtParameter(trt);
            Vector3f d = c - rt;
            if(r*r <= Vector3f::dot(d, d)) return false; // not interset
            float t2 = sqrt(r*r - Vector3f::dot(d, d));

            float thit = trt - t2;
            Vector3f normhit = ray.pointAtParameter(thit) - c;
            nhit.set(thit, material, normhit.normalized());
        } else {
            nhit.set(0, material, (ray.getOrigin() - c).normalized());
        }

        if(nhit.getT() < tmin || nhit.getT() > hit.getT()) return false;
        hit = nhit;
        return true;
    } 

protected:
    Vector3f c;
    float r;
};


#endif
