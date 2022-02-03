#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    
    int WIDTH = 0;
    int HEIGHT = 0;

    if (argc == 2) {
        //get file name from arguments
        string input_file = argv[1];
        string line;
        ifstream file;
        //open file
        file.open(input_file);
        if (!file.is_open()) {
            cout << "File not found" << endl;
            exit(EXIT_FAILURE);
        }
        //parse file
        if (getline(file, line)) {
            int pos = 0;
            string delimiter = " ";
            int count = 0;
            //find position of spaces
            while ((pos = line.find(delimiter)) != string::npos) {  
                if (count == 0) {          
                    //check keyword
                    if (line.substr(0,pos) != "imsize") {
                        cout << "Wrong keyword" << endl;
                        exit(EXIT_FAILURE);
                    }
                    line.erase(0, pos + delimiter.length());
                    count ++;
                }

                //get width
                else if (count == 1) {
                    try {
                        WIDTH = abs(stoi(line.substr(0,pos)));
                    }
                    catch (exception &err)
                    {
                        cout << "not numerical" << endl;
                        exit(EXIT_FAILURE);
                    }
                    //chop head off line
                    line.erase(0, pos + delimiter.length());
                }
            }
            //get height
            try {
                        HEIGHT = abs(stoi(line));
            }
            catch (exception &err)
            {
                cout << "not numerical" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        cout<< "Error no file" << endl;
        return 0;
    }
    //create and name outputfile and output_stream
    string outputfile = "myppm.ppm";
    ofstream output_stream(outputfile, ios::out | ios::binary);


    //header for ppm
    output_stream << "P3\n"
    << WIDTH << " "
    << HEIGHT << "\n"
    << 255 << "\n";

    for (uint32_t y = 0; y < HEIGHT; y++) {
        for (uint32_t x = 0; x < WIDTH; x++) {
            //cout << ((y*WIDTH+x)+1)%2 << " ";
            if (((y*WIDTH + x)+y) % 2 == 0) {
                output_stream << float(y)*float(10)/float(HEIGHT) << " " << float(y)*float(255)/float(HEIGHT) << " " << float(y)*float(140)/float(HEIGHT) << " ";
            }
            else {
                output_stream << float(x)*float(255)/float(WIDTH) << " " << float(x)*float(155)/float(WIDTH) << " " << float(x)*float(10)/float(WIDTH) << " ";
            }
        }
    }

    output_stream.close();
    return 0;

}
