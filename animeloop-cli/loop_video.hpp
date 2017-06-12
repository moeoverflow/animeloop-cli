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
    const std::string kVersion = "1.3.1";

    class LoopVideo {
    public:
        // infomation
        std::string filename;
        std::string series;
        std::string episode;
        
        // I/O path
        boost::filesystem::path input_path;
        boost::filesystem::path output_path;
        boost::filesystem::path caches_path;
        boost::filesystem::path dhash_filename;
        boost::filesystem::path phash_filename;
        boost::filesystem::path resized_video_filename;
        std::string output_type = "mp4";
//        boost::filesystem::path face_cascade_filename;
        
        // Effect the final results.
        double kMinduration = 0.6;
        double kMaxduration = 6;
        int kResizedHeight = 32;
        int kResizedWidth = 32;
        double kVarience = 1.0;
        bool cover_enabled = false;

        VideoInfo info;
        HashVector dhash_strings;
        HashVector phash_strings;
        LoopDurations durations;
        LoopDurations filtered_durations;
        FrameVector frames;
        
        /**
         Class LoopVideo constructor.
         
         @param series series title
         @param episode episode title
         @param input input file path
         @param output output file path
         */
        LoopVideo(std::string series, std::string episode, std::string input, std::string output);
//        cv::CascadeClassifier face_cascade;
        
        /**
         Initial resized video file, pHash and dHash file for cache.
         */
        void init();
        
        /**
         Animeloop algorithm filter workflow.
         */
        void filter();
        
        /**
         Print durations data.
         
         @param durations durations data
         */
        void print(LoopDurations durations);
        
        /**
         Generate loop video files from durations data.
         
         @param durations durations data
         */
        void generate(const al::LoopDurations filtered_durations);

    private:
        std::string md5;
    };
}


#endif /* loop_video_hpp */
