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

int main() {

    string name;

    cout << "Please type your filename with the file extension:\n";
    getline(cin, name);
    string extension;
    bool pass = false;
    for(char c : name){
        if(c == '.') pass = true;
        if(pass) extension += c;
    }

    string binaryExtension;
    for(char c : extension){
        bitset<8> binary(c);
        binaryExtension += binary.to_string();
    }

    cout << "press 1 to decode or 0 to encode\n";
    int choice;
    cin >> choice;
    if(choice == 0){
        
        auto start = std::chrono::high_resolution_clock::now();

        ifstream inputFile(name, ios::binary);


        if (!inputFile.is_open()) {
            cout << "Error opening input file." << endl;
            return 1;
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
    if(choice == 1){
        VideoCapture decodeVid(name);

        if(!decodeVid.open(name)){
            cout << "Error opening encoded video" << endl;
            return -1;
        }

        int frameNum = 0;
        while(true){
            Mat frame;
            decodeVid >> frame;

            if(frame.empty()) break;

            string filename = "frames/frame" + to_string(frameNum) + ".png";
            imwrite(filename, frame);
            frameNum++;
        }
        decodeVid.release();



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
        extension = "";

        auto start = std::chrono::high_resolution_clock::now();

        for(int i = 0; i < frameNum; i++){
            frameName = "frames/frame" + to_string(i) + ".png";
            frameImage = imread(frameName);
            for(int j = 0; j < 360 and !exitLoop; j = j+4){
                for(int k = 0; k < 640; k = k+4){
                    valSum = 0;

                    for(int x = 0; x < 4; x++){
                        for(int y = 0; y < 4; y++){
                            pixelValue = frameImage.at<Vec3b>(j+x,k+y);
                            valSum    += static_cast<int>(pixelValue[0]) + static_cast<int>(pixelValue[1]) + static_cast<int>(pixelValue[2]);
                        }
                    }
                    
                    if(seenRed){
                        if(valSum < 2040) extension += zero;
                        else if(valSum > 6160) extension += one;
                        else{
                            exitLoop = true;
                            break;
                        }
                    }
                    else if(valSum < 2040 and !seenRed) outputFile4 << zero;
                    else if(valSum > 6160 and !seenRed) outputFile4 << one;
                    else seenRed = true;
                }
            } 
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end - start);
        cout << "Elapsed time: " << duration.count() << " seconds." << endl;

        outputFile4.close();
        name = "DecodedFile";
        for(int i = 0; i < extension.length(); i+= 8){
            bitset<8> binary(extension.substr(i,8));
            name += static_cast<char>(binary.to_ulong());
        }

        ifstream inputFile4("binary_data2.txt");
        ofstream outputFile5(name, ios::binary);

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
    return 0;
}
