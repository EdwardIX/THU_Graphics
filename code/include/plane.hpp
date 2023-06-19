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
    Plane() {

    }

    Plane(const Vector3f &norm, float d_, Material *m) : Object3D(m) {
        normal = norm / norm.length();
        d = d_ / norm.length();
    }

    ~Plane() override = default;

    bool intersect(const Ray &ray, Hit &hit, float tmin) override {
        // solve: dot(ray.getOrigin() + t*ray.getDirection() , norm) == d
        float fac = Vector3f::dot(ray.getDirection(), normal);
        if(!O(fac)) return false;
        float thit = (d - Vector3f::dot(ray.getOrigin(), normal)) / fac;
        if(thit < tmin || thit > hit.getT()) return false;
        hit.set(thit, material, normal, ray.pointAtParameter(thit));
        return true;
    }

protected:

    Vector3f normal;
    float d;
};

#endif //PLANE_H
		

