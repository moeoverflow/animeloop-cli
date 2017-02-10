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
    typedef std::vector<LoopDuration> LoopDurationVector;
    typedef std::vector<std::string> HashVector;
    
    class LoopVideo {
    public:
        std::string name;
        std::string filename;
        std::string workpath;
        std::string hashpath;
        std::string outputpath;
        
        double minDuration;
        double maxDuration;
        
        bool is_cache_hash_string = true;
        
        LoopVideo(std::string id, std::string filename, std::string workpath);
        
        void find_loop_video();
        void write_loop_video_files();

        
    private:
        cv::VideoCapture capture;
        HashVector hash_strings;
        LoopDurationVector durations;
        
        double fps;
        double fourcc;
        cv::Size size;
        int frame_count;
        
        bool open_capture();
        void update_capture_prop();
//        void close_capture();
        
        void get_hash_strings();
        void analyse_loop_durations();
    };
}

#endif /* loop_video_hpp */
