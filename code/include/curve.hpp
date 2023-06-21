#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <map>
#include <utility>

#include <algorithm>

// TODO (PA2): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override { return false;}
    
    std::vector<Vector3f> &getControls() { return controls;}

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        for(size_t i=0; i<controls.size()-1; i+=3) {
            float dt = 1.0 / resolution;
            for(int j=0; j<resolution || (j==resolution && i == controls.size()-4); j++) {
                float t = dt * j;
                Vector3f V = Vector3f::ZERO, T = Vector3f::ZERO;
                for(int k=0; k<3; k++) {
                    V += controls[i+k] * B(k, 3, t);
                    T += (controls[i+k+1] - controls[i+k]) * B(k, 2, t);
                }
                V += controls[i+3] * B(3, 3, t);
                T *= 3;
                data.push_back((CurvePoint){V, T});
            }
        }
    }

protected:
    float qpow(float x, int t) {
        float ans = 1;
        for(; t; t--) ans *= x;
        return ans;
    }
    int C(int n, int i) { // assert n <= 3
        if(i == n || i == 0) return 1;
        return (n == 2 ? 2 : 3);
    }
    float B(int i, int n, float t) {
        if(i < 0 || i > n) return 0;
        return C(n, i) * qpow(t, i) * qpow(1-t, n-i);
    }
};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
        float dt = 1.0 / (controls.size() + 3);
        for(size_t i = 0; i < controls.size() + 3 + 1; i++)
            knots.push_back(dt * i);
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        for(size_t i=3; i<controls.size(); i++) {
            float dt = (knots[i+1] - knots[i]) / resolution;
            for(int j=0; j<resolution; j++) {
                float t = dt * j + knots[i];
                Vector3f V = Vector3f::ZERO, T = Vector3f::ZERO;
                for(int k=i-3; k<=i; k++) {
                    V += controls[k] * N(k, 3, t);
                    T += controls[k] * dN(k, 3, t);
                }
                data.push_back((CurvePoint){V, T});
            }
        }
    }

protected:
    std::vector<float> knots;
    std::map<std::pair<int, float>, float> cache[4];

    float N(int i, int p, float t) { // assert p <= 3
        if(i < 0 || i > (int)controls.size()) return 0;
        if(p == 0) return (knots[i] <= t && t < knots[i+1] ? 1.0 : 0.0);
        auto key = std::make_pair(i, t);
        if(cache[p].count(key)) return cache[p][key];
        return cache[p][key] = \
            (t - knots[i]) / (knots[i+p] - knots[i]) * N(i, p-1, t) + \
            (knots[i+p+1] - t) / (knots[i+p+1] - knots[i+1]) * N(i+1, p-1, t);
    }

    float dN(int i, int p, float t) { // assert p <= 3
        if(p == 0 || i < 0 || i > (int)controls.size()) return 0;
        return p * (N(i, p-1, t) / (knots[i+p] - knots[i]) - N(i+1, p-1, t) / (knots[i+p+1] - knots[i+1]));
    }
};

#endif // CURVE_HPP
