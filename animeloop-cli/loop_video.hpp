//
//  loop_video.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef loop_video_hpp
#define loop_video_hpp

#include "models.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>


namespace al {        
    class LoopVideo {
    public:
        // infomation
        std::string filename;
        std::string title;
        
        // I/O path
        boost::filesystem::path input_path;
        boost::filesystem::path output_path;
        boost::filesystem::path caches_path;
        boost::filesystem::path dhash_filename;
        boost::filesystem::path phash_filename;
        boost::filesystem::path resized_video_filename;
        std::string output_type = "mp4";
        
        // Effect the final results.
        double kMinduration = 0.8;
        double kMaxduration = 4;
        int kResizedHeight = 32;
        int kResizedWidth = 32;
        double kVarience = 1.0;
        bool cover_enabled = false;

        VideoInfo info;
        HashVector dhash_strings;
        HashVector phash_strings;
        LoopDurations durations;
        LoopDurations filtered_durations;
        
        LoopVideo(std::string title, std::string input, std::string output);
        
        void init();
        void filter();
        void print(LoopDurations durations);
        
        void generate(const al::LoopDurations filtered_durations);

    private:
        std::string md5;
    };
}


#endif /* loop_video_hpp */
