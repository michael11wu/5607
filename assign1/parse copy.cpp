#include <type.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

ColorType Trace_Ray( RayType ray )   {    
    ColorType return_color;
    return_color.r = 1;
    return_color.g = 0;
    return_color.b = 0;
    /* “ray” specifies the incident ray (origin and 
    direction); for assignment 1a it is the ray
    from the eye through a point in the view window */
    /* please be forewarned that this parameter list will
    evolve; later on we will need to pass in 
    more information */
    
    /* for each object in the scene, check for a 
    ray/object intersection; keep track of the 
    closest intersection point to the eye and the   
    identity of the object hit at that point */
    /* call Shade_Ray() to determine the color */  
    return return_color;
}

// ColorType Shade_Ray( parameter list ) {
//     /* for assignment 1a, we only need to pass in an
//     identifier that allows access to the material 
//     properties of the intersected object; in the future,
//     we will need to also provide the coordinates of the
//     point where the shading will be computed */
   
//     /* compute the color at the intersection point; presently
//     this consists of just returning the sphere’s material
//     color. Later, the illumination calculations will get
//     more complicated, and you will eventually make
//     recursive calls to TraceRay from within Shade_Ray when 
//     your program is extended */
//     return( computed_color );
// }



int main(int argc, char* argv[]) {

    vector<float> eye;
    vector<float> viewdir;
    vector<float> updir;
    float vfov = 0;
    vector<int> imsize;
    vector<float> bkgcolor;
    vector<float> mtlcolor;
    vector<float> sphere;

    float x = 0,y = 0,z = 0;
    int width = 0, height = 0;
    float r = 0,g = 0,b = 0;
    float rad = 0;

    if (argc == 2) {
        //get file name from arguments
        char filename[100];
        strcpy(filename, argv[1]);
        char line[50];
        //open file
        FILE *fp = fopen(filename,"r");
        if (fp == NULL) {
            cout << "File not found" << endl;
            exit(EXIT_FAILURE);
        }
        //parse file
        while (fgets(line, sizeof(line), fp)!=NULL) {
            char keyword[10];
            sscanf(line, "%s", keyword);
            if (strcmp(keyword,"eye")) {
                sscanf(line, "%s %f %f %f", keyword, &x, &y, &z);
                eye.push_back(x);
                eye.push_back(y);
                eye.push_back(z);
            }
            else if (strcmp(keyword,"viewdir")) {
                sscanf(line, "%s %f %f %f", keyword, &x, &y, &z);
                viewdir.push_back(x);
                viewdir.push_back(y);
                viewdir.push_back(z);
            }
            else if (strcmp(keyword,"updir")) {
                sscanf(line, "%s %f %f %f", keyword, &x, &y, &z);
                updir.push_back(x);
                updir.push_back(y);
                updir.push_back(z);
            }
            else if (strcmp(keyword,"vfov")) {
                sscanf(line, "%s %f", keyword, &vfov);
            }
            else if (strcmp(keyword,"imsize")) {
                sscanf(line, "%s %d %d", keyword, &width, &height);
            }
            else if (strcmp(keyword,"bkgcolor")) {
                sscanf(line, "%s %f %f %f", keyword, &r, &g, &b);
                bkgcolor.push_back(r);
                bkgcolor.push_back(g);
                bkgcolor.push_back(b);
            }
            else if (strcmp(keyword,"mtlcolor")) {
                sscanf(line, "%s %f %f %f", keyword, &r, &g, &b);
                mtlcolor.push_back(r);
                mtlcolor.push_back(g);
                mtlcolor.push_back(b);
            }
            else if (strcmp(keyword,"sphere")) {
                sscanf(line, "%s %f %f %f %f", keyword, &x, &y, &z, &rad);
                sphere.push_back(x);
                sphere.push_back(y);
                sphere.push_back(z);
                sphere.push_back(rad);
            }
            else if (strcmp(keyword,"cylinder")) {
                //
            }

        }

    }
    else {
        cout<< "Error no file" << endl;
        return 1;
    }
    return 0;

}

