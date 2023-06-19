#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include "vecmath.h"

struct Photon;

struct KDTreeNode {
    KDTreeNode *l, *r;
    int beg, end; // store the range of Datas in its subtree
    Vector3f mi, mx; // the box of the node
    KDTreeNode(int b, int e):beg(b), end(e){}
    ~KDTreeNode(){ if(l) delete l; if(r) delete r;}
};

class KDTree {
public:
    void clear(){ if(rt) delete rt; data.clear(); rt=nullptr;}
    void push(const Photon &d) {data.push_back(d);}
    void build();
    void search(std::vector<Photon*> &Result, Vector3f position, float radius);
    ~KDTree(){ if(rt) delete rt;}
private:
    std::vector<Photon> data;
    KDTreeNode *rt;
    KDTreeNode *build_recursive(int beg, int end, int split);
    void search_recursive(std::vector<Photon*> &Result, KDTreeNode *p, int split, const Vector3f &pos, const float r);
};

#endif