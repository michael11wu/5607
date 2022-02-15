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

struct Material {
    float odr;
    float odg;
    float odb;
    float osr;
    float osg;
    float osb;
    float ka;
    float kd;
    float ks;
    float n;
};

struct Light {
    vec3 position;
    float w;
    vec3 color;
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
    //std::vector<Cylinder> cylinders;
    //std::vector<Objects> objects;
    std::vector<Sphere> spheres;
    std::vector<Material> materials;
    std::vector<Light> lights;
    vec3 bkgcolor;
    vec3 eye;
    vec3 updir;
    vec3 viewdir;
    float vfov;
    int height;
    int width;
};

#endif