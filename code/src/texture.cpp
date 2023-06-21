#define STB_IMAGE_IMPLEMENTATION
#include "texture.hpp"
#include "stb_image.h"

Texture::Texture(const char *filename) {
    pic = stbi_load(filename, &width, &height, &channel, 0);
}

Vector3f Texture::get_color(float u, float v) { // if u,v exceed [0, 1), move them into [0, 1)
    // pose u & v to picture 
    u = (u - std::floor(u)) * (width - 1); 
    v = (1-v - std::floor(1-v)) * (height - 1); // v=0 for highest point
    int idx = std::floor(u), idy = std::floor(v);
    return interp_4(
        get_pixel(idx, idy),
        get_pixel(idx+1, idy),
        get_pixel(idx, idy+1),
        get_pixel(idx+1, idy+1),
        u - idx,
        v - idy
    );
}

Vector2f Texture::get_grad(float u, float v) {
    u = (u - std::floor(u)) * (width - 1); 
    v = (1-v - std::floor(1-v)) * (height - 1); // v=0 for highest point
    int idx = std::floor(u), idy = std::floor(v);
    float g0 = get_grayscale(idx, idy), g01 = get_grayscale(idx+1, idy), g10 = get_grayscale(idx, idy+1);
    return Vector2f((g01 - g0), (g10 - g0));
}

Vector3f Texture::get_pixel(int x, int y) { // assert x in [0, width), y in [0, height)
    int id = (x + y * width) * channel;
    return Vector3f(pic[id + 0], pic[id + 1], pic[id + 2]) / 255;
}

float Texture::get_grayscale(int x, int y) { // assert x in [0, width), y in [0, height)
    int id = (x + y * width) * channel;
    return (pic[id + 0] * 0.3 + pic[id + 1] * 0.59 + pic[id + 2] * 0.11) / 255; // formula for grayscale
}