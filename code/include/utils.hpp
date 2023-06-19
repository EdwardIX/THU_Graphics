#ifndef UTILS_H
#define UTILS_H

#include "vecmath.h"
#include <cstring>
#include <string>
#include <cstdio>
#include <random>
#include <cmath>

#define USE_OPENMP
// constants 

// BVH Tree
const int BVHLEAF_MAXIMUM_TRIANGLE = 20;

// PPM Algorithm
const int PPM_ITER = 15;
const int PPM_PHOTON_PER_ITER = 1e6;
const int PPM_PHOTON_NUM = PPM_ITER * PPM_PHOTON_PER_ITER;
const int PPM_TRACERAY_MAXDEPTH = 5;
const int PPM_TRACEPHOTON_MAXDEPTH = 5;
const float PPM_INIT_RADIUS = 0.3;
const float PPM_ALPHA = 0.7;

// floating points 
const float eps = 1e-5;
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

inline void raise(char *msg, bool exit=true) {
    printf("\033[31m%s\033[0m", msg);
    fflush(stdout);
    if(exit) throw 123123;
}

inline std::string debug_3f(Vector3f v){
    static char buf[100];
    sprintf(buf, "(%f, %f, %f)", v[0], v[1], v[2]);
    return std::string(buf);
}

#endif