#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color, const Vector3f &r_color, 
                      const Vector3f &coef, float s, float refr, 
                    const char *texture_file, const char *bump_file) :
            diffuseColor(d_color), specularColor(s_color), refractionColor(r_color), 
            shininess(s), refraction_rate(refr) {
        Vector3f c = coef.normalized();
        coef_diff = c[0]; coef_reflect = c[1]; coef_refract = c[2];
        texture_color = strlen(texture_file) ? new Texture(texture_file) : nullptr;
        texture_bump = strlen(bump_file) ? new Texture(bump_file) : nullptr;
    }

    virtual ~Material(){
        if(texture_color) delete texture_color;
        if(texture_bump) delete texture_bump;
    }

    virtual Vector3f getDiffuseColor() const { return diffuseColor;}
    virtual Vector3f getDiffuseColor(float u, float v) { return texture_color ? texture_color->get_color(u, v) : diffuseColor;}
    virtual Vector3f getRefractionColor() const { return refractionColor;}
    virtual Vector3f getSpecularColor() const { return specularColor;}
    virtual Vector2f getGrad(float u, float v) { return texture_bump ? texture_bump->get_grad(u, v) : Vector2f(0);}

    Vector3f diffuseColor;
    Vector3f specularColor;
    Vector3f refractionColor;
    float coef_diff, coef_reflect, coef_refract;
    float shininess;
    float refraction_rate;
    Texture *texture_color, *texture_bump; // two different type of texture
};


#endif // MATERIAL_H
