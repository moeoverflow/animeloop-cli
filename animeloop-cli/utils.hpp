//
//  utils.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/4/3.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include "models.hpp"

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>


namespace al {
    VideoInfo get_info(std::string filename);
    VideoInfo get_info(cv::VideoCapture &capture);
    
    void resize_video(std::string file, std::string output, cv::Size size);

    bool get_hash_strings(std::string file, std::string type, al::HashVector &hash_strings, std::string hash_file);
    
    double get_variance(std::vector<int> distances);
    double get_mean(cv::Mat image);
    
    std::string time_string(double seconds);
    std::string md5_of_file(std::string filename);
}

#endif /* utils_hpp */
