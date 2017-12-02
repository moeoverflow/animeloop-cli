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
    const std::string kVersion = "2.0.3";
    const std::string kOutputVersion = "2.0.0";

    class LoopVideo {
    public:
        // infomation
        std::string filename;
        std::string title;
        
        // I/O path
        boost::filesystem::path input_filepath;
        boost::filesystem::path output_path;
        boost::filesystem::path loops_dirpath;
        boost::filesystem::path caches_dirpath;
        boost::filesystem::path phash_filepath;
        boost::filesystem::path cuts_filepath;
        boost::filesystem::path resized_video_filepath;
        std::string output_type = "mp4";

        // settings
        double min_duration = 0.6;
        double max_duration = 6.0;
        int resize_length = 64;
        int hash_length = 64;
        int phash_dct_length = 16;
        bool cover_enabled = false;

        unsigned int threads = 1;

        // Data
        VideoInfo info;
        HashVector phash_strings;
        LoopDurations durations;
        LoopDurations filtered_durations;
        FrameVector resized_frames;
        CutVector cuts;
        
        /**
         Class LoopVideo constructor.

         @param input input file path
         @param output output file path
         */
        LoopVideo(std::string input, std::string output_path);

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
        bool ffmpeg_available = false;
    };
}

#endif /* loop_video_hpp */
