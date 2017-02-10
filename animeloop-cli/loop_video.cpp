//
//  loop_video.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "loop_video.hpp"
#include <numeric>
#include <boost/filesystem.hpp>

#include "utils.hpp"
#include "dHash.hpp"
#include "aHash.hpp"

al::LoopVideo::LoopVideo(std::string input, std::string output) {
    this->input_path = input;
    
    auto path = boost::filesystem::path(input);
    this->name = path.stem().string();
    
    this->output_path = boost::filesystem::path(output).append(this->name).string();
    
    this->min_duration = 0.8;
    this->max_duration = 4;

    
    if (!boost::filesystem::exists(this->output_path)) {
        boost::filesystem::create_directory(this->output_path);
    }
}

void al::LoopVideo::find_loop_video() {
    
    // Get hash strings.
    this->get_hash_strings();
    this->analyse_loop_durations();
}

bool al::LoopVideo::open_capture() {
    bool flag = this->capture.open(this->input_path);
    this->update_capture_prop();
    return flag;
}

void al::LoopVideo::update_capture_prop() {
    this->fps = this->capture.get(CV_CAP_PROP_FPS);
    this->fourcc = this->capture.get(CV_CAP_PROP_FOURCC);
    auto height = this->capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    auto width = this->capture.get(CV_CAP_PROP_FRAME_WIDTH);
    this->size = cv::Size(width, height);
    this->frame_count = this->capture.get(CV_CAP_PROP_FRAME_COUNT);
}

//void al::LoopVideo::close_capture() {
//    this->capture.release();
//}


/**
 get hash strings.
 */
void al::LoopVideo::get_hash_strings() {
    std::string filename = boost::filesystem::path(output_path).append(this->name + "_dhash.txt").string();
    
    // Read video frames hash string if data file exists.
    HashVector hashs = read_vector_of_string_from_file(filename);
    bool hash_from_file = !hashs.empty();
    
    // Calculate hash string per frame when data file does not exist.
    if (!hash_from_file) {
        int frame_count = 0;
        cv::Mat image;
        this->open_capture();
        while (capture.read(image)) {
            std::cout << "Calculating hash: " << ++frame_count << "/" << this->frame_count << std::endl;
            // Magic number - rows, cols
            std::string hash = dHash(image, 8, 9);
            hashs.push_back(hash);
            
            image.release();
        }
        this->capture.release();
        
        // Write video frames hash string when data file does not exist.
        write_vector_of_string_to_file(filename, hashs);
    } else {
        this->open_capture();
        this->update_capture_prop();
    }
    
    this->hash_strings = hashs;
}

void al::LoopVideo::analyse_loop_durations() {
    // Find all possile loop video duration.
    al::LoopDurationVector possible_durations;
    std::vector<double> variances;
    for (auto it = this->hash_strings.begin(); it != (this->hash_strings.end() - this->min_duration * this->fps); ++it) {
        long start = std::distance(this->hash_strings.begin(), it);
        long end = -1;
        
        std::vector<int> distances;
        std::vector<int> nearbyDistances;
        
        for (int i = this->min_duration * this->fps; i < this->max_duration * this->fps; ++i) {
            if (it+i+1 == this->hash_strings.end()) {
                break;
            }
            
            if (i < this->min_duration * this->fps) {
                continue;
            }
            
            int distance = hamming_distance(*it, *(it+i));
            distances.push_back(distance);
            
            if (distance == 0) {
                end = start+i;
//                if there are break, result will get min loop video,
//                or it will get max loop one.
                
//                break;
            }
        }
        
        if (end == -1) {
            continue;
        }
        
        for (int i = 0; i < end - start - 1; ++i) {
            int distance = hamming_distance(*(it+i), *(it+i+1));
            nearbyDistances.push_back(distance);
        }
        
        double sum = std::accumulate(nearbyDistances.begin(), nearbyDistances.end(), 0.0);
        double average =  sum / nearbyDistances.size();
        
        double accum  = 0.0;
        std::for_each(nearbyDistances.begin(), nearbyDistances.end(), [&](const double distance) {
            accum += (distance-average) * (distance-average);
        });
        
        double variance = sqrt(accum / (nearbyDistances.size() - 1));
        
        std::cout << "variance: " << variance << "\n";
        
        
        if (!isinf(variance) && !isnan(variance) && variance > 0.5) {
            variances.push_back(variance);
            possible_durations.push_back(std::make_tuple(start, end, variance));
//            std::cout << "loop: " << start / this->fps << "s-" << end / this->fps << "s " << start << "\n";
        }
        
    }
    
    for (auto it = possible_durations.begin(); it != possible_durations.end(); ++it) {
        std::cout << "loop: " << std::get<0>(*it) / this->fps << "s-" << std::get<1>(*it) / this->fps << "s " << std::get<0>(*it) << "\n";
    }
    
    
    // filter loop video duration
    al::LoopDurationVector durations;
    
    durations.push_back(*possible_durations.begin());
    for (auto it = possible_durations.begin() + 1; it != possible_durations.end(); ++it) {
        int start_frame = std::get<0>(*it);
        int end_frame = std::get<1>(*it);
        int prev_start_frame = std::get<0>(*(it-1));

        
        if (start_frame - prev_start_frame < this->min_duration * this->fps) {
            continue;
        }
        
        
        // Ensure that the start and end frame are consistent.
        this->open_capture();
        
        capture.set(CV_CAP_PROP_POS_FRAMES, start_frame);
        cv::Mat begin_image;
        capture.read(begin_image);
        
        capture.set(CV_CAP_PROP_POS_FRAMES, end_frame);
        cv::Mat end_image;
        capture.read(end_image);
        
        // Magic number: rows, cols
        std::string begin_hash = aHash(begin_image, 16, 16);
        std::string end_hash = aHash(end_image, 16, 16);
        
        this->capture.release();
        
        int dist = hamming_distance(begin_hash, end_hash);
        std::cout << std::get<0>(*it) << " dist: " << dist << "\n";
        if (dist > 1) {
            continue;
        }
        
        durations.push_back(*it);
    }
    this->durations = durations;
}

/**
 Write loop videos file.
 */
void al::LoopVideo::write_loop_video_files() {
    auto path = boost::filesystem::path(output_path);
    if (!boost::filesystem::exists(path)) {
        boost::filesystem::create_directory(path);
    }
    
    std::for_each(this->durations.begin(), this->durations.end(), [&](const LoopDuration duration) {
        int start_frame = std::get<0>(duration);
        int end_frame = std::get<1>(duration);
        
        this->open_capture();
        this->capture.set(CV_CAP_PROP_POS_FRAMES, std::get<0>(duration));
        
        std::string filename = this->output_path + "/" + this->name + "_loop_from_" + convert_seconds_to_time(start_frame / this->fps) + "_to_" + convert_seconds_to_time(end_frame/this->fps) + ".mp4";
        cv::VideoWriter writer(filename, this->fourcc, this->fps, this->size);
        
        cv::Mat image;
        int frame = start_frame;
        while (capture.read(image)) {
            if (frame > end_frame) {
                break;
            }
            writer.write(image);
            frame++;
        }
        capture.release();
    });
}
