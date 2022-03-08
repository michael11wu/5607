
#include <fstream>
#include <string.h>
#include <string>
#include "type.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <regex>
#include <stdio.h>

using namespace std;



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

float length(vec3 a) {
    float length;
    length = sqrt(pow(a.x,2) + pow(a.y,2) + pow(a.z,2));
    return length;
}

float dot(vec3 a, vec3 b) {
    float ret;
    ret = (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
    return ret;
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

std::vector<int> textureDimension(string filename) {
    FILE* fp = fopen(filename.c_str(),"r");
    if (fp == NULL) {
        fprintf(stderr,"File not found\n");
        exit(1);
    }
    char line[20];
    int height, width;
    std::vector<int> dimen;
    if (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line,"P3 %d %d", &width, &height) ==2) { //Send values of width and height
            dimen.push_back(width);
            dimen.push_back(height);
        }
    }
    return dimen;
}
//Texture Object
vec3 ** texture_parse(string filename) {
    FILE* fp = fopen(filename.c_str(),"r");
    if (fp == NULL) {
        fprintf(stderr,"File not found\n");
        exit(1);
    }
    //int offset;
    int height, width;
    float max_val;
    std::vector<vec3> rgb;
    vec3 **texArray;
    vec3 i;
    if (fscanf(fp,"P3 %d %d %f", &width, &height, &max_val) == 3) {
        //printf("HEADER: P3 %d %d %f\n", width, height, max_val);
        texArray = new vec3 *[height];
            for (uint32_t i = 0; i < height; i++) {
                texArray[i] = new vec3[width];
            }
    }
    while (fscanf(fp,"%f %f %f", &i.x, &i.y, &i.z) == 3) {
        //printf("RGB: %f %f %f\n", i.x, i.y, i.z);
        i.x = i.x/max_val;
        i.y = i.y/max_val;
        i.z = i.z/max_val;
        rgb.push_back(i);
    }

    for (uint32_t j = 0; j < height; j++) {
        for (uint32_t i = 0; i < width; i++) {
            texArray[j][i] = rgb[(width*j)+i];
        }
    }
    fclose(fp);
    return texArray;
}


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
    int textureIndex = -1;
    bool texture = false;

    while(getline(inputstream, keyword, ' ') && !(inputstream.eof())) {

        //If keyword starts with a \n, cut it off
        while (keyword.c_str()[0] == '\n') {
            //printf("%s\n",keyword.c_str());
            keyword = keyword.erase(0,1);
        }


        if (keyword == "sphere") {
            vec3 c;
            float r;

            inputstream >> c.x >> c.y >> c.z >> r;
            Sphere sphere;
            sphere.center = c;
            sphere.radius = r;
            sphere.index = index;
            sphere.textureIndex = textureIndex;
            sphere.texture = texture;

            scene.spheres.push_back(sphere);
            printf("Sphere (Position: %f %f %f) Radius: %f Index: %d TextureIndex : %d)\n",
                sphere.center.x, sphere.center.y, sphere.center.z, sphere.radius, sphere.index, sphere.textureIndex);

        }
        
        else if (keyword == "cylinder") {
            vec3 center;
            vec3 dir;
            float length;
            float radius;
            inputstream >> center.x >> center.y >> center.z >> dir.x >> dir.y >> dir.z >> radius >> length;
            Cylinder cylinder = {
                .center = center,
                .dir = dir,
                .radius = radius,
                .length = length,
                .index = index,
            };
            printf("Cylinder (Position: %f %f %f) (Dir: %f %f %f) Radius: %f Length: %f Index: %d)\n",
                cylinder.center.x, cylinder.center.y, cylinder.center.z, cylinder.dir.x, cylinder.dir.y, cylinder.dir.z, cylinder.radius, cylinder.length, cylinder.index);
            scene.cylinders.push_back(cylinder);
        }

        else if (keyword == "texture") {
            texture = true;
            textureIndex++;
            string filename;
            inputstream >> filename;
            printf("Filename: %s\n", filename.c_str());
            Texture newTexture;
            newTexture.imageArray = texture_parse(filename);
            newTexture.width = textureDimension(filename)[0];
            newTexture.height = textureDimension(filename)[1];
            scene.textureImages.push_back(newTexture);
        }

        else if (keyword == "mtlcolor") {
            index++;
            Material m;
            inputstream >> m.odr >> m.odg >> m.odb >> m.osr >> m.osg >> m.osb >> m.ka >> m.kd >> m.ks >> m.n;
            scene.materials.push_back(m);
            printf("mtcolor: %f %f %f %f %f %f %f %f %f %f\n", m.odr, m.odg, m.odb, m.osr, m.osg, m.osb, m.ka, m.kd, m.ks, m.n);

        }

        else if (keyword == "light") {
            Light l;
            vec3 position;
            vec3 color;
            vec3 c = {
                c.x = -1,
                c.y = -1,
                c.z = -1,
            };
            inputstream >> position.x >> position.y >> position.z >> l.w >> color.x >> color.y >> color.z;
            l.position = position;
            l.color = color;
            l.c = c;
            scene.lights.push_back(l);
            printf("light: %f %f %f %f %f %f %f\n", l.position.x, l.position.y, l.position.z, l.w, l.color.x, l.color.y, l.color.z);

        }

        else if (keyword == "attlight") {
            //scene.atten = true;
            Light l;
            vec3 position;
            vec3 color;
            vec3 c;
            inputstream >> position.x >> position.y >> position.z >> l.w >> color.x >> color.y >> color.z >> c.x >> c.y >> c.z;
            l.position = position;
            l.color = color;
            l.c = c;
            scene.lights.push_back(l);
            printf("attlight: %f %f %f %f %f %f %f %f %f %f\n", l.position.x, l.position.y, l.position.z, l.w, l.color.x, l.color.y, l.color.z, l.c.x, l.c.y, l.c.z);
        }

        else if (keyword == "depthcueing") {
            scene.depth = true;
            Depth depth;
            vec3 c;
            inputstream >> c.x >> c.y >> c.z >> depth.aMax >> depth.aMin >> depth.dMax >> depth.dMin;
            depth.color = c;
            scene.depthCue = depth;
            printf("Depth Cueing: %f %f %f %f %f %f %f\n", depth.color.x, depth.color.y, depth.color.z, depth.aMax, depth.aMin, depth.dMax, depth.dMin);
        }

        else if (keyword == "bkgcolor") {
            vec3 c;
            inputstream >> c.x >> c.y >> c.z;
            scene.bkgcolor = c;
            printf("bkgcolor: %f %f %f\n", c.x, c.y, c.z);

        }

        else if (keyword == "v") {
            vec3 v;
            inputstream >> v.x >> v.y >> v.z;
            scene.vertices.push_back(v);
            printf("Vertices: %f %f %f\n", v.x, v.y, v.z);

        }

        else if (keyword == "vn") {
            vec3 v;
            inputstream >> v.x >> v.y >> v.z;
            scene.norm_vertices.push_back(v);
            printf("Norm Vertices: %f %f %f\n", v.x, v.y, v.z);

        }

        else if (keyword == "vt") {
            vec3 vt;
            inputstream >> vt.x >> vt.y;
            scene.texture_coords.push_back(vt);
            printf("Texture Coords: %f %f \n", vt.x, vt.y);

        }

        else if (keyword == "f") {
            Face f;
            std::streampos p = inputstream.tellg();
            stringstream buffer;
            buffer << inputstream.rdbuf();
            inputstream.seekg(p);
            string s = buffer.str();
            if (s.find('\n') == string::npos) {
                //leave it be
            }
            else {
                s.erase(s.find('\n'), (s.length() - s.find('\n')));
            }
            f.index = index;
            f.textureIndex = textureIndex;

            if (sscanf(s.c_str(),"%f %f %f", &f.v.x, &f.v.y, &f.v.z)==3) { //f 1 2 3
                printf("V STYLE: ");
                printf("Faces: %f %f %f\n", f.v.x, f.v.y, f.v.z);
                f.type = 0;
            } 
            else if (sscanf(s.c_str(), "%f//%f %f//%f %f//%f",&f.v.x, &f.vn.x, &f.v.y, &f.vn.y, &f.v.z, &f.vn.z) == 6) { //f 1//1 1//1 1//1
            //success reading a face in v//n format; proceed accordingly
                printf("v//n style: ");
                printf("Faces: %f %f %f %f %f %f\n", f.v.x, f.vn.x, f.v.y, f.vn.y, f.v.z, f.vn.z);
                f.type = 1;
            } 
            else if (sscanf(s.c_str(), "%f/%f %f/%f %f/%f",&f.v.x, &f.vt.x, &f.v.y, &f.vt.y, &f.v.z, &f.vt.z) == 6) { //f 1/1 1/1 1/1
            //success reading a face in v/t format; proceed accordingly
                printf("v/t style: ");
                printf("Faces: %f %f %f %f %f %f\n", f.v.x, f.vt.x, f.v.y, f.vt.y, f.v.z, f.vt.z);
                f.type = 2;
            } 
            else if (sscanf(s.c_str(), "%f/%f/%f %f/%f/%f %f/%f/%f", &f.v.x, &f.vt.x, &f.vn.x, &f.v.y, &f.vt.y, &f.vn.y, &f.v.z, &f.vt.z, &f.vn.z ) == 9) { //f 1/1/1 1/1/1 1/1/1
                printf("v/t/n style: ");
                printf("Faces: %f %f %f %f %f %f %f %f %f\n", f.v.x, f.vt.x, f.vn.x, f.v.y, f.vt.y, f.vn.y, f.v.z, f.vt.z, f.vn.z);
                f.type = 3;
            //success reading a face in v/t/n format; proceed accordingly
            } 
            scene.faces.push_back(f);
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

        else if (keyword == "softshadow") {
            bool flag;
            inputstream >> flag;
            scene.softshadow = flag;
            printf("Soft shadow: %d\n", flag);
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

float soft_shadow(Ray shadow, Sphere s, Light light, vec3 intersect) {
    //srand(time(0)); //rand seed
    float shadow_flag = 0.0f;
    for (int i = 0; i < 60; i++) {

        //printf("Shadow flag: %f\n", shadow_flag);

        Light temp = light;
        
        //printf("CHANGE: \n ");

        //cout << light.position << endl;
        //jitter
        temp.position.x = light.position.x + ((float(rand()/float(RAND_MAX)) * 2) - (1));
        temp.position.y = light.position.y + ((float(rand()/float(RAND_MAX)) * 2) - (1));
        temp.position.z = light.position.z + ((float(rand()/float(RAND_MAX)) * 2) - (1));

        //cout << temp.position << endl;
        //cout << light.position << endl;

        float light_distance = length(temp.position - intersect);
        //cout << temp.position << endl;
        Ray soft;
        soft.origin = intersect;
        if (light.w == 0) { //directional light
            soft.dir = temp.position*(-1.0f);
            soft.dir = unit(soft.dir);
        }
        else {      //point light
            soft.dir = (temp.position - intersect);
            soft.dir = unit(soft.dir);
        }
        
        float b = 2 * ( (soft.dir.x * (soft.origin.x - s.center.x)) + (soft.dir.y * (soft.origin.y - s.center.y)) + (soft.dir.z * (soft.origin.z - s.center.z)));
        float c = pow(soft.origin.x - s.center.x,2) + pow(soft.origin.y - s.center.y,2) + pow(soft.origin.z - s.center.z,2) - pow(s.radius,2);
        float discriminant = (pow(b,2) - 4 * c);

        if (light.w == 0) { //directional light
            if (discriminant > 0) { //2 intersections
                float t1 = (-b + sqrt(discriminant)) / 2.0;
                float t2 = (-b - sqrt(discriminant)) / 2.0;
                if (t1 > 0.01f || t2 > 0.01f) {
                    shadow_flag += 0;
                }
                else {
                    shadow_flag += 1;
                }
            }
            else if (discriminant == 0) { //one intersection point
                float t = -b / 2.0;
                if (t > 0.01f) {
                    shadow_flag += 0;
                }
                else {
                    shadow_flag+=1;
                }
            }
        }
        else { //point light
            if (discriminant > 0) { //2 intersections
                float t1 = (-b + sqrt(discriminant)) / 2.0;
                float t2 = (-b - sqrt(discriminant)) / 2.0;
                float dist1 = length(soft.dir * t1);
                float dist2 = length(soft.dir * t2);
                if ((t1 > 0.01f && dist1 < light_distance) || (t2 > 0.01f && dist2 < light_distance)) {
                    shadow_flag += 0;
                }
                else {
                    shadow_flag+=1;
                }

            }
            else if (discriminant == 0) { //one intersection point
                float t = -b / 2.0;
                float dist = length(soft.dir * t);
                if (t > 0.01f && dist < light_distance) {
                    shadow_flag += 0;
                }
                else {
                    shadow_flag+=1;
                }
            }
        }
    }
    shadow_flag = shadow_flag/60.0f;
    //printf("Shadow flag value : %f\n", shadow_flag);
    return shadow_flag;

}

vec3 shade_ray(Scene scene, Sphere sphere, vec3 intersect) {

    vec3 shaded_color;

    float Ir;
    float Ig;
    float Ib;

    Material color;

    if (sphere.index > -1) {
        color = scene.materials[sphere.index];
        //printf("Sphere index: %d\n", sphere.index);
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }


    if (sphere.texture) {   //apply texture to spehre
        float Nx = (intersect.x - sphere.center.x)/ sphere.radius;
        float Ny = (intersect.y - sphere.center.y)/ sphere.radius;
        float Nz = (intersect.z - sphere.center.z)/ sphere.radius;
        
        //flipped Ny and Nz so Y axis has the old Z axis since View dir is down Z

        //make sure Ny is in bounds for acos
        if (Ny <= -1.0000000000000000f) {
            printf("Ny %f\n", Ny);
            Ny += 0.00001f;
            printf("Ny %f\n", Ny);
        }
        if (Ny >= 1.00000000000000000f) {
            Ny -= 0.00001f;
        }


        float φ = acosf(Ny);
        //printf("Phi: %f\n", φ);
        float Θ = atan2f((Nz),(Nx));

        //printf("Phi Theta %f %f\n",φ,Θ);
        //rotate the longitude value 180 degrees
        Θ = Θ * -0.5;

        //Make it range on the value of 1
        float v = φ / pi; //v
        float u = (Θ + pi) / (2.0f * pi); //u
        
        //printf("U V: %f %f\n", u,v);

        float i = round(u * (scene.textureImages[sphere.textureIndex].width - 1));
        float j = round(v * (scene.textureImages[sphere.textureIndex].height - 1));

        //make sure it doesn't go out of bounds
        if (i == scene.textureImages[sphere.textureIndex].width) {
            i -= 1;
        }
        if (j == scene.textureImages[sphere.textureIndex].height) {
            j-=1;
        }
        //printf("Scene texture Widht and height: %f %f\n",scene.textureImages[sphere.textureIndex].width, scene.textureImages[sphere.textureIndex].height );
        //printf("I J: %f %f\n",i,j);
        color.odr = scene.textureImages[sphere.textureIndex].imageArray[int(j)][int(i)].x;
        color.odg = scene.textureImages[sphere.textureIndex].imageArray[int(j)][int(i)].y;
        color.odb = scene.textureImages[sphere.textureIndex].imageArray[int(j)][int(i)].z;
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    

    vec3 lightIntensity; //

    //surface normal vector
    vec3 N = (intersect - sphere.center) / sphere.radius;
    //Vector in direction of viewer
    vec3 V = scene.eye - intersect;
    //vec3 V = (scene.viewdir);
    V = unit(V);
    for (Light light : scene.lights) { //loop through lights

        float atten = 1.0f;

        lightIntensity = light.color;

        //For Calculate shadow
        Ray shadow;
        shadow.origin = intersect;
        float shadow_flag = 1.0f;
        vec3 vlight = light.position;
        float light_distance = length(vlight - intersect);

        
        vec3 L;

        if (light.w == 0) { //directional light
            L = vlight*(-1.0);
            L = unit(L);
            shadow.dir = L;
        }
        else {      //point light
            L = vlight - intersect;
            L = unit(L);
            shadow.dir = L;
            if (light.c.x != -1.0f) { //if -1, not doing light attenuation
                atten = 1.0f / (light.c.x + (light.c.y * light_distance) + (light.c.z * pow(light_distance,2)));
            }
        }

        //N dot L value
        //printf("N DOT L: %f\n", dot(N,L));
        float cosDiffuse = max(0.0f, dot(N,L));

        //Halfway vector
        vec3 H = (L + V);
        H = unit(H);
        
        //Check for shadow intersection
        

        for (Sphere s : scene.spheres) {
            float b = 2 * ( (shadow.dir.x * (shadow.origin.x - s.center.x)) + (shadow.dir.y * (shadow.origin.y - s.center.y)) + (shadow.dir.z * (shadow.origin.z - s.center.z)));
            float c = pow(shadow.origin.x - s.center.x,2) + pow(shadow.origin.y - s.center.y,2) + pow(shadow.origin.z - s.center.z,2) - pow(s.radius,2);
            float discriminant = (pow(b,2) - 4 * c);

            if (light.w == 0) { //directional light
                if (discriminant > 0) { //2 intersections
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    if (t1 > 0.01f || t2 > 0.01f) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        shadow_flag = 1;
                    }
                }
                else if (discriminant == 0) { //one intersection point
                    float t = -b / 2.0;
                    if (t > 0.01f) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        shadow_flag = 1;
                    }
                }
            }
            else { //point light
                if (discriminant > 0) { //2 intersections
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    float dist1 = length(shadow.dir * t1);
                    float dist2 = length(shadow.dir * t2);
                    if ((t1 > 0.01f && dist1 < light_distance) || (t2 > 0.01f && dist2 < light_distance)) {
                        if (scene.softshadow) {
                            //call soft shadow
                            //printf("Sphere coord: %f,%f,%f\n", s.center.x, s.center.y,s.center.z);
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        //printf("Intersect with itself or not in view\n");
                        //shadow_flag += 1;
                    }

                }
                else if (discriminant == 0) { //one intersection point
                    float t = -b / 2.0;
                    float dist = length(shadow.dir * t);
                    if ((t > 0.01f) && (dist < light_distance)) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                            //printf("Shadow_flag val : %f\n", shadow_flag);
                            
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        printf("Intersect with itself or not in view\n");
                        //shadow_flag += 1;
                    }
                }
            }
        }

        for (Triangle t: scene.triangles) { //check if any triangles cause shadow
            //Ax + By + Cz + D = 0;
            vec3 e1 = t.p1 - t.p0;
            vec3 e2 = t.p2 - t.p0;
            vec3 n = cross(e1,e2);
            float d;
            d = ((n.x * t.p0.x) + (n.y * t.p0.y) + (n.z * t.p0.z)) * (-1);
            float triangle_t;
            float numer = (-1) * ((n.x * shadow.origin.x) + (n.y * shadow.origin.y) + (n.z * shadow.origin.z) + d);
            float denom = ((n.x * shadow.dir.x) + (n.y * shadow.dir.y) + (n.z * shadow.dir.z));
            if (denom == 0) {
                //no intersection
                shadow_flag = 1;
            }
            else {
                triangle_t = numer / denom;
                //printf("TRIANGLE_T: %f", triangle_t);
                float dist = length(shadow.dir * triangle_t);
                if (light.w == 1) { //point light
                    if (triangle_t > 0.01f && dist < light_distance) { //THIS IS INTERSECTION
                        //printf("INTERSECT WITH PLANE\n");
                        vec3 p = (shadow.origin + (shadow.dir * triangle_t));
                        vec3 e3 = p - t.p1;
                        vec3 e4 = p - t.p2;
                        float A = (0.5f) * length(cross(e1,e2));
                        float a = (0.5f) * length(cross(e3,e4));
                        float b = (0.5f) * length(cross(e4,e2));
                        float c = (0.5f) * length(cross(e1,e3));
                        float alpha = a/A;
                        float beta = b/A;
                        float gamma = c/A;
                        if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                            if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.998) {
                                shadow_flag = 0;
                            }
                            else {
                                shadow_flag = 1;
                            }
                        }
                    }
                }
                else { //direcitonal light
                    if (triangle_t > 0.01f) { //THIS IS INTERSECTION
                        //printf("INTERSECT WITH PLANE\n");
                        vec3 p = (shadow.origin + (shadow.dir * triangle_t));
                        vec3 e3 = p - t.p1;
                        vec3 e4 = p - t.p2;
                        float A = (0.5f) * length(cross(e1,e2));
                        float a = (0.5f) * length(cross(e3,e4));
                        float b = (0.5f) * length(cross(e4,e2));
                        float c = (0.5f) * length(cross(e1,e3));
                        float alpha = a/A;
                        float beta = b/A;
                        float gamma = c/A;
                        if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                            if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.998) {
                                shadow_flag = 0;
                            }
                            else {
                                shadow_flag = 1;
                            }
                        }
                    }
                }
            }
        }

        shadow_flag = min(1.0f,shadow_flag);

        shadow_flag = max(0.0f, shadow_flag);

        if (scene.softshadow)  {
            if (shadow_flag < 1 && shadow_flag > 0) {
                //cout << dot(N,H) << endl;
                //cout << shadow_flag << endl;
                // printf("=================\n");
                // printf("%f %f %f %f %f %f %f\n", shadow_flag, lightIntensity, atten, (color.kd * color.odr), cosDiffuse,(color.ks * color.osr), pow(max(-0.000000000001f,dot(N, H)),color.n) );
                //printf("R: %f\n",(Ir + (shadow_flag * lightIntensity.x * atten * ( ((color.kd * color.odr) * (cosDiffuse)) + ((color.ks * color.osr) * pow(max(0.0f,dot(N, H)),color.n))))));
                //printf("G: %f\n",(Ig + (shadow_flag * lightIntensity.y * atten * ( ((color.kd * color.odr) * (cosDiffuse)) + ((color.ks * color.osr) * pow(max(0.0f,dot(N, H)),color.n))))));
                //printf("b: %f\n",(Ib + (shadow_flag * lightIntensity.z * atten * ( ((color.kd * color.odr) * (cosDiffuse)) + ((color.ks * color.osr) * pow(max(0.0f,dot(N, H)),color.n))))));
            }
        }
        else {
            //cout << shadow_flag <<endl;
        }
        if (shadow_flag < 1) {
            //cout << "FINAL IG COLOR: " << Ig << endl;
        }

        Ir += (shadow_flag * lightIntensity.x * atten * ( ((color.kd * color.odr) * (cosDiffuse)) + ((color.ks * color.osr) * pow(max(0.0f,dot(N, H)),color.n)))); //red
        Ig += (shadow_flag * lightIntensity.y * atten * ( ((color.kd * color.odg) * (cosDiffuse)) + ((color.ks * color.osg) * pow(max(0.0f,dot(N, H)),color.n)))); //green
        Ib += (shadow_flag * lightIntensity.z * atten * ( ((color.kd * color.odb) * (cosDiffuse)) + ((color.ks * color.osb) * pow(max(0.0f,dot(N, H)),color.n)))); //blue

    }

    //printf("Ir Ig Ib: %f %f %f\n", Ir, Ig, Ib);

    //if doing depthcue
    if (scene.depth) {
        //depth cueing
        float obj_dist = length(scene.eye - intersect);
        //printf("Obj Dist %f\n", obj_dist);
        float aDepth = 1.0f;
        if (obj_dist <= scene.depthCue.dMin) {
            aDepth = scene.depthCue.aMax;
        }
        else if (obj_dist <= scene.depthCue.dMax && obj_dist >= scene.depthCue.dMin) {
            aDepth = scene.depthCue.aMin + ((scene.depthCue.aMax - scene.depthCue.aMin) * ((scene.depthCue.dMax - obj_dist)/(scene.depthCue.dMax - scene.depthCue.dMin)));
        }
        else if (obj_dist >= scene.depthCue.dMax) {
            aDepth = scene.depthCue.aMin;
        }
        else {
            fprintf(stderr,"Should never get here!");
        }

        //clamp to 1

        Ir = min(1.0f, Ir);
        Ig = min(1.0f, Ig);
        Ib = min(1.0f, Ib);


        shaded_color.x = Ir;
        shaded_color.y = Ig;
        shaded_color.z = Ib;

        //printf("aDepht: %f\n", aDepth);

        shaded_color = (shaded_color * aDepth) + (scene.depthCue.color * (1.0f - aDepth));
    }

    else {

        //clamp to 1
        Ir = min(1.0f, Ir);
        Ig = min(1.0f, Ig);
        Ib = min(1.0f, Ib);

        shaded_color.x = Ir;
        shaded_color.y = Ig;
        shaded_color.z = Ib;
    }

    //cout << shaded_color << endl;
    return shaded_color;
}

