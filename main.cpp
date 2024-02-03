#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <unistd.h>
#include <chrono>

using namespace cv;
using namespace std;

void encode(const string& name);
void decode(const string& name);
void thirdOption(const string& name);

string getExtension(const string& fileName){ //Gets the inputted file extension
    string extension;                        //
    bool pass = false;
    for(char c : fileName){
        if(c == '.') pass = true;
        if(pass) extension += c;
    }
    return extension;
}
string textToBinary(const string& text){
    string binaryText;
    for(char c : text){
        bitset<8> binary(c);
        binaryText += binary.to_string();
    }
    return binaryText;
}

int main() {

    string name;

    cout << "Please type your filename with the file extension:\n";
    getline(cin, name);


    cout << "press 1 to decode or 0 to encode\n";
    int choice;
    cin >> choice;

    if(choice == 0){
        encode(name);
    } else if(choice == 1){
        decode(name);
    }
    return 0;
}


void encode(const string& name){
        
        auto start = std::chrono::high_resolution_clock::now();

        ifstream inputFile(name, ios::binary);


        if (!inputFile.is_open()) {
            cout << "Error opening input file." << endl;
            return;
        }

        uintmax_t fileSize = filesystem::file_size(name);
        uintmax_t target = fileSize / 100;
        uintmax_t counter = 0;
        int loadIdx = 0;
        vector<char> characters;
        characters.reserve(fileSize*8);

        char buffer;
        string loader = "                                                                                                    ";
        while (inputFile.read(&buffer, sizeof(buffer))) {
            for (int i = 7; i >= 0; --i) {
                char bit = (buffer >> i) & 1;
                characters.push_back(bit + '0');
            }
            counter++;
            if (counter == target and loadIdx < 100) {
                counter = 0;
                std::cout << "Converting to binary: [" << loader << "]" << loadIdx << '%' << '\r' << std::flush;
                loader[loadIdx] = '#';
                loadIdx++;
            }
        }

        cout << "Converting to binary: " << "[####################################################################################################]100%" << endl;

        inputFile.close();

        uintmax_t idx = 0;
        int numImages = 0;
        string imageName;
        counter = 0;
        loader = "                                                                                                    ";
        target = characters.size() / 100;
        loadIdx = 0;
        bool done = false;
        bool exitLoop = false;
        uintmax_t vecSize = characters.size()-1;

        string binaryExtension = getExtension(name);
        binaryExtension = textToBinary(binaryExtension);
        

        for(char c : binaryExtension) characters.push_back(c);

        const char* charPtr = characters.data();
        size_t repetitions = ((characters.size()*16)/(640*360)) + 1;


        for(int i = 0; i < repetitions; i++){
            Mat image(360, 640, CV_8UC3, Scalar(0, 0, 0));
            for(int j = 0; j < 360 and !exitLoop; j = j+4){
                for(int k = 0; k < 640; k = k+4){
                    counter++;
                    if(counter == target and loadIdx < 100){
                        counter = 0;
                        cout << "Converting binary to images: [" << loader << "]" << loadIdx << '%' << '\r' << flush;
                        loader[loadIdx] = '#';
                        loadIdx++;
                    }
                    if(idx == vecSize){
                        image(Rect(k,j,4,4)) = Vec3b(0, 0, 255);
                        vecSize = characters.size();
                        if(done){
                            exitLoop = true;
                            break;
                        }
                        done = true;
                        idx++;
                        continue;
                    }
                    else if(charPtr[idx] == '1'){
                        image(Rect(k,j,4,4)) = Vec3b(255, 255, 255);
                    }
                    idx++;
                }
            }
            imageName = "image" + to_string(numImages) + ".png";
            numImages++;
            imwrite("outputImages/" + imageName, image);
        }
        cout << "Converting binary to images: " << "[####################################################################################################]100%" << endl;


        int original_stderr = dup(fileno(stderr));
        freopen("/dev/null", "w", stderr);
        VideoWriter video("outputVideo.avi", VideoWriter::fourcc('F', 'F', 'V', '1'), 24.0, Size(640, 360));
        dup2(original_stderr, fileno(stderr));
        close(original_stderr);


        counter = 0;
        loader = "                                                                                                    ";
        target = numImages / 100;
        loadIdx = 0;
        
        for(int i = 0; i < numImages; i++){
            counter++;
            if(counter == target and loadIdx < 100){
                counter = 0;
                cout << "Generating video: [" << loader << "]" << loadIdx << '%' << '\r' << flush;
                loader[loadIdx] = '#';
                loadIdx++;
            }
            string filename = "outputImages/image" + to_string(i) + ".png";
            Mat frame = imread(filename);
            video.write(frame);
        }
        cout << "Generating video: " << "[####################################################################################################]100%" << endl;

        video.release();


    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start);
    cout << "Elapsed time: " << duration.count() << " seconds." << endl;
        
}


