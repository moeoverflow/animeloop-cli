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
    
    /**
     Resize a video and write into a new file.

     @param file intput path of origin video file
     @param output output path of resized video file
     @param size resizing size
     */
    void resize_video(std::string file, std::string output, cv::Size size);

    bool get_frames(std::string file, FrameVector &frames);

    bool get_hash_strings(std::string file, std::string type, al::HashVector &hash_strings, std::string hash_file);
    
    double get_variance(std::vector<int> distances);
    double get_mean(cv::Mat image);
    
    
    /**
     Convert seconds value to a time string.

     @param seconds seconds value
     @return Time string
     */
    std::string time_string(double seconds);
    
    /**
     Generate the MD5 checksum of a file.

     @param filename file path
     @return md5 string value
     */
    std::string md5_of_file(std::string filename);
}

#endif /* utils_hpp */
