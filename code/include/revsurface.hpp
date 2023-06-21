#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "bvhtree.hpp"
#include "triangle.hpp"
#include "object3d.hpp"
#include "curve.hpp"
#include <tuple>

class RevSurface : public Object3D {

    Curve *pCurve;
    std::vector<Triangle> tri;
    BVHTree bvh;

public:
    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z() != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        typedef std::tuple<unsigned, unsigned, unsigned> Tup3u;

        struct Surface {
            std::vector<Vector3f> VV;
            std::vector<Vector3f> VN;
            std::vector<Tup3u> VF;
        } surface;

        std::vector<CurvePoint> curvePoints;
        pCurve->discretize(30, curvePoints);
        const int steps = 40;
        for (unsigned int ci = 0; ci < curvePoints.size(); ++ci) {
            const CurvePoint &cp = curvePoints[ci];
            for (unsigned int i = 0; i < steps; ++i) {
                float t = (float) i / steps;
                Quat4f rot;
                rot.setAxisAngle(t * 2 * M_PI, Vector3f::UP);
                Vector3f pnew = Matrix3f::rotation(rot) * cp.V;
                Vector3f pNormal = Vector3f::cross(cp.T, -Vector3f::FORWARD);
                Vector3f nnew = Matrix3f::rotation(rot) * pNormal;
                surface.VV.push_back(pnew);
                surface.VN.push_back(nnew);
                int i1 = (i + 1 == steps) ? 0 : i + 1;
                if (ci != curvePoints.size() - 1) {
                    surface.VF.emplace_back((ci + 1) * steps + i, ci * steps + i1, ci * steps + i);
                    surface.VF.emplace_back((ci + 1) * steps + i, (ci + 1) * steps + i1, ci * steps + i1);
                }
            }
        }
        tri.clear();
        for (unsigned i = 0; i < surface.VF.size(); i++) {
            int a = std::get<0>(surface.VF[i]), b = std::get<1>(surface.VF[i]), c = std::get<2>(surface.VF[i]);
            tri.push_back(Triangle(surface.VV[a], surface.VV[b], surface.VV[c], material));
            tri.back().setn(surface.VN[a], surface.VN[b], surface.VN[c]);
        }
        
        bvh.build(tri);
    }

    ~RevSurface() override {
        delete pCurve;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return bvh.intersect(r, h, tmin);
    }
};

#endif //REVSURFACE_HPP
