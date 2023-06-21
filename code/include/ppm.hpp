#ifndef PPM_H
#define PPM_H

#include <vector>
#include "vecmath.h"
#include "camera.hpp"
#include "group.hpp"
#include "image.hpp"
#include "light.hpp"
#include "kdtree.hpp"
#include "utils.hpp"

struct ViewPoint {
    Hit hit;
    Vector3f direction;
    Vector3f color; // included weight & color to camera
    // for radius & result update
    Vector3f result;
    float radius, photon_num; // photon_num is float because it is an estimate.
    // record corresponding pixel
    int pixel_x, pixel_y;
    // for field depth
    float dist;
    ViewPoint(const Hit &h, Vector3f d, Vector3f c, int x, int y, float dis):
        hit(h), direction(d), color(c), result(0.0), radius(PPM_INIT_RADIUS), photon_num(0), pixel_x(x), pixel_y(y), dist(dis) {} 
};

struct Photon {
    Vector3f position;
    Vector3f direction;
    Vector3f color;
    Photon(Vector3f p, Vector3f d, Vector3f c):
        position(p), direction(d), color(c){}
};

class PPM {
public:
    Camera *camera;
    Group *group; // objects
    Image *image;
    Light *light;
    Vector3f background_color;

    PPM(Camera *c, Group *g, Image *i, Light *l, Vector3f bgc):
        camera(c), group(g), image(i), light(l), background_color(bgc){}

    std::vector<ViewPoint> vps;
    KDTree kdt;

    void run();
    void traceRay(Ray ray, Vector3f color, int depth, int pixel_x, int pixel_y, float dist);
    void tracePhoton(Ray ray, Vector3f color, int depth);
};

#endif