vec3 shade_rayTriangle(Scene scene, Triangle triangle, vec3 intersect) {

    vec3 shaded_color;

    float Ir;
    float Ig;
    float Ib;
    Material color;

    if (triangle.index > -1) { //MATERIAL COLOR
        color = scene.materials[triangle.index];
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    

    vec3 lightIntensity; /// scene.lights.size();

    //surface normal vector
    vec3 N;

    //printf("TRIANGLE TYPE: %d", triangle.type);
    if (triangle.type == 0) { //flat shading
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        N = cross(e1,e2);
        N = unit(N);
    }

    else if (triangle.type == 1) { //smooth shading
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 e3 = intersect - triangle.p1;
        vec3 e4 = intersect - triangle.p2;
        float A = (0.5f) * length(cross(e1,e2));
        float a = (0.5f) * length(cross(e3,e4));
        float b = (0.5f) * length(cross(e4,e2));
        float c = (0.5f) * length(cross(e1,e3));
        float alpha = a/A;
        float beta = b/A;
        float gamma = c/A;
        //cout << "N0: " << triangle.n0 << endl;
        //cout << triangle.n1 << endl;
        //cout << triangle.n2 << endl;
        //printf("%f %f %f \n", alpha, beta, gamma);
        N = ((triangle.n0 * alpha) + (triangle.n1 * beta) + (triangle.n2 * gamma));
        N = unit(N);
    }

    else if (triangle.type == 2) { //texture
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 e3 = intersect - triangle.p1;
        vec3 e4 = intersect - triangle.p2;
        float A = (0.5f) * length(cross(e1,e2));
        float a = (0.5f) * length(cross(e3,e4));
        float b = (0.5f) * length(cross(e4,e2));
        float c = (0.5f) * length(cross(e1,e3));
        float alpha = a/A;
        float beta = b/A;
        float gamma = c/A;

        N = cross(e1,e2);
        N = unit(N);
        //vt0.x = u0, vt0.y = v0
        float u = (triangle.vt0.x * alpha) + (triangle.vt1.x * beta) + (triangle.vt2.x * gamma);
        float v = (triangle.vt0.y * alpha) + (triangle.vt1.y * beta) + (triangle.vt2.y * gamma);
        //printf("U and V value: %f %f\n", u,v);
        //printf("Texture size: %d %d\n", scene.textureHeight, scene.textureWidth);
        float i = round(u * (scene.textureImages[triangle.textureIndex].width - 1));
        float j = round(v * (scene.textureImages[triangle.textureIndex].height - 1));

        //make sure it doesn't go out of bounds

        if (i == scene.textureImages[triangle.textureIndex].width) {
            i -= 1;
        }
        if (j == scene.textureImages[triangle.textureIndex].height) {
            j-=1;
        }
        color.odr = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].x;
        color.odg = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].y;
        color.odb = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].z;
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    else if (triangle.type == 3) { //texture
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 e3 = intersect - triangle.p1;
        vec3 e4 = intersect - triangle.p2;
        float A = (0.5f) * length(cross(e1,e2));
        float a = (0.5f) * length(cross(e3,e4));
        float b = (0.5f) * length(cross(e4,e2));
        float c = (0.5f) * length(cross(e1,e3));
        float alpha = a/A;
        float beta = b/A;
        float gamma = c/A;
        N = ((triangle.n0 * alpha) + (triangle.n1 * beta) + (triangle.n2 * gamma));
        N = unit(N);
        //vt0.x = u0, vt0.y = v0
        float u = (triangle.vt0.x * alpha) + (triangle.vt1.x * beta) + (triangle.vt2.x * gamma);
        float v = (triangle.vt0.y * alpha) + (triangle.vt1.y * beta) + (triangle.vt2.y * gamma);
        //printf("U and V value: %f %f\n", u,v);
        //printf("Texture size: %d %d\n", scene.textureHeight, scene.textureWidth);
        float i = round(u * (scene.textureImages[triangle.textureIndex].width - 1));
        float j = round(v * (scene.textureImages[triangle.textureIndex].height - 1));

        if (i == scene.textureImages[triangle.textureIndex].width) {
            i -= 1;
        }
        if (j == scene.textureImages[triangle.textureIndex].height) {
            j-=1;
        }

        color.odr = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].x;
        color.odg = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].y;
        color.odb = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].z;
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    //Vector in direction of viewer
    vec3 V = scene.eye - intersect;
    //vec3 V = (scene.viewdir);
    V = unit(V);
    for (Light light : scene.lights) { //loop through lights

        float atten = 1.0f;

        lightIntensity = light.color;

        //For Calculate shadow
        Ray shadow;
        shadow.origin = intersect;
        float shadow_flag = 1.0f;
        vec3 vlight = light.position;
        float light_distance = length(vlight - intersect);

        
        vec3 L;

        if (light.w == 0) { //directional light
            L = vlight*(-1.0);
            L = unit(L);
            shadow.dir = L;
        }
        else {      //point light
            L = vlight - intersect;
            L = unit(L);
            shadow.dir = L;
            if (light.c.x != -1.0f) { //if -1, not doing light attenuation
                atten = 1.0f / (light.c.x + (light.c.y * light_distance) + (light.c.z * pow(light_distance,2)));
            }
        }

        //N dot L value
        //printf("N DOT L: %f\n", dot(N,L));
        float cosDiffuse = max(0.0f, dot(N,L));

        //Halfway vector
        vec3 H = (L + V);
        H = unit(H);
        
        //Check for shadow intersection
        

        for (Sphere s : scene.spheres) {
            float b = 2 * ( (shadow.dir.x * (shadow.origin.x - s.center.x)) + (shadow.dir.y * (shadow.origin.y - s.center.y)) + (shadow.dir.z * (shadow.origin.z - s.center.z)));
            float c = pow(shadow.origin.x - s.center.x,2) + pow(shadow.origin.y - s.center.y,2) + pow(shadow.origin.z - s.center.z,2) - pow(s.radius,2);
            float discriminant = (pow(b,2) - 4 * c);

            if (light.w == 0) { //directional light
                if (discriminant > 0) { //2 intersections
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    if (t1 > 0.01f || t2 > 0.01f) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        shadow_flag = 1;
                    }
                }
                else if (discriminant == 0) { //one intersection point
                    float t = -b / 2.0;
                    if (t > 0.01f) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        shadow_flag = 1;
                    }
                }
            }
            else { //point light
                if (discriminant > 0) { //2 intersections
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    float dist1 = length(shadow.dir * t1);
                    float dist2 = length(shadow.dir * t2);
                    if ((t1 > 0.01f && dist1 < light_distance) || (t2 > 0.01f && dist2 < light_distance)) {
                        if (scene.softshadow) {
                            //call soft shadow
                            //printf("Sphere coord: %f,%f,%f\n", s.center.x, s.center.y,s.center.z);
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        //printf("Intersect with itself or not in view\n");
                        //shadow_flag += 1;
                    }

                }
                else if (discriminant == 0) { //one intersection point
                    float t = -b / 2.0;
                    float dist = length(shadow.dir * t);
                    if ((t > 0.01f) && (dist < light_distance)) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                            //printf("Shadow_flag val : %f\n", shadow_flag);
                            
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        printf("Intersect with itself or not in view\n");
                        //shadow_flag += 1;
                    }
                }
            }
        }

        for (Triangle t: scene.triangles) { //check if any triangles cause shadow

            vec3 ee1 = t.p1 - t.p0;
            vec3 ee2 = t.p2 - t.p0;
            vec3 normal = cross(ee1,ee2);
            normal = unit(normal);
            //Ax + By + Cz + D = 0;
            float d;
            d = ((normal.x * t.p0.x) + (normal.y * t.p0.y) + (normal.z * t.p0.z)) * (-1);
            float triangle_t;
            float numer = (-1) * ((normal.x * shadow.origin.x) + (normal.y * shadow.origin.y) + (normal.z * shadow.origin.z) + d);
            float denom = ((normal.x * shadow.dir.x) + (normal.y * shadow.dir.y) + (normal.z * shadow.dir.z));
            if (denom == 0) {
                //no intersection
                shadow_flag = 1;
            }
            else {
                triangle_t = numer / denom;
                //printf("TRIANGLE_T: %f", triangle_t);
                float dist = length(shadow.dir * triangle_t);
                if (light.w == 1) { //point light
                    if (triangle_t > 0.01f && dist < light_distance) { //THIS IS INTERSECTION
                        //printf("INTERSECT WITH PLANE\n");
                        vec3 p = (shadow.origin + (shadow.dir * triangle_t));
                        vec3 e3 = p - t.p1;
                        vec3 e4 = p - t.p2;
                        float A = (0.5f) * length(cross(ee1,ee2));
                        float a = (0.5f) * length(cross(e3,e4));
                        float b = (0.5f) * length(cross(e4,ee2));
                        float c = (0.5f) * length(cross(ee1,e3));
                        float alpha = a/A;
                        float beta = b/A;
                        float gamma = c/A;
                        if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                            if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.999) {
                                //printf("SHADOW\n");
                                shadow_flag = 0;
                            }
                            else {
                                shadow_flag = 1;
                            }
                        }
                    }
                }
                else { //direcitonal light
                    if (triangle_t > 0.01f) { //THIS IS INTERSECTION
                        //printf("INTERSECT WITH PLANE\n");
                        vec3 p = (shadow.origin + (shadow.dir * triangle_t));
                        vec3 e3 = p - t.p1;
                        vec3 e4 = p - t.p2;
                        float A = (0.5f) * length(cross(ee1,ee2));
                        float a = (0.5f) * length(cross(e3,e4));
                        float b = (0.5f) * length(cross(e4,ee2));
                        float c = (0.5f) * length(cross(ee1,e3));
                        float alpha = a/A;
                        float beta = b/A;
                        float gamma = c/A;
                        if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                            if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.999) {
                                //printf("SHADOW\n");
                                shadow_flag = 0;
                            }
                            else {
                                shadow_flag = 1;
                            }
                        }
                    }
                }
            }
        }

        shadow_flag = min(1.0f,shadow_flag);
        shadow_flag = max(0.0f, shadow_flag);

        //printf("%f\n",  (pow(max(0.0f,dot(N, H)),color.n)));

        Ir += (shadow_flag * lightIntensity.x * atten * ( ((color.kd * color.odr) * (cosDiffuse)) + ((color.ks * color.osr) * pow(max(0.0f,dot(N, H)),color.n)))); //red
        Ig += (shadow_flag * lightIntensity.y * atten * ( ((color.kd * color.odg) * (cosDiffuse)) + ((color.ks * color.osg) * pow(max(0.0f,dot(N, H)),color.n)))); //green
        Ib += (shadow_flag * lightIntensity.z * atten * ( ((color.kd * color.odb) * (cosDiffuse)) + ((color.ks * color.osb) * pow(max(0.0f,dot(N, H)),color.n)))); //blue
    }

    //if doing depthcue
    if (scene.depth) {
        //depth cueing
        float obj_dist = length(scene.eye - intersect);
        //printf("Obj Dist %f\n", obj_dist);
        float aDepth = 1.0f;
        if (obj_dist <= scene.depthCue.dMin) {
            aDepth = scene.depthCue.aMax;
        }
        else if (obj_dist <= scene.depthCue.dMax && obj_dist >= scene.depthCue.dMin) {
            aDepth = scene.depthCue.aMin + ((scene.depthCue.aMax - scene.depthCue.aMin) * ((scene.depthCue.dMax - obj_dist)/(scene.depthCue.dMax - scene.depthCue.dMin)));
        }
        else if (obj_dist >= scene.depthCue.dMax) {
            aDepth = scene.depthCue.aMin;
        }
        else {
            fprintf(stderr,"Should never get here!");
        }

        //clamp to 1

        Ir = min(1.0f, Ir);
        Ig = min(1.0f, Ig);
        Ib = min(1.0f, Ib);


        shaded_color.x = Ir;
        shaded_color.y = Ig;
        shaded_color.z = Ib;

        //printf("aDepht: %f\n", aDepth);

        shaded_color = (shaded_color * aDepth) + (scene.depthCue.color * (1.0f - aDepth));
    }

    else {
        Ir = min(1.0f, Ir);
        Ig = min(1.0f, Ig);
        Ib = min(1.0f, Ib);

        shaded_color.x = Ir;
        shaded_color.y = Ig;
        shaded_color.z = Ib;
    }

    //cout << shaded_color << endl;
    return shaded_color;
}

