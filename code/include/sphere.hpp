#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() {}

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
        float t2 = sqrt(r*r - d.squaredLength()), thit, u, v;
        if(t1 - t2 > tmin && t1 - t2 < hit.getT())       thit = t1 - t2;
        else if (t1 + t2 > tmin && t1 + t2 < hit.getT()) thit = t1 + t2;
        else return false;

        Vector3f p = ray.pointAtParameter(thit), U, V;
        getuv(u, v, p, U, V);
        hit.set(thit, material, p - c, p, u, v);
        hit.disturbNormal(U, V);
        return true;
    } 

protected:
    Vector3f c;
    float r;
    
    void getuv(float &u, float &v, const Vector3f &p, Vector3f &U, Vector3f &V) {
        Vector3f dir = (p - c) / r;
        float phi = std::acos(dir.y()), psi = std::atan2(dir.x(), dir.z());
        u = psi / (2 * pi) + 0.5;
        v = 1 - phi / pi;
        V = Vector3f(-cos(phi)*sin(psi), sin(phi), -cos(phi)*cos(psi));
        U = Vector3f::cross(V, dir);
    }
};


#endif
