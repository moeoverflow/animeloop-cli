//
//  loop_video.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef loop_video_hpp
#define loop_video_hpp

#include <iostream>
#include <opencv2/opencv.hpp>

namespace al {    
    typedef std::tuple<int, int, double> LoopDuration;
    typedef std::vector<LoopDuration> LoopDurations;
    typedef std::vector<std::string> HashVector;
    
    class LoopVideo {
    public:
        std::string name;
        std::string input_path;
        std::string output_path;
        std::string hash_path;

        double min_duration;
        double max_duration;
        
        HashVector dHash_strings;
//        LoopDurations durations;
        
        LoopVideo(std::string input, std::string output);
        
        al::LoopDurations find_loop_parts();
        void save_loop_parts();
        
        cv::VideoCapture capture;
        
        double fps;
        double fourcc;
        cv::Size size;
        int frame_count;
        
        
        bool open_capture();
        void update_capture_prop();
        void close_capture();
        
        void save_loop_parts(al::LoopDurations durations);
    };
    
    bool open_capture();
    
    void update_capture_prop();
    
    //void close_capture();
    
    std::vector<std::string> get_hash_strings(al::LoopVideo *loop_video, std::string hash_type);

    std::vector<std::string> restore_hash_strings(std::string filepath);
    
    
    void save_hash_strings(std::string filepath, std::vector<std::string> hashs);
    
    std::string time_string(double seconds);
    
    double get_variance_of_distances(std::vector<int> distances);
    
    double get_mean_of_images(cv::Mat image);
}


#endif /* loop_video_hpp */
