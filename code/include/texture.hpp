#ifndef TEXTURE_H
#define TEXTURE_H

#include "vecmath.h"
#include "utils.hpp"
#include <cmath>

class Texture {
public:
    Texture(const char *filename);
    Vector3f get_color(float u, float v);
    Vector2f get_grad(float u, float v);

private:
    unsigned char *pic;
    int width, height, channel;

    Vector3f get_pixel(int x, int y);
    float get_grayscale(int x, int y);
};

#endif