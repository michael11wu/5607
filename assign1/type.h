#ifndef TYPE_H
#define TYPE_H

#include <vector>

struct vec3 {
    float x,y,z;
};

struct Ray{
    vec3 origin;
    vec3 dir;
};

struct Sphere {
    vec3 center;
    float radius;
};

struct Scene {
    std::vector<Sphere> spheres;
    std::vector<vec3> materials;
    vec3 bkgcolor;
    vec3 eye;
    vec3 updir;
    vec3 viewdir;
    float vfov;
    int height;
    int width;
};

#endif