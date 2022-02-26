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

struct Triangle {
    vec3 p0;
    vec3 p1;
    vec3 p2;
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
    vec3 c;
};

struct Depth {
    vec3 color;
    float aMax;
    float aMin;
    float dMax;
    float dMin;
};

struct Face {
    float x;
    float y;
    float z;
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
    //std::vector<Cylinder> cylinders;
    //std::vector<Objects> objects;
    std::vector<vec3> vertices;
    std::vector<Face> faces;
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    std::vector<Light> lights;
    Depth depthCue;
    vec3 bkgcolor;
    vec3 eye;
    vec3 updir;
    vec3 viewdir;
    float vfov;
    int height;
    int width;
    bool depth = false; //flag if depthcueing is being used
    bool softshadow = false;
    //bool atten = false;
};

#endif