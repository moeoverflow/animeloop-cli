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
#include "thread_pool.h"
#include "progress_bar.hpp"
#include "child_process.hpp"

#include <json/json.h>

using namespace std;
using namespace boost::filesystem;
using namespace cv;
using namespace al;


al::LoopVideo::LoopVideo(std::string input, std::string output_path) {
    this->filename = path(input).stem().string();
    this->title = this->filename;

    this->input_filepath = path(input);
    this->output_path = path(output_path);
    this->loops_dirpath = path(output_path).append("loops").append(this->filename);
    this->caches_dirpath = path(output_path).append("caches").append(this->filename);

    string phash_filename = this->filename + "_pHash.txt";
    this->phash_filepath = path(caches_dirpath).append(phash_filename);
    string resized_video_filename = this->filename + "_" + to_string(this->resize_length) + "x" + to_string(this->resize_length) + "." + this->output_type;
    this->resized_video_filepath = path(caches_dirpath).append(resized_video_filename);
    string cuts_filename = this->filename + "_cuts.txt";
    this->cuts_filepath = path(caches_dirpath).append(cuts_filename);

    if (!exists(this->output_path)) {
        create_directories(this->output_path);
    }
    if (!exists(this->caches_dirpath)) {
        create_directories(this->caches_dirpath);
    }
    if (!exists(this->loops_dirpath)) {
        create_directories(this->loops_dirpath);
    }
}

void al::LoopVideo::init() {
    cout << ":: init" << endl;
    resize_video(this->input_filepath, this->resized_video_filepath, cv::Size(this->resize_length, this->resize_length));
    get_frames(this->resized_video_filepath, this->resized_frames);
    get_hash_strings(this->resized_video_filepath, "pHash", this->hash_length, this->phash_dct_length, this->phash_strings, this->phash_filepath);
    get_cuts(this->resized_video_filepath, this->cuts, this->cuts_filepath);
    this->info = get_info(this->resized_video_filepath.string());
}

void al::LoopVideo::filter() {
    cout << ":: finding loops..." << endl;
    filter::all_loops(this, this->durations);
    LoopDurations filtered_durations(this->durations);

    filter::cut_in_loop(this, filtered_durations);
    filter::loop_nearby(this, filtered_durations);
    filter::loop_tiny_frame_change(this, filtered_durations);
    filter::loop_white_or_black(this, filtered_durations);
    filter::loop_same_color(this, filtered_durations);

    this->filtered_durations = filtered_durations;
    cout << "[o] done." << endl;

}

void al::LoopVideo::print(LoopDurations durations) {
    VideoInfo info = get_info(this->input_filepath);
    
    cout << ":: total " << durations.size() << " loops:" << endl;
    for (auto duration : durations) {
        long start_frame, end_frame;
        tie(start_frame, end_frame) = duration;
        cout << "[o] " << al::time_string(start_frame / info.fps) << " ~ " << al::time_string(end_frame / info.fps) << endl;
    }
}

void al::LoopVideo::generate(const LoopDurations durations) {
    VideoInfo info = get_info(this->input_filepath);

    Json::Value videos_json;
    videos_json["title"] = this->title;
    videos_json["version"] = kOutputVersion;
    Json::Value source_json;
    source_json["filename"] = this->filename;
    videos_json["source"].append(source_json);


    ThreadPool pool(threads);
    vector<future<void>> futures;

    ProgressBar progressBar1(durations.size()+1, 35);
    ++progressBar1;
    cout << ":: saving loop video files..." << endl;
    /*
     * Multi-threads support for generating result video files.
     * */
    for_each(durations.begin(), durations.end(), [&](const LoopDuration duration) {
        long start_frame, end_frame;
        tie(start_frame, end_frame) = duration;

        string base_filename = "frame_from_" + to_string(start_frame) + "_to_" + to_string(end_frame);
        auto video_filename =
                base_filename + "_" + to_string(info.size.width) + "x" + to_string(info.size.height) + "." +
                this->output_type;
        auto video_filepath = path(this->loops_dirpath).append(video_filename).string();
        auto cover_filename = base_filename + "_cover.jpg";
        auto cover_filepath = path(this->loops_dirpath).append(cover_filename).string();
        auto input_filename = this->input_filepath.string();

        if (!exists(video_filepath)) {
            futures.push_back(pool.enqueue([=, &progressBar1]() -> void {
                VideoCapture capture;
                capture.open(input_filename);
                capture.set(CV_CAP_PROP_POS_FRAMES, start_frame);
                auto fourcc = CV_FOURCC('a', 'v', 'c', '1');
                cv::VideoWriter writer(video_filepath, fourcc, info.fps, info.size);

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
                capture.release();

                ++progressBar1;
                progressBar1.display();
            }));
        } else {
            ++progressBar1;
            progressBar1.display();
        }
    });

    /*
     * Waiting for all thread jobs done.
     * */
    for_each(futures.begin(), futures.end(), [](future<void>& f) {
        f.get();
    });

    progressBar1.done();
    cout << "[o] done." << endl;

    ProgressBar progressBar2(durations.size()+1, 35);
    ++progressBar2;

    cout << ":: saving loop cover files..." << endl;
    /*
     * generate cover files and info json file.
     * */
    for_each(durations.begin(), durations.end(), [&](const LoopDuration duration) {
        long start_frame, end_frame;
        tie(start_frame, end_frame) = duration;
        auto video_duration = (end_frame - start_frame) / info.fps;

        string base_filename = "frame_from_" + to_string(start_frame) + "_to_" + to_string(end_frame);
        auto video_filename = base_filename + "_" + to_string(info.size.width) + "x" + to_string(info.size.height) + "." + this->output_type;
        auto video_filepath = path(this->loops_dirpath).append(video_filename).string();
        auto cover_filename = base_filename + "_cover.jpg";
        auto cover_filepath = path(this->loops_dirpath).append(cover_filename).string();
        auto input_filename = this->input_filepath.string();

        if (cover_enabled && !exists(cover_filepath)) {
            futures.push_back(pool.enqueue([=]() -> void {
                const string i = video_filepath;
                const string o = cover_filepath;
                const string cli = "ffmpeg -loglevel panic -i \"" + i + "\" -vframes 1 -f image2 \"" + o + "\"";
                child_process(cli);
            }));
        }

        ++progressBar2;
        progressBar2.display();

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

        videos_json["loops"].append(video_json);
    });

    progressBar2.done();
    cout << "[o] done." << endl;

    cout << ":: saving info json file..." << endl;
    string json_string = videos_json.toStyledString();
    std::ofstream out(path(this->loops_dirpath).append(this->filename + ".json").string());
    out << json_string;
    out.close();

    cout << "[o] done." << endl;
}
