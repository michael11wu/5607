#ifndef TYPE_H
#define TYPE_H

#include <vector>

struct vec3 {
    float x;
    float y;
    float z;
};

struct Ray{
    vec3 origin;
    vec3 dir;
};

struct Sphere {
    vec3 center;
    float radius;
    int index;
};

// struct Cylinder {
//     vec3 center;
//     float radius;
//     int index;
// };

// struct Objects {
//     std::vector<Sphere> spheres;
//     std::vector<Cylinder> cylinders;
// }

struct Scene {
    std::vector<Sphere> spheres;
    //std::vector<Cylinder> cylinders;
    //std::vector<Objects> objects;
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