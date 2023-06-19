#include "mesh.hpp"
#include "bvhtree.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>


void BVHTree::build_recursive(std::vector<TriangleInfo> t, BVHNode *p) {

    p->mx = Vector3f(-1e20); p->mi = -p->mx;
    for(int i=0; i<(int)t.size(); i++) {
        p->mi = min_v3f(p->mi, t[i].mi);
        p->mx = min_v3f(p->mx, t[i].mx);
    }

    if(t.size() < BVHLEAF_MAXIMUM_TRIANGLE) { // one triangle left for the node, build it
        p->l = nullptr; p->r = nullptr;
        p->ts.clear();
        for (int i=0; i<(int)t.size(); i++) p->ts.push_back(t[i].t);
        return;
    }

    bool (*comp)(const TriangleInfo &t1, const TriangleInfo &t2);
    Vector3f boxsize = p->mx - p->mi;
    if(boxsize.x() > boxsize.y() && boxsize.x() > boxsize.z()) { 
        comp = [](const TriangleInfo &t1, const TriangleInfo &t2){ return t1.center[0] < t2.center[0]; }; // partition along x
    } else if(boxsize.y() > boxsize.z()) { 
        comp = [](const TriangleInfo &t1, const TriangleInfo &t2){ return t1.center[1] < t2.center[1]; }; // partition along y
    } else { 
        comp = [](const TriangleInfo &t1, const TriangleInfo &t2){ return t1.center[2] < t2.center[2]; }; // partition along z
    }
    
    std::nth_element(t.begin(), t.begin() + t.size()/2, t.end(), comp);

    p->l = new BVHNode();
    p->r = new BVHNode();

    std::vector<TriangleInfo> nxt;
    nxt.clear();
    for(int i=0; i < (int)t.size()/2; i++) nxt.push_back(t[i]);
    build_recursive(nxt, p->l);
    nxt.clear();
    for(int i=(int)t.size()/2; i < (int)t.size(); i++) nxt.push_back(t[i]);
    build_recursive(nxt, p->r);

    // printf("current: mi %s; mx %s.\n    Left mi %s; mx%s.\n    Right mi %s; mx %s\n",
    //     debug_3f(p->mi).c_str(), debug_3f(p->mx).c_str(), 
    //     debug_3f(p->l->mi).c_str(), debug_3f(p->l->mx).c_str(), 
    //     debug_3f(p->r->mi).c_str(), debug_3f(p->r->mx).c_str());
}

TriangleInfo::TriangleInfo(Triangle &tri) {
    t = &tri;
    center = (tri.vertices[0] + tri.vertices[1] + tri.vertices[2]) / 3;
    mi = min_v3f(tri.vertices[0], min_v3f(tri.vertices[1], tri.vertices[2]));
    mx = max_v3f(tri.vertices[0], max_v3f(tri.vertices[1], tri.vertices[2]));
}

void BVHTree::build(Mesh &mesh) {
    rt = new BVHNode();
    std::vector<TriangleInfo> triinfo; // used only in build process
    for (int i=0; i<mesh.t.size(); i++) { // preprocess for triangles
        tri.push_back(Triangle(mesh.v[mesh.t[i][0]], mesh.v[mesh.t[i][1]], mesh.v[mesh.t[i][2]], mesh.material));
    }
    for (int i=0; i<tri.size(); i++) {
        triinfo.push_back(TriangleInfo(tri[i]));
    }
    build_recursive(triinfo, rt);
    return;
}

// bool inside_pt_box(const Vector3f &pt, const Vector3f &bmi, const Vector3f &bmx) {
//     return bmi[0] <= pt[0] && pt[0] <= bmx[0]
//         && bmi[1] <= pt[1] && pt[1] <= bmx[1]
//         && bmi[2] <= pt[2] && pt[2] <= bmx[2];
// }
// float intersect_ray_box(const Ray &r, const Vector3f &bmi, const Vector3f &bmx, float tmin, float tmax) { //TODO:optimize this part
//     if(inside_pt_box(r.pointAtParameter(tmin), bmi, bmx)) return tmin;

