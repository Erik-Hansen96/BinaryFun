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
    cout << "Please type your filename with the file extension:\n";
    cin >> name;

    ifstream inputFile(name, ios::binary);

    ofstream outputFile("binary_data.txt", ios::binary);

    if (!inputFile.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1;
    }
    if (!outputFile.is_open()) {
        cerr << "Error opening output file." << endl;
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


    ifstream inputFile2("binary_data.txt");


    ofstream outputFile2("After" + name, ios::binary);


    if (!inputFile2.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1;
    }
    if (!outputFile2.is_open()) {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    stringstream buffer2;
    buffer2 << inputFile2.rdbuf();

    string binaryString = buffer2.str();
    for (size_t i = 0; i < binaryString.length(); i += 8) {
        bitset<8> binaryByte(binaryString.substr(i, 8));
        outputFile2.put(static_cast<char>(binaryByte.to_ulong()));
    }

    inputFile2.close();
    outputFile2.close();

    cout << "Binary data converted to png: output.png" << endl;
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
    Vec3b pixelValue;
    /*
    Mat outputImage = imread("outputImages/image1.png");

    for(int i = 0; i < 17; i++){
        pixelValue = outputImage.at<Vec3b>(0,i);
        for(int j = 0; j < 3; j++){
            cout << static_cast<int>(pixelValue[j]) << " ";
        }
        cout << endl;
    }
*/


    VideoWriter video("outputVideo.avi", VideoWriter::fourcc('H', '2', '6', '4'), 30, Size(1280, 720));

    for(int i = 0; i < numImages; i++){
        cout << i << endl;
        string filename = "outputImages/image" + to_string(i) + ".png";
        Mat frame = imread(filename);
        video.write(frame);
    }
    video.release();
cout << 1234 << endl;
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

    return 0;
}