vec3 shade_rayCylinder(Scene scene, Cylinder cylinder, vec3 intersect) {

    vec3 shaded_color;

    float Ir;
    float Ig;
    float Ib;
    Material color;

    if (triangle.index > -1) { //MATERIAL COLOR
        color = scene.materials[triangle.index];
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    

    vec3 lightIntensity; /// scene.lights.size();

    //surface normal vector
    vec3 N;

    //printf("TRIANGLE TYPE: %d", triangle.type);
    if (triangle.type == 0) { //flat shading
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        N = cross(e1,e2);
        N = unit(N);
    }

    else if (triangle.type == 1) { //smooth shading
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 e3 = intersect - triangle.p1;
        vec3 e4 = intersect - triangle.p2;
        float A = (0.5f) * length(cross(e1,e2));
        float a = (0.5f) * length(cross(e3,e4));
        float b = (0.5f) * length(cross(e4,e2));
        float c = (0.5f) * length(cross(e1,e3));
        float alpha = a/A;
        float beta = b/A;
        float gamma = c/A;
        //cout << "N0: " << triangle.n0 << endl;
        //cout << triangle.n1 << endl;
        //cout << triangle.n2 << endl;
        //printf("%f %f %f \n", alpha, beta, gamma);
        N = ((triangle.n0 * alpha) + (triangle.n1 * beta) + (triangle.n2 * gamma));
        N = unit(N);
    }

    else if (triangle.type == 2) { //texture
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 e3 = intersect - triangle.p1;
        vec3 e4 = intersect - triangle.p2;
        float A = (0.5f) * length(cross(e1,e2));
        float a = (0.5f) * length(cross(e3,e4));
        float b = (0.5f) * length(cross(e4,e2));
        float c = (0.5f) * length(cross(e1,e3));
        float alpha = a/A;
        float beta = b/A;
        float gamma = c/A;

        N = cross(e1,e2);
        N = unit(N);
        //vt0.x = u0, vt0.y = v0
        float u = (triangle.vt0.x * alpha) + (triangle.vt1.x * beta) + (triangle.vt2.x * gamma);
        float v = (triangle.vt0.y * alpha) + (triangle.vt1.y * beta) + (triangle.vt2.y * gamma);
        //printf("U and V value: %f %f\n", u,v);
        //printf("Texture size: %d %d\n", scene.textureHeight, scene.textureWidth);
        float i = round(u * (scene.textureImages[triangle.textureIndex].width - 1));
        float j = round(v * (scene.textureImages[triangle.textureIndex].height - 1));

        //make sure it doesn't go out of bounds

        if (i == scene.textureImages[triangle.textureIndex].width) {
            i -= 1;
        }
        if (j == scene.textureImages[triangle.textureIndex].height) {
            j-=1;
        }
        color.odr = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].x;
        color.odg = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].y;
        color.odb = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].z;
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    else if (triangle.type == 3) { //texture
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 e3 = intersect - triangle.p1;
        vec3 e4 = intersect - triangle.p2;
        float A = (0.5f) * length(cross(e1,e2));
        float a = (0.5f) * length(cross(e3,e4));
        float b = (0.5f) * length(cross(e4,e2));
        float c = (0.5f) * length(cross(e1,e3));
        float alpha = a/A;
        float beta = b/A;
        float gamma = c/A;
        N = ((triangle.n0 * alpha) + (triangle.n1 * beta) + (triangle.n2 * gamma));
        N = unit(N);
        //vt0.x = u0, vt0.y = v0
        float u = (triangle.vt0.x * alpha) + (triangle.vt1.x * beta) + (triangle.vt2.x * gamma);
        float v = (triangle.vt0.y * alpha) + (triangle.vt1.y * beta) + (triangle.vt2.y * gamma);
        //printf("U and V value: %f %f\n", u,v);
        //printf("Texture size: %d %d\n", scene.textureHeight, scene.textureWidth);
        float i = round(u * (scene.textureImages[triangle.textureIndex].width - 1));
        float j = round(v * (scene.textureImages[triangle.textureIndex].height - 1));

        if (i == scene.textureImages[triangle.textureIndex].width) {
            i -= 1;
        }
        if (j == scene.textureImages[triangle.textureIndex].height) {
            j-=1;
        }

        color.odr = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].x;
        color.odg = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].y;
        color.odb = scene.textureImages[triangle.textureIndex].imageArray[int(j)][int(i)].z;
        Ir = color.ka * color.odr; //ambient
        Ig = color.ka * color.odg; //ambient
        Ib = color.ka * color.odb; //ambient
    }
    //Vector in direction of viewer
    vec3 V = scene.eye - intersect;
    //vec3 V = (scene.viewdir);
    V = unit(V);
    for (Light light : scene.lights) { //loop through lights

        float atten = 1.0f;

        lightIntensity = light.color;

        //For Calculate shadow
        Ray shadow;
        shadow.origin = intersect;
        float shadow_flag = 1.0f;
        vec3 vlight = light.position;
        float light_distance = length(vlight - intersect);

        
        vec3 L;

        if (light.w == 0) { //directional light
            L = vlight*(-1.0);
            L = unit(L);
            shadow.dir = L;
        }
        else {      //point light
            L = vlight - intersect;
            L = unit(L);
            shadow.dir = L;
            if (light.c.x != -1.0f) { //if -1, not doing light attenuation
                atten = 1.0f / (light.c.x + (light.c.y * light_distance) + (light.c.z * pow(light_distance,2)));
            }
        }

        //N dot L value
        //printf("N DOT L: %f\n", dot(N,L));
        float cosDiffuse = max(0.0f, dot(N,L));

        //Halfway vector
        vec3 H = (L + V);
        H = unit(H);
        
        //Check for shadow intersection
        

        for (Sphere s : scene.spheres) {
            float b = 2 * ( (shadow.dir.x * (shadow.origin.x - s.center.x)) + (shadow.dir.y * (shadow.origin.y - s.center.y)) + (shadow.dir.z * (shadow.origin.z - s.center.z)));
            float c = pow(shadow.origin.x - s.center.x,2) + pow(shadow.origin.y - s.center.y,2) + pow(shadow.origin.z - s.center.z,2) - pow(s.radius,2);
            float discriminant = (pow(b,2) - 4 * c);

            if (light.w == 0) { //directional light
                if (discriminant > 0) { //2 intersections
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    if (t1 > 0.01f || t2 > 0.01f) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        shadow_flag = 1;
                    }
                }
                else if (discriminant == 0) { //one intersection point
                    float t = -b / 2.0;
                    if (t > 0.01f) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        shadow_flag = 1;
                    }
                }
            }
            else { //point light
                if (discriminant > 0) { //2 intersections
                    float t1 = (-b + sqrt(discriminant)) / 2.0;
                    float t2 = (-b - sqrt(discriminant)) / 2.0;
                    float dist1 = length(shadow.dir * t1);
                    float dist2 = length(shadow.dir * t2);
                    if ((t1 > 0.01f && dist1 < light_distance) || (t2 > 0.01f && dist2 < light_distance)) {
                        if (scene.softshadow) {
                            //call soft shadow
                            //printf("Sphere coord: %f,%f,%f\n", s.center.x, s.center.y,s.center.z);
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        //printf("Intersect with itself or not in view\n");
                        //shadow_flag += 1;
                    }

                }
                else if (discriminant == 0) { //one intersection point
                    float t = -b / 2.0;
                    float dist = length(shadow.dir * t);
                    if ((t > 0.01f) && (dist < light_distance)) {
                        if (scene.softshadow) {
                            //call soft shadow
                            shadow_flag = soft_shadow(shadow, s, light, intersect);
                            //printf("Shadow_flag val : %f\n", shadow_flag);
                            
                        }
                        else {
                            shadow_flag = 0;
                        }
                    }
                    else {
                        printf("Intersect with itself or not in view\n");
                        //shadow_flag += 1;
                    }
                }
            }
        }

        for (Triangle t: scene.triangles) { //check if any triangles cause shadow

            vec3 ee1 = t.p1 - t.p0;
            vec3 ee2 = t.p2 - t.p0;
            vec3 normal = cross(ee1,ee2);
            normal = unit(normal);
            //Ax + By + Cz + D = 0;
            float d;
            d = ((normal.x * t.p0.x) + (normal.y * t.p0.y) + (normal.z * t.p0.z)) * (-1);
            float triangle_t;
            float numer = (-1) * ((normal.x * shadow.origin.x) + (normal.y * shadow.origin.y) + (normal.z * shadow.origin.z) + d);
            float denom = ((normal.x * shadow.dir.x) + (normal.y * shadow.dir.y) + (normal.z * shadow.dir.z));
            if (denom == 0) {
                //no intersection
                shadow_flag = 1;
            }
            else {
                triangle_t = numer / denom;
                //printf("TRIANGLE_T: %f", triangle_t);
                float dist = length(shadow.dir * triangle_t);
                if (light.w == 1) { //point light
                    if (triangle_t > 0.01f && dist < light_distance) { //THIS IS INTERSECTION
                        //printf("INTERSECT WITH PLANE\n");
                        vec3 p = (shadow.origin + (shadow.dir * triangle_t));
                        vec3 e3 = p - t.p1;
                        vec3 e4 = p - t.p2;
                        float A = (0.5f) * length(cross(ee1,ee2));
                        float a = (0.5f) * length(cross(e3,e4));
                        float b = (0.5f) * length(cross(e4,ee2));
                        float c = (0.5f) * length(cross(ee1,e3));
                        float alpha = a/A;
                        float beta = b/A;
                        float gamma = c/A;
                        if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                            if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.999) {
                                //printf("SHADOW\n");
                                shadow_flag = 0;
                            }
                            else {
                                shadow_flag = 1;
                            }
                        }
                    }
                }
                else { //direcitonal light
                    if (triangle_t > 0.01f) { //THIS IS INTERSECTION
                        //printf("INTERSECT WITH PLANE\n");
                        vec3 p = (shadow.origin + (shadow.dir * triangle_t));
                        vec3 e3 = p - t.p1;
                        vec3 e4 = p - t.p2;
                        float A = (0.5f) * length(cross(ee1,ee2));
                        float a = (0.5f) * length(cross(e3,e4));
                        float b = (0.5f) * length(cross(e4,ee2));
                        float c = (0.5f) * length(cross(ee1,e3));
                        float alpha = a/A;
                        float beta = b/A;
                        float gamma = c/A;
                        if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                            if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.999) {
                                //printf("SHADOW\n");
                                shadow_flag = 0;
                            }
                            else {
                                shadow_flag = 1;
                            }
                        }
                    }
                }
            }
        }

        shadow_flag = min(1.0f,shadow_flag);
        shadow_flag = max(0.0f, shadow_flag);

        //printf("%f\n",  (pow(max(0.0f,dot(N, H)),color.n)));

        Ir += (shadow_flag * lightIntensity.x * atten * ( ((color.kd * color.odr) * (cosDiffuse)) + ((color.ks * color.osr) * pow(max(0.0f,dot(N, H)),color.n)))); //red
        Ig += (shadow_flag * lightIntensity.y * atten * ( ((color.kd * color.odg) * (cosDiffuse)) + ((color.ks * color.osg) * pow(max(0.0f,dot(N, H)),color.n)))); //green
        Ib += (shadow_flag * lightIntensity.z * atten * ( ((color.kd * color.odb) * (cosDiffuse)) + ((color.ks * color.osb) * pow(max(0.0f,dot(N, H)),color.n)))); //blue
    }

    //if doing depthcue
    if (scene.depth) {
        //depth cueing
        float obj_dist = length(scene.eye - intersect);
        //printf("Obj Dist %f\n", obj_dist);
        float aDepth = 1.0f;
        if (obj_dist <= scene.depthCue.dMin) {
            aDepth = scene.depthCue.aMax;
        }
        else if (obj_dist <= scene.depthCue.dMax && obj_dist >= scene.depthCue.dMin) {
            aDepth = scene.depthCue.aMin + ((scene.depthCue.aMax - scene.depthCue.aMin) * ((scene.depthCue.dMax - obj_dist)/(scene.depthCue.dMax - scene.depthCue.dMin)));
        }
        else if (obj_dist >= scene.depthCue.dMax) {
            aDepth = scene.depthCue.aMin;
        }
        else {
            fprintf(stderr,"Should never get here!");
        }

        //clamp to 1

        Ir = min(1.0f, Ir);
        Ig = min(1.0f, Ig);
        Ib = min(1.0f, Ib);


        shaded_color.x = Ir;
        shaded_color.y = Ig;
        shaded_color.z = Ib;

        //printf("aDepht: %f\n", aDepth);

        shaded_color = (shaded_color * aDepth) + (scene.depthCue.color * (1.0f - aDepth));
    }

    else {
        Ir = min(1.0f, Ir);
        Ig = min(1.0f, Ig);
        Ib = min(1.0f, Ib);

        shaded_color.x = Ir;
        shaded_color.y = Ig;
        shaded_color.z = Ib;
    }

    //cout << shaded_color << endl;
    return shaded_color;
}


