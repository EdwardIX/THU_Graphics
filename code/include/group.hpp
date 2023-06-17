#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {
    }

    explicit Group (int num_objects): V(num_objects) {
    }

    ~Group() override {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool inter = false;
        for(size_t i=0; i<V.size(); i++) {
            if(V[i]->intersect(r, h, tmin)) inter = true;
        }
        return inter;
    }

    void addObject(int index, Object3D *obj) {
        V[index] = obj;
    }

    int getGroupSize() {
        return V.size();
    }

private:
    std::vector<Object3D *> V;
};

#endif
	
