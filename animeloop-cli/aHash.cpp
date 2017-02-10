//
//  aHash.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "aHash.hpp"
#include <numeric>

std::string aHash(cv::Mat image, int rows, int cols) {
    cv::Size size(cols, rows);
    cv::resize(image, image, size);
    cv::cvtColor(image, image, CV_RGB2GRAY);
    
    int sum = std::accumulate(image.begin<uchar>(), image.end<uchar>(), 0.00);
    int average = sum / image.cols / image.rows;
    
    std::string hash_string = "";
    
    for (auto it = image.begin<uchar>(); it != image.end<uchar>(); ++it) {
        hash_string += *it > average ? "1" : "0";
    }
    
    return hash_string;
}
