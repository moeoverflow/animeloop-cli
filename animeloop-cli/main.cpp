 //
//  main.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/9.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include <iostream>
#include <boost/filesystem.hpp>

#include "cxxopts.hpp"
#include "loop_video.hpp"

const std::string kVersion = "1.1.0";

int main(int argc, char * argv[]) {

    auto rpath = boost::filesystem::path(argv[0]).parent_path();
    double min_duration, max_duration;
    
    try {
        cxxopts::Options options("animeloop", "anime loop video generator.");

        std::string input, output, title;
        
        options.add_options()
        ("h,help", "Show animeloop help")
        ("v,version", "Show animeloop version")
        ("title", "Title of the source video", cxxopts::value<std::string>(title))
        ("i,input", "Input video file path", cxxopts::value<std::string>(input))
        ("o,output", "Output video directory path", cxxopts::value<std::string>(output)->default_value(rpath.string()))
        ("min-duration", "Minimum duration (second) of loop video", cxxopts::value<double>(min_duration)->default_value("0.8"))
        ("max-duration", "Maximum duration (second) of loop video", cxxopts::value<double>(max_duration)->default_value("4.0"))
        ;
        
        options.parse(argc, argv);
        
        if (options.count("version")) {
            std::cout << "version: " << kVersion << std::endl;
        }
        
        if (options.count("help")) {
            std::cout << options.help() << std::endl;
        }
        
        if (options.count("input") && options.count("title")) {
            al::LoopVideo loop_video(title, input, output);
            loop_video.min_duration = min_duration;
            loop_video.max_duration = max_duration;
            
            auto durations = loop_video.find_loop_parts();
            
            std::cout << "Total count: " << durations.size() << std::endl;
            std::cout << "Loop parts of this video:" << std::endl;
            for (auto it = durations.begin(); it != durations.end(); ++it) {
                int start_frame = std::get<0>(*it);
                int end_frame = std::get<1>(*it);
                double fps = loop_video.fps;
                std::cout << al::time_string(start_frame / fps) << " ~ " << al::time_string(end_frame / fps) << std::endl;
            }
            
            loop_video.save_loop_parts(durations);
        }
        
        
    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    return 0;
}