void decode(const string& name){
        auto start = std::chrono::high_resolution_clock::now();

        VideoCapture decodeVid(name);
        if(!decodeVid.open(name)){
            cout << "Error opening encoded video" << endl;
            return;
        }

        int frameNum = 0;
        Vec3b pixelValue;
        ofstream outputFile4("binary_data2.txt");
        char one = '1';
        char zero = '0';
        bool seenRed = false;
        bool done = false;
        bool exitLoop = false;
        string frameName;
        Mat frameImage;
        int valSum;
        string extension = "";

        while(!exitLoop){
            Mat frame;
            decodeVid >> frame;    //Pull a frame from video
            //if(frame.empty()) break;

            for(int j = 0; j < 360 and !exitLoop; j = j+4){     //Pixel rows
                for(int k = 0; k < 640; k = k+4){               //Pixel columns
                    valSum = 0;
                    for(int x = 0; x < 4; x++){                 //Each bit is represented by 4x4 pixel matrix.
                        for(int y = 0; y < 4; y++){             //Combine pixels' BGR value  
                            pixelValue = frame.at<Vec3b>(j+x,k+y);
                            valSum    += static_cast<int>(pixelValue[0]) + static_cast<int>(pixelValue[1]) + static_cast<int>(pixelValue[2]);
                        }
                    }                                            //If we've seen red pixels, that means we are now looking at
                    if(seenRed){                                 //bits that represent the original file's extension so we know what to save it as
                        if(valSum < 2040) extension += zero;     //Black  
                        else if(valSum > 6160) extension += one; //White
                        else{                                    //Second red pixel means we're done.   
                            exitLoop = true;
                            break;
                        }
                    }                                                           //Haven't seen red pixels yet, so this is just the original file's data
                    else if(valSum < 2040 and !seenRed) outputFile4 << zero;    //Black pixels
                    else if(valSum > 6160 and !seenRed) outputFile4 << one;     //White pixels
                    else seenRed = true;                                        //Red pixels means next data we see is file extension
                }
            } 
        }
        decodeVid.release();

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end - start);
        cout << "Elapsed time: " << duration.count() << " seconds." << endl;

        outputFile4.close();
        string exportName = "DecodedFile";                  
        for(int i = 0; i < extension.length(); i+= 8){          //Translate the binary extension into plain text
            bitset<8> binary(extension.substr(i,8));
            exportName += static_cast<char>(binary.to_ulong());
        }

        ifstream inputFile4("binary_data2.txt");
        ofstream outputFile5(exportName, ios::binary);

        stringstream buffer3;
        buffer3 << inputFile4.rdbuf();

        string binaryString = buffer3.str();
        for (size_t i = 0; i < binaryString.length(); i += 8) {
            bitset<8> binaryByte(binaryString.substr(i, 8));
            outputFile5.put(static_cast<char>(binaryByte.to_ulong()));
        }

        inputFile4.close();
        outputFile5.close();

}