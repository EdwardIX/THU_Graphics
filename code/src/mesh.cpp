#define TINYOBJLOADER_IMPLEMENTATION
#include "mesh.hpp"
#include "bvhtree.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <vector>
#include "tiny_obj_loader.h"

int read(char *&p) {
    int x=0;char c=*(p++);
    for(;c < '0' && c > '9'; c=*(p++));
    for(;c >= '0' && c <= '9'; c=*(p++)) x = x * 10 + (c ^ 48);
    return x;
}
Vector3f getVector3f(std::vector<float> &d, int x) {x *= 3; return Vector3f(d[x], d[x+1], d[x+2]);}
Vector2f getVector2f(std::vector<float> &d, int x) {x *= 2; return Vector2f(d[x], d[x+1]);}
Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, NULL, true);
    if(!err.empty()) {printf("\033[33m%s\033[0m\n",err.c_str());}
    // if(!warn.empty()) {printf("\033[33m%s\033[0m\n",warn.c_str());}
    if(!success) {printf("Cannot Open %s\n", filename); return;}

    if(shapes.size() != 1) raise("Multiple Shapes Enconter in mesh\n");

    for(int i=0; i < (int)shapes[0].mesh.indices.size(); i+=3) {
        tinyobj::index_t a = shapes[0].mesh.indices[i+0];
        tinyobj::index_t b = shapes[0].mesh.indices[i+1];
        tinyobj::index_t c = shapes[0].mesh.indices[i+2];

        tri.push_back(Triangle(
            getVector3f(attrib.vertices, a.vertex_index),
            getVector3f(attrib.vertices, b.vertex_index),
            getVector3f(attrib.vertices, c.vertex_index),
            material
        ));

        if(a.texcoord_index != -1 && b.texcoord_index != -1 && c.texcoord_index != -1)
            tri.back().setuv(
                getVector2f(attrib.texcoords, a.texcoord_index),
                getVector2f(attrib.texcoords, b.texcoord_index),
                getVector2f(attrib.texcoords, c.texcoord_index)
            );
        
        if(a.normal_index != -1 && b.normal_index != -1 && c.normal_index != -1)
            tri.back().setn(
                getVector3f(attrib.normals, a.normal_index),
                getVector3f(attrib.normals, b.normal_index),
                getVector3f(attrib.normals, c.normal_index)
            );
    }

    bvh.build(tri);
}

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) {
    return bvh.intersect(r, h, tmin);
}