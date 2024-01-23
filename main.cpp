#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    ifstream inputFile("input.jpg", ios::binary);

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


    ofstream outputFile2("output.jpg", ios::binary);


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

    cout << "Binary data converted to jpg: output.jpg" << endl;
    ifstream inputFile3("binary_data.txt");

    vector<char> characters;
    char ch;
    while(inputFile3.get(ch)) characters.push_back(ch);


    int idx = 0;
    string imageName = "image0.jpg";
    for(int i = 0; i < (characters.size()/(1920*1080))+1; i++){
        Mat image(1080, 1920, CV_8UC3, Scalar(0, 0, 0));
        for(int j = 0; j < 1080; j++){
            for(int k = 0; k < 1920; k++){
                if(idx == characters.size()){
                    image.at<Vec3b>(j,k) = Vec3b(255,0,0);
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
        imageName[5]++;
        imwrite(imageName, image);
    }
    
    inputFile3.close();
    return 0;
}
