
#include <fstream>
#include <string.h>
#include <string>
#include "type.h"
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

Scene parse(string filename) {

    Scene scene;

    ifstream inputstream(filename, std::ios::in | std::ios::binary);

    if(!inputstream.is_open()) {
        fprintf(stderr, "Cound't open");
        exit(-1);
    }

    string keyword;

    while(getline(inputstream, keyword, ' ') || getline(inputstream, keyword, '\n')) {

        if (keyword == "sphere") {
            vec3 c;
            float r;

            inputstream >> c.x >> c.y >> c.z >> r;

            Sphere sphere = {
                .center = c,
                .radius = r,
            };

            scene.spheres.push_back(sphere);
            printf("Sphere (Position: %f %f %f) Radius: %f)\n",
                sphere.center.x, sphere.center.y, sphere.center.z, sphere.radius);

        }

        else if (keyword == "mtlcolor") {
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
            scene.bkgcolor = c;
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

        std::getline(inputstream, keyword, '\n');
    }
    return scene;
}

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
        a.x/length, 
        a.y/length, 
        a.z/length};
    return unit;

}

vec3 operator*(vec3 a, float scale) {
    vec3 scaled_vect {
        a.x * scale,
        a.y * scale,
        a.z *scale,
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

    vec3 u = cross(scene.viewdir, scene.updir);
    u = unit(u);

    vec3 v = cross(u,scene.viewdir);
    v = unit(v);


    //vec3 w = scene.viewdir*(-1);

    // cout << u << endl;
    // cout << v << endl;
    // cout << w << endl;
    float h_view = 2 * viewdist * tanh(scene.vfov/2.0f);
    float w_view= h_view * (float(scene.width)/float(scene.height));

    vec3 ul = scene.eye + (unit(scene.viewdir)*viewdist) - (u * (w_view/2.0)) + (v * (h_view/2));
    vec3 ur = scene.eye + (unit(scene.viewdir)*viewdist) + (u * (w_view/2.0)) + (v * (h_view/2));
    vec3 ll = scene.eye + (unit(scene.viewdir)*viewdist) - (u * (w_view/2.0)) - (v * (h_view/2));
    vec3 lr = scene.eye + (unit(scene.viewdir)*viewdist) + (u * (w_view/2.0)) - (v * (h_view/2));

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
    ch_offset = h_offset / 2.0f;
    cv_offset = v_offset / 2.0f;

    cout << h_offset << endl;
    cout << v_offset << endl;
    cout << ch_offset << endl;
    cout << cv_offset << endl;

    string outputfile = "raycast.ppm";
    ofstream output_stream(outputfile, ios::out | ios::binary);


    //header for ppm
    output_stream << "P3\n"
    << scene.width << " "
    << scene.height << "\n"
    << 1 << "\n";

    for (int i = 0; i < scene.height; i++) {
        for (int j = 0; j < scene.width; j++) {
            vec3 point = ul + (h_offset * i) + (v_offset * j) + ch_offset + cv_offset;
            Ray ray;
            ray.origin = scene.eye;
            vec3 dir = point - ray.origin;
            dir = unit(dir);
            ray.dir = dir;

            for (Sphere sphere: scene.spheres) {
                float b = 2 * ( (ray.dir.x * (ray.origin.x - sphere.center.x)) + (ray.dir.y * (ray.origin.y - sphere.center.y)) + (ray.dir.z * (ray.origin.z - sphere.center.z)));
                float c = pow(ray.origin.x - sphere.center.x,2) + pow(ray.origin.y - sphere.center.y,2) + pow(ray.origin.z - sphere.center.z,2) - pow(sphere.radius,2);
                float discriminant = (pow(b,2) - 4 * c);
                if (discriminant > 0) {
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    float t = min(t1,t2);
                    //printf("%f\n", t);
                    output_stream << scene.materials[0].x << " " << scene.materials[0].y << " " << scene.materials[0].z << " ";

                }
                else if (discriminant == 0) {

                }
                else {
                    output_stream << scene.bkgcolor.x << " " << scene.bkgcolor.y << " " << scene.bkgcolor.z << " ";
                }
            }
        }
    }
}




