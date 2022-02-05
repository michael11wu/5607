
#include <fstream>
#include <string.h>
#include <string>
#include "type.h"
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

//Parsing scene and returns a scene object
Scene parse(string filename) {

    Scene scene;

    ifstream inputstream(filename, std::ios::in | std::ios::binary);

    if(!inputstream.is_open()) {
        fprintf(stderr, "Couldn't open");
        exit(-1);
    }

    string keyword;
    int index = -1;

    while(getline(inputstream, keyword, ' ')) {

        //If keyword starts with a \n, cut it off
        if (keyword.c_str()[0] == '\n') {
            keyword = keyword.erase(0,1);
        }


        if (keyword == "sphere") {
            vec3 c;
            float r;

            inputstream >> c.x >> c.y >> c.z >> r;

            Sphere sphere = {
                .center = c,
                .radius = r,
                .index = index,
            };

            scene.spheres.push_back(sphere);
            printf("Sphere (Position: %f %f %f) Radius: %f Index: %d)\n",
                sphere.center.x, sphere.center.y, sphere.center.z, sphere.radius, sphere.index);

        }

        else if (keyword == "mtlcolor") {
            index++;
            vec3 c;
            inputstream >> c.x >> c.y >> c.z;
            scene.materials.push_back(c);
            printf("mtcolor: %f %f %f\n", c.x, c.y, c.z);

        }

        else if (keyword == "bkgcolor") {
            vec3 c;
            inputstream >> c.x >> c.y >> c.z;
            scene.bkgcolor = c;
            printf("bkgcolor: %f %f %f\n", c.x, c.y, c.z);

        }

        else if (keyword == "eye") {
            vec3 c;
            inputstream >> c.x >> c.y >> c.z;
            scene.eye = c;
            printf("eye: %f %f %f\n", c.x, c.y, c.z);

        }

        else if (keyword == "viewdir") {
            vec3 c;
            inputstream >> c.x >> c.y >> c.z;
            scene.viewdir= c;
            printf("viewdir: %f %f %f\n", c.x, c.y, c.z);

        }

        else if (keyword == "updir") {
            vec3 c;
            inputstream >> c.x >> c.y >> c.z;
            scene.updir = c;
            printf("updir: %f %f %f\n", c.x, c.y, c.z);

        }

        else if (keyword == "imsize") {
            int w,h;
            inputstream >> w >> h;
            scene.width = w;
            scene.height = h;
            printf("width/height: %d %d\n", w, h);

        }

        else if (keyword == "vfov") {
            float fov;
            inputstream >> fov;
            scene.vfov = fov;
            printf("Fov: %f\n", fov);

        }
        else {
            fprintf(stderr,"%s ",keyword.c_str());
            fprintf(stderr,"Word not recognized\n");
            exit(-1);
        }
        std::getline(inputstream, keyword, '\n');
    }
    return scene;
}


//Functions to compute linear algebra ops
vec3 cross(vec3 a, vec3 b) {
    vec3 u{
        (a.y*b.z) - (a.z*b.y),
        (a.z*b.x) - (a.x*b.z),
        (a.x*b.y) - (a.y*b.x)};
    return u;
}

vec3 unit(vec3 a) {
    float length = sqrt(pow(a.x,2.0) + pow(a.y,2.0) + pow(a.z,2.0));
    vec3 unit = {
        a.x / length, 
        a.y / length, 
        a.z / length,
    };
    return unit;

}

vec3 operator*(vec3 a, float scale) {
    vec3 scaled_vect {
        a.x * scale,
        a.y * scale,
        a.z * scale,
    };

    return scaled_vect;
}

vec3 operator/(vec3 a, float scale) {
    vec3 scaled_vect {
        a.x / scale,
        a.y / scale,
        a.z / scale,
    };

    return scaled_vect;
}

vec3 operator+(vec3 a, vec3 b) {
    vec3 vect {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };

    return vect;
}

vec3 operator-(vec3 a, vec3 b) {
    vec3 vect {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };

    return vect;
}

ostream& operator<<(ostream& os, vec3 vect) {
    return cout << vect.x << " " << vect.y << " " << vect.z;
}


vec3 shade_ray(int index, Scene scene) {
    return scene.materials[index];
}


vec3 trace_ray(Ray ray, Scene scene) {

    bool intersect = false;

    Sphere closest_sphere {
        .center = vec3 {0,0,0},
        .radius = 0,
        .index = 0,
    };

    float closest_t = 99;

    //FInd closest object by finding the sphere with the closest t with the ray
    for (Sphere sphere: scene.spheres) {
        float b = 2 * ( (ray.dir.x * (ray.origin.x - sphere.center.x)) + (ray.dir.y * (ray.origin.y - sphere.center.y)) + (ray.dir.z * (ray.origin.z - sphere.center.z)));
        float c = pow(ray.origin.x - sphere.center.x,2) + pow(ray.origin.y - sphere.center.y,2) + pow(ray.origin.z - sphere.center.z,2) - pow(sphere.radius,2);
        float discriminant = (pow(b,2) - 4 * c);

        if (discriminant > 0) {
            float t1 = (-b + sqrt(discriminant)) / 2.0;
            float t2 = (-b - sqrt(discriminant)) / 2.0;

            if (t1 < 0 && t2 > 0) {       //t1 is under 0
                if (t2 < closest_t) {
                    closest_sphere = sphere;
                    closest_t = t2;
                    intersect = true;
                }
            }
            else if (t2 < 0 && t1 > 0) {  //t2 is under 0
                if (t1 < closest_t) {
                    closest_sphere = sphere;
                    closest_t = t1;
                    intersect = true;
                }
            }

            else if (t2 > 0 && t1 > 0) { //both ts are positive
                float t = min(t1,t2);
                if (t < closest_t) {
                    closest_sphere = sphere;
                    closest_t = t;
                    intersect = true;
                }
            }
        }
        else if (discriminant == 0) { //one intersection point
             float t = -b / 2.0;
             if (t > 0 && t < closest_t) {
                 closest_t = t;
                 closest_sphere = sphere;
                 intersect = true;
             }
        }
    }
    if (!intersect) {
        //printf("No INTERSECTION\n");
        return scene.bkgcolor;
    }
    else {
        //printf("T: %f\n", closest_t);
        return shade_ray(closest_sphere.index, scene);
    }
}


