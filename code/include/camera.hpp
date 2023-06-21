#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "image.hpp"
#include "utils.hpp"
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setImage(Image *i) {img = i;}
    virtual void AddPixel(int x, int y, const Vector3f &color, float dist) = 0;

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
    // image
    Image *img;
};

// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        fx = (imgW / 2) / tan(angle/2);
        fy = (imgH / 2) / tan(angle/2);
    }

    Ray generateRay(const Vector2f &point) override {
        Vector3f coef = Vector3f(1, (point[0] - width/2) / fx, (point[1] - height/2) / fy).normalized();
        return Ray(center, direction * coef[0] + horizontal * coef[1] + up * coef[2]);
    }
    
    void AddPixel(int x, int y, const Vector3f &color, float dist) override {
        img->AddPixel(x, y, color);
    }

protected: 
    float fx, fy;
};

class LensCamera : public PerspectiveCamera {

public:
    LensCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle, float dist, float r) : 
            PerspectiveCamera(center, direction, up, imgW, imgH, angle), radius(r) {
        // according to convex lens imaging formula, 1/u + 1/v = 1/f
        distv = dist; // assert v = u
        focal = 1 / (1/dist + 1/distv);
    }

    void AddPixel(int x, int y, const Vector3f &color, float dist) override {
        // v = 1 / (1/f - 1/u), r / (distv-v) = radius / v => r = radius * (distv*vinv - 1)
        float r = std::abs(radius * ((1/focal - 1/dist)*distv - 1)); // blur radius
        if(!O(r)) {
            img->AddPixel(x, y, color);
            return;
        }
        int n, i, j, k;
        for(n = 1; gauss(n, 0, r) > eps; n++);
        float *gmatrix = new float[(2*n+1)*(2*n+1)], sum = 0;
        for(i=-n, k=0; i<=n; i++) for(j=-n; j<=n; j++, k++) {
            gmatrix[k] = gauss(i, j, r);
            sum += gmatrix[k];
        }
        for(i=-n, k=0; i<=n; i++) for(j=-n; j<=n; j++, k++) if(img->Legal(x+i, y+j)) {
            img->AddPixel(x+i, y+j, color * gmatrix[k] / sum);
        }
        delete[] gmatrix;
    }

protected: 
    float radius; // radius of aperture
    float focal; // focal length
    float distv; 
    
    float gauss(float x, float y, float r) {
        return exp(- (x*x + y*y) / (8 * r * r));
    }
};

#endif //CAMERA_H
