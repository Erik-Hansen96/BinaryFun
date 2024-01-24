#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    string name;
   // cout << "press 1 to decode or 0 to encode\n";
   // int choice;
    //cin >> choice;
   // if(choice == 0){
        cout << "Please type your filename with the file extension:\n";
        cin >> name;

        ifstream inputFile(name, ios::binary);

        ofstream outputFile("binary_data.txt", ios::binary);

        if (!inputFile.is_open()) {
            cout << "Error opening input file." << endl;
            return 1;
        }
        if (!outputFile.is_open()) {
            cout << "Error opening output file." << endl;
            return 1;
        }

        char buffer;
        while (inputFile.get(buffer)) {
            bitset<8> binary(buffer);
            outputFile << binary;
        }


        inputFile.close();
        outputFile.close();
        

        cout << "File content saved in binary_data.txt." << endl;





        ifstream inputFile3("binary_data.txt");

        vector<char> characters;
        char ch;
        while(inputFile3.get(ch)) characters.push_back(ch);


        int idx = 0;
        int numImages = 0;
        string imageName;
        for(int i = 0; i < (characters.size()/(1280*720))+1; i++){
            Mat image(720, 1280, CV_8UC3, Scalar(0, 0, 0));
            for(int j = 0; j < 720; j++){
                for(int k = 0; k < 1280; k++){
                    if(idx == characters.size()){
                        image.at<Vec3b>(j,k) = Vec3b(0,0,255);
                        goto save;
                    }
                    else if(characters.at(idx) == '0'){
                        image.at<Vec3b>(j,k) = Vec3b(0,0,0);
                    }
                    else if(characters.at(idx) == '1'){
                        image.at<Vec3b>(j,k) = Vec3b(255,255,255);
                    }
                    idx++;
                }
            }
        save:
            imageName = "image" + to_string(numImages) + ".png";
            numImages++;
            imwrite("outputImages/" + imageName, image);
        }
        
        inputFile3.close();


        VideoWriter video("outputVideo.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 24, Size(1280, 720));

            if (!video.isOpened()) {
            cout << "Error: Could not open the video writer." << endl;
            return -1;
        }

        for(int i = 0; i < numImages; i++){
            string filename = "outputImages/image" + to_string(i) + ".png";
            Mat frame = imread(filename);
            video.write(frame);
        }
        video.release();
        

    //if(choice == 1){
        VideoCapture decodeVid("outputVideo.avi");

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
        bool done = false;
        string frameName;
        Mat frameImage;
        int valSum;

        for(int i = 0; i < frameNum; i++){
            frameName = "frames/frame" + to_string(i) + ".png";
            frameImage = imread(frameName);

            for(int j = 0; j < 720; j++){
                for(int k = 0; k < 1280; k++){
                    pixelValue = frameImage.at<Vec3b>(j,k);
                    valSum = static_cast<int>(pixelValue[0]) + static_cast<int>(pixelValue[1]) + static_cast<int>(pixelValue[2]);
                    if(valSum < 45) outputFile4 << zero;
                    else if(valSum > 720) outputFile4 << one;
                    else{ 
                        done = true; 
                        break;
                    }
                }
                if(done) break;
            }
            
            if(done) break;
        }
        outputFile4.close();

        ifstream inputFile4("binary_data2.txt");
        ofstream outputFile5("decoded" + name, ios::binary);

        stringstream buffer3;
        buffer3 << inputFile4.rdbuf();

        string binaryString = buffer3.str();
        for (size_t i = 0; i < binaryString.length(); i += 8) {
            bitset<8> binaryByte(binaryString.substr(i, 8));
            outputFile5.put(static_cast<char>(binaryByte.to_ulong()));
        }

        inputFile4.close();
        outputFile5.close();

   // }
    return 0;
}
