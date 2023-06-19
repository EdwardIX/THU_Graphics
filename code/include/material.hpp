#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {
        refractionColor = d_color;
        coef_reflect = 0.0;
        coef_refract = 0.0;
        coef_diff = 1;
        refraction_rate = 1.1;
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }
    virtual Vector3f getRefractionColor() const {
        return refractionColor; // TODO: enable the second one ? 
    }
    virtual Vector3f getSpecularColor() const {
        return specularColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        float coef1 = Vector3f::dot(dirToLight, hit.getNormal());
        Vector3f reflect = hit.getNormal() * (2*coef1) - dirToLight;
        float coef2 = Vector3f::dot(-ray.getDirection(), reflect);
        Vector3f shed = diffuseColor * std::max(coef1, 0.0f) + specularColor * pow(std::max(coef2, 0.0f), shininess);
        return lightColor * shed;
    }

    Vector3f diffuseColor;
    Vector3f specularColor;
    Vector3f refractionColor;
    float coef_reflect, coef_refract, coef_diff;
    float shininess;
    float refraction_rate;
};


#endif // MATERIAL_H
