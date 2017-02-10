//
//  dHash.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/9.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "dHash.hpp"

#include <vector>
#include <iomanip>
#include <bitset>

std::string dHash(cv::Mat image, int rows, int cols) {
    cv::Size size(cols, rows);
    cv::resize(image, image, size);
    cv::cvtColor(image, image, CV_RGB2GRAY);
    
    std::string difference = "";
    for (int row = 0; row < rows; ++row) {
        int row_start_index = row * cols;
        for (int col = 0; col < cols-1; ++col) {
            int col_left_index = row_start_index + col;
            bool diff = image.at<uchar>(col_left_index) > image.at<uchar>(col_left_index+1);
            difference += diff ? "1" : "0";
        }
    }
    return difference;
}
