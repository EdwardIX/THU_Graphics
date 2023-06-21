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
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m), auv(0), buv(0), cuv(0) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(b-a, c-a).normalized();
		use_interpolate_normal = false;
	}

	const Vector3f & operator [] (int i) const { return vertices[i];}
	void setuv(const Vector2f& a, const Vector2f& b, const Vector2f& c) {
		auv = a; buv = b; cuv = c;
	}
	void setn(const Vector3f& a, const Vector3f& b, const Vector3f& c) {
		an = a.normalized(); bn = b.normalized(); cn = c.normalized(); use_interpolate_normal = true;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		// want ray.o + t * ray.d = v0 + u*(v1-v0) + v*(v2-v0)
		// solve (ray.d, v0-v1, v0-v2) * (t, u, v) = v0-ray.o
		if(!O(Vector3f::dot(ray.getDirection(), normal))) return false; // otherwise, matrix is singular

		Matrix3f mat(ray.getDirection(), vertices[0]-vertices[1], vertices[0]-vertices[2]);
		Matrix3f matinv = mat.inverse();
		Vector3f result = matinv * (vertices[0] - ray.getOrigin());
		if(result[1] < 0 || result[2] < 0 || result[1] + result[2] > 1) return false;
		if(result[0] < tmin || result[0] > hit.getT()) return false;

		Vector3f collide_normal = use_interpolate_normal ? interp_3(an, bn, cn, result[1], result[2]) : normal;
		Vector2f uv = interp_3(auv, buv, cuv, result[1], result[2]);
		hit.set(result[0], material, collide_normal, ray.pointAtParameter(result[0]), uv[0], uv[1]);
        return true;
	}
	
protected:
	Vector3f normal;
	Vector3f vertices[3];
	Vector2f auv, buv, cuv;
	Vector3f an, bn, cn; 
	bool use_interpolate_normal;
};

#endif //TRIANGLE_H
