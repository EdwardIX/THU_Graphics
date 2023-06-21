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
        p->mx = max_v3f(p->mx, t[i].mx);
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
    center = (tri[0] + tri[1] + tri[2]) / 3;
    mi = min_v3f(tri[0], min_v3f(tri[1], tri[2]));
    mx = max_v3f(tri[0], max_v3f(tri[1], tri[2]));
}

void BVHTree::build(std::vector<Triangle> &tri) {
    rt = new BVHNode();
    std::vector<TriangleInfo> triinfo; // used only in build process
    for (int i=0; i<tri.size(); i++) 
        triinfo.push_back(TriangleInfo(tri[i]));
    build_recursive(triinfo, rt);
}

float intersect_ray_box(const Ray &r, const Vector3f &bmi, const Vector3f &bmx, float tmin, float tmax) {
    float tstart=tmin, tend=tmax, t1, t2;
    const Vector3f &ori = r.getOrigin(), &dir = r.getDirection();
    for(int i=0; i<3; i++) {
        if(O(dir[i])) {
            t1 = (bmi[i] - ori[i]) / dir[i]; t2 = (bmx[i] - ori[i]) / dir[i];
            tstart = std::max(std::min(t1, t2), tstart); tend = std::min(std::max(t1, t2), tend);
            if(O(tstart - tend) > 0) return tmax + 1;
        } else {
            if(ori[i] < bmi[i] || bmx[i] < ori[i]) return tmax + 1;
        }
    }
    return tstart;
}
bool BVHTree::intersect_recursive(const Ray &r, Hit &h, float tmin, BVHNode *p) {
    bool result=false;

    if (!p->ts.empty()) { // at leaf
        for(size_t i=0; i<p->ts.size(); i++) {
            result |= p->ts[i]->intersect(r, h, tmin);
        }
        return result;
    }

    float lc_tenter = intersect_ray_box(r, p->l->mi, p->l->mx, tmin, h.getT());
    float rc_tenter = intersect_ray_box(r, p->r->mi, p->r->mx, tmin, h.getT());
    if(lc_tenter < rc_tenter) {
        if(lc_tenter < h.getT()) result |= intersect_recursive(r, h, tmin, p->l);
        if(rc_tenter < h.getT()) result |= intersect_recursive(r, h, tmin, p->r);
    } else {
        if(rc_tenter < h.getT()) result |= intersect_recursive(r, h, tmin, p->r);
        if(lc_tenter < h.getT()) result |= intersect_recursive(r, h, tmin, p->l);   
    }
    
    return result;
}

bool BVHTree::intersect(const Ray &r, Hit &h, float tmin) {
    float tenter = intersect_ray_box(r, rt->mi, rt->mx, tmin, h.getT());
    if (tenter >= h.getT()) return false;
    return intersect_recursive(r, h, tmin, rt);
}