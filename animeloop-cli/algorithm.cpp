//
//  algorithm.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/14.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "algorithm.hpp"

#include <opencv2/objdetect/objdetect.hpp>
#include <vector>
#include <iomanip>
#include <numeric>
#include <string>

using namespace std;
using namespace cv;

void cvt_image(cv::Mat &image, int length) {
    if(3 == image.channels()) {
        cvtColor(image, image, CV_RGB2GRAY);
    }
    if (length != 0) {
        resize(image, image, Size(length, length));
    }
}

string al::aHash(Mat image, int length) {
    cvt_image(image, length);
    
    int sum = accumulate(image.begin<uchar>(), image.end<uchar>(), 0.00);
    int average = sum / image.cols / image.rows;
    
    std::string hash_string = "";
    
    for (auto it = image.begin<uchar>(); it != image.end<uchar>(); ++it) {
        hash_string += *it > average ? "1" : "0";
    }
    
    return hash_string;
}


string al::dHash(Mat image, int length) {
    cvt_image(image, length);
    
    string hash_string = "";
    for (int row = 0; row < length; ++row) {
        int row_start_index = row * length;
        for (int col = 0; col < length-1; ++col) {
            int col_left_index = row_start_index + col;
            bool diff = image.at<uchar>(col_left_index) > image.at<uchar>(col_left_index+1);
            hash_string += diff ? "1" : "0";
        }
    }
    return hash_string;
}

string al::pHash(Mat image, int length, int dct_length) {
    cvt_image(image, length);

    Mat dct = Mat_<double>(image);
    cv::dct(dct, dct);
    
    string hash_string = "";

    double mean = 0.0;
    for (int i = 0; i < dct_length; ++i)
    {
        for (int j = 0; j < dct_length; ++j)
        {
            mean += dct.at<double>(i, j);
        }
    }
    mean /= length * length;

    for (int i = 0; i < dct_length; ++i) {
        for (int j = 0; j < dct_length; ++j) {
            hash_string += (dct.at<double>(i, j) >= mean ? "1" : "0");
        }
    }
    return hash_string;
}


unsigned int al::hamming_distance(string str1, string str2) {
    if (str1.empty() || str2.empty()) { return 0; }
    
    unsigned long len1 = str1.length();
    unsigned long len2 = str2.length();
    
    if (len1 != len2) { return 0; }
    
    unsigned int dist = 0;
    for (int i = 0; i < len1; ++i) {
        dist += (str1.at(i) != str2.at(i)) ? 1 : 0;
    }
    return dist;
}

unsigned int al::hamming_distance(int64_t n1, int64_t n2) {
    int64_t difference;
    
    unsigned int count = 0;
    for (difference = n1 ^ n2; difference; difference >>= 1) {
        count += difference & 1;
    }
    return count;
}