#include "kdtree.hpp"
#include "ppm.hpp"
#include "utils.hpp"
#include <vector>
#include <algorithm>

KDTreeNode *KDTree::build_recursive(int beg, int end, int split) {
    if(beg == end) return nullptr; // empty
    KDTreeNode *p = new KDTreeNode(beg, end);
    if(beg + 1 == end) { // at leaf
        p->l = nullptr; p->r = nullptr;
        p->mi = p->mx = data[beg].position;
        return p;
    }

    int mid = (beg + end) / 2;
    nth_element(data.begin()+beg, data.begin()+mid, data.begin()+end, 
        [&](const Photon&d1, const Photon&d2){return d1.position[split] < d2.position[split];});
    
    p->l = build_recursive(beg,   mid, (split+1)%3);
    p->r = build_recursive(mid+1, end, (split+1)%3);
    
    // update info(mi, mx)
    p->mi = p->mx = data[mid].position;
    if(p->l) {
        p->mi = min_v3f(p->mi, p->l->mi);
        p->mx = max_v3f(p->mx, p->l->mx);
    }
    if(p->r) {
        p->mi = min_v3f(p->mi, p->r->mi);
        p->mx = max_v3f(p->mx, p->r->mx);
    }
    return p;
}

void KDTree::build() {
    rt = build_recursive(0, data.size(), 0);
}

void KDTree::search(std::vector<Photon*> &Result, Vector3f position, float radius) {
    Result.clear();
    search_recursive(Result, rt, 0, position, radius);
}

// Test: if circ & box has intersection (return 1), or box is included in circ (return 2)
int intersect_circ_box(Vector3f mi, Vector3f mx, const Vector3f &pos, const float r) {
    Vector3f dmi, dmx;
    for(int i=0; i<3; i++) {
        if(pos[i] < mi[i])      dmi[i] = mi[i] - pos[i], dmx[i] = mx[i] - pos[i];
        else if(pos[i] > mx[i]) dmi[i] = pos[i] - mx[i], dmx[i] = pos[i] - mi[i];
        else                    dmi[i] = 0,              dmx[i] = std::max(pos[i] - mi[i], mx[i] - pos[i]);
    }
    if(dmx.squaredLength() <= r*r) return 2;
    if(dmi.squaredLength() <= r*r) return 1;
    return 0;
}
void KDTree::search_recursive(std::vector<Photon*> &Result, KDTreeNode *p, int split, const Vector3f &pos, const float r) {
    if(!p) return;
    int result = intersect_circ_box(p->mi, p->mx, pos, r);
    if(result == 0) return;
    if(result == 2) {
        for(int i=p->beg; i<p->end; i++) 
            Result.push_back(&data[i]);
        return;
    }

    int mid = (p->beg + p->end) / 2;
    if((data[mid].position - pos).squaredLength() <= r*r) 
        Result.push_back(&data[mid]);
    if(pos[split] < data[mid].position[split]) { // intersect from left
        search_recursive(Result, p->l, (split+1)%3, pos, r);
        if(O(pos[split] + r - data[mid].position[split]) >= 0) // also (possibly) intersect right area
            search_recursive(Result, p->r, (split+1)%3, pos, r);
    } else { // intersect from right
        search_recursive(Result, p->r, (split+1)%3, pos, r);
        if(O(pos[split] - r - data[mid].position[split]) <= 0) // also (possibly) intersect left area
            search_recursive(Result, p->l, (split+1)%3, pos, r);
    }
}