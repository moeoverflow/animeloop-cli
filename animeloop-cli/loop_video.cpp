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
#include <sys/wait.h>

using namespace std;
using namespace boost::filesystem;
using namespace cv;
using namespace al;


al::LoopVideo::LoopVideo(std::string series, std::string episode, std::string input, std::string output) {

    this->md5 = al::md5_of_file(input);

    this->series = series;
    this->episode = episode;

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
    resize_video(this->input_path, this->resized_video_filename, cv::Size(this->kResizedWidth, this->kResizedHeight));
    get_frames(this->resized_video_filename.string(), this->frames);
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

int fork_gen_cover(string video_filepath, string cover_filepath) {
    int pid = fork();
    if(pid != 0) {
        /* We're in the parent process, return the child's pid. */
        return pid;
    }
    /* Otherwise, we're in the child process, so let's exec curl. */
    execlp("ffmpeg", "ffmpeg", "-loglevel", "panic", "-i", video_filepath.c_str(), "-vframes", "1", "-f", "image2", cover_filepath.c_str(), NULL);

    exit(100);
}

void al::LoopVideo::generate(const LoopDurations durations) {
    VideoInfo info = get_info(this->input_path.string());
    
    Json::Value videos_json;
    videos_json["series"] = this->series;
    videos_json["episode"] = this->episode;

    videos_json["animeloop_ver"] = kVersion;
    
    Json::Value source_json;
    source_json["filename"] = this->filename;
    source_json["md5"] = this->md5;
    videos_json["source"].append(source_json);
    
    int count = 0;
    for_each(durations.begin(), durations.end(), [&](const LoopDuration duration) {
        count++;

        long start_frame, end_frame;
        tie(start_frame, end_frame) = duration;
        auto video_duration = (end_frame - start_frame) / info.fps;
        
        string base_filename = "frame_from_" + to_string(start_frame) + "_to_" + to_string(end_frame);
        auto video_filename = base_filename + "_" + to_string(info.size.width) + "x" + to_string(info.size.height) + "." + this->output_type;
        auto video_filepath = path(this->output_path).append(video_filename).string();
        auto cover_filename = base_filename + "_cover.jpg";
        auto cover_filepath = path(this->output_path).append(cover_filename).string();
        
        VideoCapture capture;
        capture.open(this->input_path.string());
        
        if (!exists(video_filepath)) {
            capture.set(CV_CAP_PROP_POS_FRAMES, start_frame);
            cv::VideoWriter writer(video_filepath, CV_FOURCC('H', '2', '6', '4'), info.fps, info.size);
            
            cv::Mat image;
            long frame = start_frame;
            while (capture.read(image)) {
                // Exclude the last frame.
                if (frame >= end_frame) {
                    break;
                }

                writer.write(image);
                frame++;
            }
            writer.release();
        }
        capture.release();

        if (cover_enabled && !exists(cover_filepath)) {
            int cpid = fork_gen_cover(video_filepath, cover_filepath);
            if(cpid == -1) {
                /* Failed to fork */
                cerr << "Fork failed" << endl;
                throw;
            }

            /* Optionally, wait for the child to exit and get
               the exit status. */
            int status;
            waitpid(cpid, &status, 0);
            if(! WIFEXITED(status)) {
                cerr << "The child was killed or segfaulted or something." << endl;
            }

            status = WEXITSTATUS(status);
        }

        // Save video info json file.
        Json::Value video_json;
        
        Json::Value files_json;
        files_json["mp4_1080p"] = video_filename;
        files_json["jpg_1080p"] = cover_filename;
        video_json["files"] = files_json;
        
        video_json["duration"] = video_duration;
        
        Json::Value frame_json;
        frame_json["begin"] = to_string(start_frame);
        frame_json["end"] = to_string(end_frame);
        video_json["frame"] = frame_json;
        
        Json::Value period_json;
        period_json["begin"] = al::time_string(start_frame / info.fps);
        period_json["end"] = al::time_string(end_frame / info.fps);
        video_json["period"] = period_json;
        
        auto md5 = al::md5_of_file(video_filepath);
        video_json["md5"] = md5;
        
        videos_json["loops"].append(video_json);
    });
    
    string json_string = videos_json.toStyledString();
    std::ofstream out(path(this->output_path).append(this->filename + ".json").string());
    out << json_string;
    out.close();
}

