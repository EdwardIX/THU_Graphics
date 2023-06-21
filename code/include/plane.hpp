#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include "utils.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {}

    Plane(const Vector3f &norm, float d_, Material *m, 
        const Vector3f &texture_up = Vector3f::UP, const Vector3f &texture_conf = Vector3f(0, 0, 100)) : 
            Object3D(m), normal(norm.normalized()), d(d_ / norm.length()), configs(texture_conf) {

        right = Vector3f::cross(texture_up, normal); 
        if(!O(right.length())) {
            // raise("Warning: Plane norm and up vector are on the same line\n");
            right = Vector3f::cross(Vector3f::FORWARD, normal);
        }
        right.normalize();
        up = Vector3f::cross(normal, right);
    }

    ~Plane() override = default;

    bool intersect(const Ray &ray, Hit &hit, float tmin) override {
        // solve: dot(ray.getOrigin() + t*ray.getDirection() , norm) == d
        float fac = Vector3f::dot(ray.getDirection(), normal);
        if(!O(fac)) return false;
        float thit = (d - Vector3f::dot(ray.getOrigin(), normal)) / fac;
        if(thit < tmin || thit > hit.getT()) return false;
        float u,v; 
        getuv(u, v, ray.pointAtParameter(thit));
        hit.set(thit, material, normal, ray.pointAtParameter(thit), u, v);
        hit.disturbNormal(right, up);
        return true;
    }

protected:
    Vector3f normal;
    float d;

    // for texture only
    Vector3f up, right, configs;

    void getuv(float &u, float &v, const Vector3f &p) {
        Vector3f dir = p - d * normal;
        u = (Vector3f::dot(dir, right) - configs[0]) / configs[2];
        v = (Vector3f::dot(dir, up) - configs[1]) / configs[2];
    }
};

#endif //PLANE_H
		

