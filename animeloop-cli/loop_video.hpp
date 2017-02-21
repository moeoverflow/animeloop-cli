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
#include <boost/filesystem.hpp>


namespace al {    
    typedef std::tuple<int, int, double> LoopDuration;
    typedef std::vector<LoopDuration> LoopDurations;
    typedef std::vector<std::string> HashVector;
    
    class LoopVideo {
    public:
        std::string filename;
        std::string title;
        boost::filesystem::path input_path;
        boost::filesystem::path output_path;
        boost::filesystem::path caches_path;
        std::string output_type = "mp4";
        std::string dHash_file_path;
        
        double min_duration = 0.8;
        double max_duration = 4;
        
        HashVector dHash_strings;
        
        LoopVideo(std::string title, std::string input, std::string output);
        
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
        
        std::vector<std::string> get_hash_strings(std::string hash_type);
        void save_loop_parts(al::LoopDurations durations);
    private:
        std::string md5;
    };
    
    
    
    std::vector<std::string> restore_hash_strings(std::string filepath);
    void save_hash_strings(std::string filepath, std::vector<std::string> hashs);
    
    std::string time_string(double seconds);
    
    double get_variance_of_distances(std::vector<int> distances);
    
    double get_mean_of_images(cv::Mat image);
    
    std::string md5_of_file(std::string filename);
}


#endif /* loop_video_hpp */