//     float tenter = 1e20, t;
//     // x coodinate:
//     if(O(r.getDirection()[0])) {
//         t = (bmi[0] + eps - r.getOrigin()[0]) / r.getDirection()[0];
//         if (t >= tmin && t <= tmax && inside_pt_box(r.pointAtParameter(t), bmi, bmx)) 
//             tenter = min(tenter, t);
//         t = (bmx[0] - eps - r.getOrigin()[0]) / r.getDirection()[0];
//         if (t >= tmin && t <= tmax && inside_pt_box(r.pointAtParameter(t), bmi, bmx)) 
//             tenter = min(tenter, t);
//     }
//     if(O(r.getDirection()[1])) {
//         t = (bmi[1] + eps - r.getOrigin()[1]) / r.getDirection()[1];
//         if (t >= tmin && t <= tmax && inside_pt_box(r.pointAtParameter(t), bmi, bmx)) 
//             tenter = min(tenter, t);
//         t = (bmx[1] - eps - r.getOrigin()[1]) / r.getDirection()[1];
//         if (t >= tmin && t <= tmax && inside_pt_box(r.pointAtParameter(t), bmi, bmx)) 
//             tenter = min(tenter, t);
//     }
//     if(O(r.getDirection()[2])) {
//         t = (bmi[2] + eps - r.getOrigin()[2]) / r.getDirection()[2];
//         if (t >= tmin && t <= tmax && inside_pt_box(r.pointAtParameter(t), bmi, bmx)) 
//             tenter = min(tenter, t);
//         t = (bmx[2] - eps - r.getOrigin()[2]) / r.getDirection()[2];
//         if (t >= tmin && t <= tmax && inside_pt_box(r.pointAtParameter(t), bmi, bmx)) 
//             tenter = min(tenter, t);
//     }
//     return tenter;
// }
float intersect_ray_box(const Ray &r, const Vector3f &bmi, const Vector3f &bmx, float tmin, float tmax) {
    float tstart=tmin, tend=tmax, t1, t2;
    const Vector3f &ori = r.getOrigin(), &dir = r.getDirection();
    for(int i=0; i<3; i++) {
        if(O(ori[i])) {
            t1 = (bmi[i] - ori[i]) / dir[i]; t2 = (bmx[i] - ori[i]) / dir[i];
            tstart = max(min(t1, t2), tstart); tend = min(max(t1, t2), tend);
            if(O(tstart - tend) > 0) return 1e20;
        } else {
            if(bmi[i] > ori[i] || ori[i] < bmx[i]) return 1e20;
        }
    }
    return tstart;
}
bool BVHTree::intersect_recursive(const Ray &r, Hit &h, float tmin, BVHNode *p) {
    bool result=false;

    if (!p->ts.empty()) { // at leaf
        for(int i=0; i<(int)p->ts.size(); i++) {
            result |= p->ts[i]->intersect(r, h, tmin);
        }
        return result;
    }

    float lc_tenter = intersect_ray_box(r, p->l->mi, p->l->mx, tmin, h.getT());
    float rc_tenter = intersect_ray_box(r, p->r->mi, p->r->mx, tmin, h.getT());
    if(lc_tenter < rc_tenter) {
        if(lc_tenter != 1e20)
            result |= intersect_recursive(r, h, tmin, p->l);
        if(rc_tenter != 1e20)
            result |= intersect_recursive(r, h, tmin, p->r);
    } else {
        if(rc_tenter != 1e20)
            result |= intersect_recursive(r, h, tmin, p->r);
        if(lc_tenter != 1e20)
            result |= intersect_recursive(r, h, tmin, p->l);   
    }
    
    
    return result;
}

bool BVHTree::intersect(const Ray &r, Hit &h, float tmin) {
    float tenter = intersect_ray_box(r, rt->mi, rt->mx, tmin, h.getT());
    if (tenter == 1e20) return false;
    return intersect_recursive(r, h, tmin, rt);
}