vec3 trace_ray(Ray ray, Scene scene) {

    bool intersect = false;
    bool inter_triangle = false;
    bool inter_cylinder = false;

    Sphere closest_sphere;

    float closest_t = 100000;

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
                    inter_triangle = false;
                }
            }
            else if (t2 < 0 && t1 > 0) {  //t2 is under 0
                if (t1 < closest_t) {
                    closest_sphere = sphere;
                    closest_t = t1;
                    intersect = true;
                    inter_triangle = false;
                }
            }

            else if (t2 > 0 && t1 > 0) { //both ts are positive
                float t = min(t1,t2);
                if (t < closest_t) {
                    closest_sphere = sphere;
                    closest_t = t;
                    intersect = true;
                    inter_triangle = false;
                }
            }
        }
        else if (discriminant == 0) { //one intersection point
             float t = -b / 2.0;
             if (t > 0 && t < closest_t) {
                 closest_t = t;
                 closest_sphere = sphere;
                 intersect = true;
                 inter_triangle = false;
             }
        }
    }

    //Triangle Intersection
    Triangle closest_triangle;

    for (Triangle triangle: scene.triangles) {
        vec3 e1 = triangle.p1 - triangle.p0;
        vec3 e2 = triangle.p2 - triangle.p0;
        vec3 n = cross(e1,e2);
        n = unit(n);

        //Ax + By + Cz + D = 0;
        float d;
        d = ((n.x * triangle.p0.x) + (n.y * triangle.p0.y) + (n.z * triangle.p0.z)) * (-1);
        float triangle_t;
        float numer = (-1) * ((n.x * ray.origin.x) + (n.y * ray.origin.y) + (n.z * ray.origin.z) + d);
        float denom = ((n.x * ray.dir.x) + (n.y * ray.dir.y) + (n.z * ray.dir.z));
        if (denom == 0) {
            //no intersection
        }
        else {
            triangle_t = numer / denom;
            //printf("TRIANGLE_T: %f", triangle_t);
            if (triangle_t < closest_t && triangle_t > 0) { //THIS IS INTERSECTION
                //printf("INTERSECT WITH PLANE\n");
                vec3 p = (ray.origin + (ray.dir * triangle_t));
                vec3 e3 = p - triangle.p1;
                vec3 e4 = p - triangle.p2;
                float A = (0.5f) * length(cross(e1,e2));
                float a = (0.5f) * length(cross(e3,e4));
                float b = (0.5f) * length(cross(e4,e2));
                float c = (0.5f) * length(cross(e1,e3));
                float alpha = a/A;
                float beta = b/A;
                float gamma = c/A;
                if ((alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1) && (beta >= 0 && beta <= 1 )) { //in triangle
                    if (alpha + gamma + beta < 1.001 && alpha + gamma + beta > 0.999) {
                        closest_t = triangle_t;
                        intersect = true;
                        closest_triangle = triangle;
                        inter_triangle = true;
                    }
                }
            }
        }
    }

    Cylinder closest_cylinder;

    for (Cylinder cylinder: scene.cylinders) {
        vec3 p2 = cylinder.center + (unit(cylinder.dir)*(cylinder.length/2.0f));
        vec3 p1 = cylinder.center - (unit(cylinder.dir)*(cylinder.length/2.0f));
        cylinder.dir = (p2-p1)/length(p2-p1);
        float a = dot(ray.dir - (cylinder.dir * dot(ray.dir,cylinder.dir)),ray.dir - (cylinder.dir * dot(ray.dir,cylinder.dir)));
        float b = 2* (dot(ray.dir - (cylinder.dir * dot(ray.dir,cylinder.dir)), (ray.origin-cylinder.center - (cylinder.dir * dot((ray.origin-cylinder.center),cylinder.dir)))));
        float c = dot(ray.origin-cylinder.center - (cylinder.dir * dot(ray.origin-cylinder.center,cylinder.dir)),ray.origin-cylinder.center - (cylinder.dir * dot(ray.origin-cylinder.center,cylinder.dir))) - pow(cylinder.radius,2); 
        float discriminant = (pow(b,2) - 4 * a * c);
        
        if (discriminant > 0) {
            float t1 = (-b + sqrt(discriminant)) / (2.0f * a); 
            float t2 = (-b - sqrt(discriminant)) / (2.0f * a);

            vec3 z1,z2;

            z1 = ray.origin + (ray.dir * t1);
            z2 = ray.origin + (ray.dir * t2);

            if (t1 < 0 && t2 > 0) {       //t1 is under 0
                if (t2 < closest_t) {
                    if (dot(cylinder.dir, z2-p1) > 0 && dot(cylinder.dir, z2-p2) < 0 ) {
                        closest_cylinder = cylinder;
                        closest_t = t2;
                        intersect = true;
                        inter_triangle = false;
                        inter_cylinder = true;
                    }
                }
            }
            else if (t2 < 0 && t1 > 0) {  //t2 is under 0
                if (t1 < closest_t) {
                    if (dot(cylinder.dir, z1-p1) > 0 && dot(cylinder.dir, z1-p2) < 0 ) { 
                        closest_cylinder = cylinder;
                        closest_t = t1;
                        intersect = true;
                        inter_triangle = false;
                        inter_cylinder = true;
                    }
                }
            }

            else if (t2 > 0 && t1 > 0) { //both ts are positive
                float t = min(t1,t2);
                vec3 z = ray.origin + (ray.dir * t);
                if (t < closest_t) {
                    if (dot(cylinder.dir, z-p1) > 0 && dot(cylinder.dir, z-p2) < 0 ) {
                        closest_cylinder = cylinder;
                        closest_t = t;
                        //printf("Ray Z: %f\n",(ray.origin + (ray.dir * closest_t)).z);
                        intersect = true;
                        inter_triangle = false;
                        inter_cylinder = true;
                    }
                    else { //if at the cut off, fill the furthest part
                        t = max(t1,t2);
                        z = ray.origin + (ray.dir * t);
                        if (dot(cylinder.dir, z-p1) > 0 && dot(cylinder.dir, z-p2) < 0 ) {
                            closest_cylinder = cylinder;
                            closest_t = t2;
                            //printf("Ray Z: %f\n",(ray.origin + (ray.dir * closest_t)).z);
                            intersect = true;
                            inter_triangle = false;
                            inter_cylinder = true;
                        }
                    }
                }
            }
        }
        else if (discriminant == 0) { //one intersection point
            float t = -b / (2.0f * a);
            vec3 z = ray.origin + (ray.dir * t);
            if (t > 0 && t < closest_t) {
                if (dot(cylinder.dir, z-p1) > 0 && dot(cylinder.dir, z-p2) < 0 ) {
                    closest_t = t;
                    closest_cylinder = cylinder;
                    intersect = true;
                    inter_triangle = false;
                    inter_cylinder = true;
                }
            }
        }
    }


    if (!intersect) {
        //printf("No INTERSECTION\n");
        return scene.bkgcolor;
    }
    else {
        //printf("T: %f\n", closest_t);
        vec3 intersection_point;
        intersection_point.x = (ray.origin.x + ray.dir.x*closest_t);
        intersection_point.y = (ray.origin.y + ray.dir.y*closest_t);
        intersection_point.z =(ray.origin.z + ray.dir.z*closest_t);

        if (inter_triangle) { //calculate triangle shading
            return shade_rayTriangle(scene, closest_triangle, intersection_point);
        }

        else if (inter_cylinder) {
            return shade_rayCylinder(scene, closest_cylinder, intersection_point);
        }

        else { //sphere shading
            return shade_ray(scene, closest_sphere, intersection_point);
        }
    }
}


