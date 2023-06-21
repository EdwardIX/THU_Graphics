#ifndef UTILS_H
#define UTILS_H

#include "vecmath.h"
#include <cstring>
#include <string>
#include <random>
#include <cmath>
#include <iostream>
#include <sstream>

#define USE_OPENMP
// constants 

// BVH Tree
const int BVHLEAF_MAXIMUM_TRIANGLE = 20;

// PPM Algorithm
const int PPM_ITER = 6;
const int PPM_PHOTON_PER_ITER = 2e6;
const int PPM_PHOTON_NUM = PPM_ITER * PPM_PHOTON_PER_ITER;
const int PPM_TRACERAY_MAXDEPTH = 10;
const int PPM_TRACEPHOTON_MAXDEPTH = 10;
const float PPM_INIT_RADIUS = 1;
const float PPM_ALPHA = 0.7;

// floating points 
const float eps = 1e-3;
const float pi = 3.14159265358979324;

inline int O(float x){
    return x > eps ? 1 : (x < -eps ? -1 : 0);
}

inline Vector3f min_v3f(const Vector3f &a, const Vector3f &b) {
    return Vector3f(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]));
}
inline Vector3f max_v3f(const Vector3f &a, const Vector3f &b) {
    return Vector3f(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]));
}
inline bool less_v3f(const Vector3f &a, const Vector3f &b) {
    return a[0]<b[0] && a[1]<b[1] && a[2]<b[2];
}
inline Vector3f abs_v3f(const Vector3f &a) {
    return Vector3f((a[0]<0 ? -a[0]:a[0]), (a[1]<0 ? -a[1]:a[1]), (a[2]<0 ? -a[2]:a[2]));
}
template<class Type>
inline Type interp_3(const Type &a, const Type &b, const Type &c, float bcoef, float ccoef) { // linear interpolation 
    return (1 - bcoef - ccoef) * a + bcoef * b + ccoef * c;
}
template<class Type>
inline Type interp_4(const Type &a00, const Type &a01, const Type &a10, const Type &a11, float coef0, float coef1) { // linear interpolation 
    return (1-coef0) * ( (1-coef1) * a00 + coef1 * a01 ) + coef0 * ( (1-coef1) * a10 + coef1 * a11 );
}

// random algorithm

static std::mt19937 mersenneTwister;
static std::uniform_real_distribution<float> uniform;
inline float _rand() {
    return uniform(mersenneTwister);
}
inline float rand_range(float l, float r) {
    return _rand() * (r-l) + l;
}
inline Vector2f rand_circle() {
    float x, y, r;
    do {
        x = rand_range(-1, 1);
        y = rand_range(-1, 1);
        r = x*x + y*y;
    }while(r >= 1);
    return Vector2f(x, y);
}
inline Vector3f rand_sphere() { // generate uniform distributed vector in S^2
    Vector2f p = rand_circle();
    float r = p.absSquared();
    return Vector3f(2 * p[0] * sqrt(1 - r), 2 * p[1] * sqrt(1 - r), 1 - 2 * r);
}

// debugs

inline void raise(std::string msg, bool exit=true) {
    printf("\033[31m%s\033[0m", msg.c_str());
    fflush(stdout);
    if(exit) throw 123123;
}

inline std::string debug_3f(Vector3f v){
    std::stringstream ss;
    ss << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return ss.str();
}

#endif