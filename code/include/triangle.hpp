#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(b-a, c-a).normalized();
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		// solve (ray.d, v0-v1, v0-v2) * (t, beta, gamma) = v0-ray.o
		float dotdn = Vector3f::dot(ray.getDirection(), normal);
		if(dotdn < 1e-6 && -1e-6 < dotdn) return false;

		Matrix3f mat(ray.getDirection(), vertices[0]-vertices[1], vertices[0]-vertices[2]);
		Matrix3f matinv = mat.inverse();
		Vector3f result = matinv * (vertices[0] - ray.getOrigin());
		if(result[1] < 0 || result[2] < 0 || result[1] + result[2] > 1) return false;
		if(result[0] < tmin || result[0] > hit.getT()) return false;
		hit.set(result[0], material, (dotdn < 0 ? normal : -normal));
        return true;
	}
	Vector3f normal;
	Vector3f vertices[3];
protected:

};

#endif //TRIANGLE_H
