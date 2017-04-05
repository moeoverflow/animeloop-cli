//
//  loop_video.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "loop_video.hpp"
#include "algorithm.hpp"
#include "utils.hpp"
#include "filter.hpp"

#include "json.h"
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <locale>
#include <numeric>
#include <string>

using namespace std;
using namespace boost::filesystem;
using namespace boost::uuids;
using namespace cv;
using namespace al;


al::LoopVideo::LoopVideo(std::string title, std::string input, std::string output) {

    this->md5 = al::md5_of_file(this->input_path.string());

    this->title = title;

    this->filename = path(input).stem().string();

    this->input_path = path(input);
    this->output_path = path(output).append("loops").append(this->filename);
    this->caches_path = path(output).append("caches").append(this->md5);
    
    this->dhash_filename = path(caches_path).append(this->md5 + "_dHash.txt");
    this->phash_filename = path(caches_path).append(this->md5 + "_pHash.txt");

    this->resized_video_filename = path(caches_path).append(this->md5 + "_resized." + this->output_type);

    if (!exists(this->output_path)) {
        create_directories(this->output_path);
    }
    if (!exists(this->caches_path)) {
        create_directories(this->caches_path);
    }
}

void al::LoopVideo::init() {
    resize_video(this->input_path.string(), this->resized_video_filename.string(), cv::Size(8, 8));
    get_hash_strings(this->resized_video_filename.string(), "dHash", this->dhash_strings, this->dhash_filename.string());
    get_hash_strings(this->resized_video_filename.string(), "pHash", this->phash_strings, this->phash_filename.string());
    this->info = get_info(this->resized_video_filename.string());

    filter_0(this, this->durations);
}


void al::LoopVideo::filter() {
    LoopDurations filtered_durations(this->durations);
    
    filter_1(this, filtered_durations);
    filter_2(this, filtered_durations);
    filter_3(this, filtered_durations);
    
    this->filtered_durations = filtered_durations;
}

void al::LoopVideo::print(LoopDurations durations) {
    VideoInfo info = get_info(this->input_path.string());
    
    cout << "Total count: " << durations.size() << endl;
    cout << "Loop parts of this video:" << endl;
    for (auto duration : durations) {
        long start_frame, end_frame;
        std::tie(start_frame, end_frame) = duration;
        
        std::cout << al::time_string(start_frame / info.fps) << " ~ " << al::time_string(end_frame / info.fps) << std::endl;
    }
}


void al::LoopVideo::generate(const LoopDurations durations) {
    VideoInfo info = get_info(this->input_path.string());
    
    Json::Value videos_json;
    videos_json["title"] = this->title;
    
    Json::Value source_json;
    source_json["filename"] = this->filename;
    source_json["md5"] = this->md5;
    videos_json["source"].append(source_json);
    
    int count = 0;
    for_each(durations.begin(), durations.end(), [&](const LoopDuration duration) {
        auto uuid = random_generator()();
        auto uuid_string = to_string(uuid);
        count++;
        string video_filename = uuid_string + "." + this->output_type;
        auto video_filepath = path(this->output_path).append(video_filename);
        string cover_filename = uuid_string + "_cover.jpg";
        auto cover_filepath = path(this->output_path).append(cover_filename);
        
        long start_frame, end_frame;
        tie(start_frame, end_frame) = duration;
        
        auto video_duration = (end_frame - start_frame) / info.fps;
        
        VideoCapture capture;
        capture.open(this->input_path.string());
        capture.set(CV_CAP_PROP_POS_FRAMES, start_frame);
        
        // Save video file.
        cv::VideoWriter writer(video_filepath.string(), info.fourcc, info.fps, info.size);
        
        cv::Mat image;
        long frame = start_frame;
        while (capture.read(image)) {
            if (cover_enabled && frame == start_frame) {
                cv::imwrite(cover_filepath.string(), image);
            } else if (frame > end_frame) {
                break;
            }
            
            writer.write(image);
            frame++;
        }
        writer.release();
        capture.release();
        
        // Save video info json file.
        Json::Value video_json;
        
        video_json["video_filename"] = video_filename;
        video_json["cover_filename"] = cover_filename;
        
        video_json["duration"] = video_duration;
        
        Json::Value frame_json;
        frame_json["start"] = to_string(start_frame);
        frame_json["end"] = to_string(end_frame);
        video_json["frame"] = frame_json;
        
        Json::Value time_json;
        time_json["start"] = al::time_string(start_frame / info.fps);
        time_json["end"] = al::time_string(end_frame / info.fps);
        video_json["time"] = time_json;
        
        videos_json["loops"].append(video_json);
    });
    
    string json_string = videos_json.toStyledString();
    std::ofstream out(path(this->output_path).append(this->filename + ".json").string());
    out << json_string;
    out.close();
}