int main(int argc, char* argv[]) {
    Scene scene;


    float viewdist = 5.0;
    if (argc == 2) {
        scene = parse(argv[1]);
    }
    else {
        printf("Incorrect amount of arguments");
        return 1;
    }

    //initialize array to store color values
    vec3 **imageArray = new vec3 *[scene.height];
    for (uint32_t i = 0; i < scene.height; i++) {
        imageArray[i] = new vec3[scene.width];
    }


    //Define the coordinate axis
    vec3 u = cross(scene.viewdir, scene.updir);
    u = unit(u);

    vec3 v = cross(u,scene.viewdir);
    v = unit(v);


    vec3 w = scene.viewdir*(-1);
    //cout << u << endl;
    //cout << v << endl;
    // cout << w << endl;

    //convert to radians

    float fov_r = (scene.vfov/2.0) * (3.14/180.0);
    //cout<< fov_r << endl;
    float h_view = 2 * viewdist * tanf(fov_r);
    //cout << h_view << endl;

    //cout<< h_view << endl;

    float aspect_ratio = float(scene.width)/float(scene.height);
    float w_view = h_view * aspect_ratio;

    //calculate scene grid
    vec3 ul = scene.eye + (unit(scene.viewdir)*viewdist) - (u * (w_view/2.0)) + (v * (h_view/2.0));
    vec3 ur = scene.eye + (unit(scene.viewdir)*viewdist) + (u * (w_view/2.0)) + (v * (h_view/2.0));
    vec3 ll = scene.eye + (unit(scene.viewdir)*viewdist) - (u * (w_view/2.0)) - (v * (h_view/2.0));
    vec3 lr = scene.eye + (unit(scene.viewdir)*viewdist) + (u * (w_view/2.0)) - (v * (h_view/2.0));

    // cout << ul << endl;
    // cout << ur << endl;
    // cout << ll << endl;
    // cout << lr << endl;

    vec3 h_offset;
    vec3 v_offset;
    vec3 ch_offset;
    vec3 cv_offset;

    h_offset = (ur-ul) / (scene.width);
    v_offset = (ll-ul) / (scene.height);
    ch_offset = (ur-ul) / (2 * scene.width);
    cv_offset = (ll-ul) / (2 * scene.height);

    // cout << h_offset << endl;
    // cout << v_offset << endl;
    // cout << ch_offset << endl;
    // cout << cv_offset << endl; 


    //shoot rays into the viewing window
    for (int j = 0; j < scene.height; j++) {
        for (int i = 0; i < scene.width; i++) {
            vec3 point = ul + (h_offset * i) + (v_offset * j) + ch_offset + cv_offset;
            Ray ray;
            ray.origin = scene.eye;
            vec3 dir = point - ray.origin;
            dir = unit(dir);
            ray.dir = dir;

            //call to check it if intersects
            vec3 color = trace_ray(ray, scene);
            imageArray[j][i]=color;
        }
    }


     //Write to ppm

    string outputfile = "raytracer.ppm";
    ofstream output_stream(outputfile, ios::out | ios::binary);

    output_stream << "P3\n"
    << scene.width << " "
    << scene.height << "\n"
    << 255 << "\n";

    for (int j = 0; j < scene.height; j++) {
        for (int i = 0; i < scene.width; i++) {
            output_stream << imageArray[j][i].x*255 << " " << imageArray[j][i].y*255 << " " << imageArray[j][i].z*255 << " ";
        }
    }

    //Delete
    delete[] imageArray;


}

//testing intersection
    
    /*Ray raytest;
    raytest.origin = vec3{
        .x = 0,
        .y = 0,
        .z = 0,
    };
    raytest.dir = vec3{
        .x = 0,
        .y = 0.5,
        .z = -1,
    };

    raytest.dir = unit(raytest.dir);

    Sphere spheretest;
    spheretest.center = vec3 {
        .x = 0,
        .y = 0,
        .z = -10,
    };
    spheretest.radius = 8;

    printf("XOrigin: %f\n", raytest.origin.x);
    printf("Center: %f\n", (raytest.dir.z * (raytest.origin.z - spheretest.center.z)));

    float b = 2 * ( (raytest.dir.x * (raytest.origin.x - spheretest.center.x)) + (raytest.dir.y * (raytest.origin.y - spheretest.center.y)) + (raytest.dir.z * (raytest.origin.z - spheretest.center.z)));
    float c = pow(raytest.origin.x - spheretest.center.x,2) + pow(raytest.origin.y - spheretest.center.y,2) + pow(raytest.origin.z - spheretest.center.z,2) - pow(spheretest.radius,2);
    float discriminant = (pow(b,2) - 4 * c);

    printf("B: %f\n", b);
    printf("C: %f\n", c);

    if (discriminant > 0) {
        float t1 = (-b + sqrt(discriminant)) / 2.0;
        float t2 = (-b - sqrt(discriminant)) / 2.0;
        printf("%f\n",t1);
        printf("%f\n",t2);
    }
    */