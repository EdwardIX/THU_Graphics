#include "hit.hpp"
#include "material.hpp"
#include "utils.hpp"

const Vector3f Hit::getDiffuseColor() const { return material->getDiffuseColor(u, v);}
const Vector3f Hit::getSpecularColor() const { return material->getSpecularColor();}
const Vector3f Hit::getRefractionColor() const {
    return material->getRefractionColor();
    // Vector3f color = material->getRefractionColor();
    // return Vector3f(exp(color[0] * -t), exp(color[1] * -t), exp(color[2] * -t));
}

Ray Hit::reflection(Ray &in) {
    float dot = Vector3f::dot(in.getDirection(), normal);
    return Ray(position, in.getDirection() - (2*dot) * normal);
}

Ray Hit::refraction(Ray &in) {
    const Vector3f &din = in.getDirection();
    float cosi = - Vector3f::dot(din, normal);
    float k = cosi > 0 ? material->refraction_rate : 1.0/material->refraction_rate; // ray(in) coming from outside/inside
    float cosr2 = 1 - (1-cosi*cosi) / (k*k); // cos2 of refraction ray
    if (O(cosr2) > 0) { // refraction
        if(cosi > 0)
            return Ray(position, din / k + normal * (cosi / k - sqrt(cosr2)));
        // note: the formula are as follow:
        // assert normal(0, 1); din(sin i, -cos i); dout(sin r, -cos r);
        // sin r = sin i / k, -cos r = -cos i / k + cos i / k - cos r
        // thus, (sin r, -cos r) = (sin i, -cos i) / k + normal * (cos i / k - cos r)
        else 
            return Ray(position, din / k + normal * (cosi / k + sqrt(cosr2)));
        // But, if ray coming from inside, normal(0, -1); cosi = -cosi, then the result is like above.
    } else { // total reflection (cosr < 0)
        return reflection(in); 
    }
}

Ray Hit::diffusion(Ray &in) { // generate according to BRDF (TODO: only Lambert way, no specular)
    // float phi = rand_range(0, pi/2);
    // Vector3f dir = Vector3f::cross(normal, rand_sphere());
    // while(dir.squaredLength() < eps) dir = Vector3f::cross(normal, rand_sphere());
    // dir.normalize();
    // return Ray(position, cos(phi) * normal + sin(phi) * dir);
    Vector3f dir = rand_sphere(), norm = getIn(in) ? normal : -normal;
    float dot = Vector3f::dot(dir, norm);
    if(dot < 0) dir += 2*dot * norm;
    return Ray(position, dir);
}

Vector3f Hit::Shade(const Vector3f &dirToCamera, const Vector3f &dirToLight, const Vector3f &lightColor) {
    float coef1 = Vector3f::dot(dirToLight, normal);
    Vector3f reflect = normal * (2*coef1) - dirToLight;
    float coef2 = Vector3f::dot(dirToCamera, reflect);
    Vector3f shed = getDiffuseColor() * std::max(coef1, 0.0f) + getSpecularColor() * pow(std::max(coef2, 0.0f), material->shininess);
    return lightColor * shed;
}

void Hit::disturbNormal(const Vector3f &U, const Vector3f &V) {
    Vector2f grad = material->getGrad(u, v);
    normal = normal + grad[0] * U + grad[1] * V;
    normal.normalize();
}