int main(int argc, char* argv[]) {
    Scene scene;


    float viewdist = 10.0f;
    if (argc == 3) {
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

    //Create the triangles
    for (int i = 0; i < scene.faces.size(); i ++) {
        if (scene.faces[i].type == 0) { //f 0 0 0
            printf("TYPE 0\n");
            vec3 p0, p1, p2;
            p0 = scene.vertices[scene.faces[i].v.x-1];
            p1 = scene.vertices[scene.faces[i].v.y-1];
            p2 = scene.vertices[scene.faces[i].v.z-1];

            Triangle t {
                .p0 = p0,
                .p1 = p1,
                .p2 = p2,
                .index = scene.faces[i].index,
                .textureIndex = scene.faces[i].textureIndex,
                .type = scene.faces[i].type,
            };
            scene.triangles.push_back(t);
        }
        else if (scene.faces[i].type == 1) { //f 0//1 0//1 0//1 smooth shaded untexture v//n
            vec3 p0, p1, p2;
            vec3 n0, n1, n2;
            p0 = scene.vertices[scene.faces[i].v.x-1];
            p1 = scene.vertices[scene.faces[i].v.y-1];
            p2 = scene.vertices[scene.faces[i].v.z-1];
            printf("TYPE 1 \n");
            n0 = scene.norm_vertices[scene.faces[i].vn.x-1];
            n1 = scene.norm_vertices[scene.faces[i].vn.y-1];
            n2 = scene.norm_vertices[scene.faces[i].vn.z-1];

            Triangle t {
                .p0 = p0,
                .p1 = p1,
                .p2 = p2,
                .n0 = n0,
                .n1 = n1,
                .n2 = n2,
                .index = scene.faces[i].index,
                .textureIndex = scene.faces[i].textureIndex,
                .type = scene.faces[i].type,
            };
            scene.triangles.push_back(t);
        }
        else if (scene.faces[i].type == 2) { //f 0/1 0/1 0/1 texture w/o smooth-shading v/t
            printf("TYPE 2 \n");
            vec3 p0, p1, p2;
            vec3 vt0, vt1, vt2;
            p0 = scene.vertices[scene.faces[i].v.x-1];
            p1 = scene.vertices[scene.faces[i].v.y-1];
            p2 = scene.vertices[scene.faces[i].v.z-1];
            vt0 = scene.texture_coords[scene.faces[i].vt.x-1];
            vt1 = scene.texture_coords[scene.faces[i].vt.y-1];
            vt2 = scene.texture_coords[scene.faces[i].vt.z-1];

            Triangle t {
                .p0 = p0,
                .p1 = p1,
                .p2 = p2,
                .vt0 = vt0,
                .vt1 = vt1,
                .vt2 = vt2,
                .index = scene.faces[i].index,
                .textureIndex = scene.faces[i].textureIndex,
                .type = scene.faces[i].type,
            };
            scene.triangles.push_back(t);
        }
        else if (scene.faces[i].type == 3) { //f 0/1/1 0/1/1 0/1/1 smooth shaded texture triangle v/t/n
            printf("TYPE 3 \n");
            vec3 p0, p1, p2;
            vec3 n0, n1, n2; 
            vec3 vt0, vt1, vt2;
            p0 = scene.vertices[scene.faces[i].v.x-1];
            p1 = scene.vertices[scene.faces[i].v.y-1];
            p2 = scene.vertices[scene.faces[i].v.z-1];
            n0 = scene.norm_vertices[scene.faces[i].vn.x-1];
            n1 = scene.norm_vertices[scene.faces[i].vn.y-1];
            n2 = scene.norm_vertices[scene.faces[i].vn.z-1];
            vt0 = scene.texture_coords[scene.faces[i].vt.x-1];
            vt1 = scene.texture_coords[scene.faces[i].vt.y-1];
            vt2 = scene.texture_coords[scene.faces[i].vt.z-1];

            Triangle t {
                .p0 = p0,
                .p1 = p1,
                .p2 = p2,
                .n0 = n0,
                .n1 = n1,
                .n2 = n2,
                .vt0 = vt0,
                .vt1 = vt1,
                .vt2 = vt2,
                .index = scene.faces[i].index,
                .textureIndex = scene.faces[i].textureIndex,
                .type = scene.faces[i].type,
            };
            scene.triangles.push_back(t);
        }
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

    float fov_r = (scene.vfov/2.0) * (pi/180.0);
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

    string outputfile = argv[2];
    ofstream output_stream(outputfile, ios::out | ios::binary);

    output_stream << "P3 "
    << scene.width << " "
    << scene.height << " "
    << 255 << "\n";

    for (int j = 0; j < scene.height; j++) {
        for (int i = 0; i < scene.width; i++) {
            output_stream << imageArray[j][i].x*255 << " " << imageArray[j][i].y*255 << " " << imageArray[j][i].z*255 << "\n";
        }
    }

    //Delete
    delete[] imageArray;
    for (int i = 0; i < scene.textureImages.size(); i ++) {
        delete[] scene.textureImages[i].imageArray;
    }


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


    //testing triangel intersection
    //         triangle.p0 = {
    //         .x = 1,
    //         .y = 0,
    //         .z = 0,
    //     };
    //     triangle.p1 = {
    //         .x = 0,
    //         .y = 1,
    //         .z = 0,
    //     };
    //     triangle.p2 = {
    //         .x = 0,
    //         .y = 0,
    //         .z = 1,
    //     };

    //     vec3 orig {
    //     .x = 0,
    //     .y = 0,
    //     .z = 0,
    // };

    // vec3 d {
    //     .x = 1.0f/3.0f,
    //     .y = 2.0f/3.0f,
    //     .z = 2.0f/3.0f,
    // };

    // ray = {
    //     .origin = orig,
    //     .dir = d,
    // };