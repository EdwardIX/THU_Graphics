#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"
#include "utils.hpp"

class Material;

class Hit {
public:

    // constructors & destructor
    Hit():material(nullptr), t(1e38) {}
    Hit(float _t, Material *m, const Vector3f &n, const Vector3f &p, float _u, float _v):
        t(_t), material(m), normal(n.normalized()), position(p), u(_u), v(_v) {}
    ~Hit() = default;

    // get & set
    float getT() const { return t;}
    Material *getMaterial() const { return material;}
    const Vector3f &getNormal() const { return normal;}
    const Vector3f &getPosition() const { return position;}
    const float getU() const { return u;}
    const float getV() const { return v;}
    const Vector3f getDiffuseColor() const;
    const Vector3f getSpecularColor() const;
    const Vector3f getRefractionColor() const;
    bool getIn(Ray &in) const { return Vector3f::dot(in.getDirection(), normal) < 0;}
    void set(float _t, Material *m, const Vector3f &n, const Vector3f &p, float _u, float _v) 
        { t = _t; material = m; normal = n.normalized(); position = p; u = _u; v = _v;}

    // Generate next direction
    Ray reflection(Ray &in);
    Ray refraction(Ray &in);
    Ray diffusion(Ray &in);
    Vector3f Shade(const Vector3f &dirToCamera, const Vector3f &dirToLight, const Vector3f &lightColor);

    // bump modification
    void disturbNormal(const Vector3f &U, const Vector3f &V);

private:
    float t; // hit time (for Ray)
    Material *material; // hit material
    Vector3f normal; // normal of hit (must point towards the outside of material)
    Vector3f position; // hit position
    float u,v; // hit position (for texture)
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
