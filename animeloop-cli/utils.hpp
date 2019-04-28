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
#include <boost/filesystem/path.hpp>


namespace al {
    VideoInfo get_info(boost::filesystem::path filename);

    VideoInfo get_info(cv::VideoCapture &capture);

    /**
     Resize a video and write into a new file.

     @param file intput path of origin video file
     @param output output path of resized video file
     @param size resizing size
     */
    void resize_video(boost::filesystem::path input, boost::filesystem::path output, cv::Size size);

    bool get_frames(boost::filesystem::path file, FrameVector &frames);

    void get_hash_strings(boost::filesystem::path filename, std::string type, int length, int dct_length, al::HashVector &hash_strings,
                          boost::filesystem::path hash_file);

    void get_cuts(boost::filesystem::path resized_filepath, CutVector &cuts, boost::filesystem::path cuts_filepath);


    double get_variance(std::vector<int> distances);

    double get_mean(cv::Mat image);

    cv::Vec3b get_mean_rgb(cv::Mat image);



    /**
     Convert seconds value to a time string.

     @param seconds seconds value
     @return Time string
     */
    std::string time_string(double seconds);
}

bool detect_ffmpeg();

#endif /* utils_hpp */
