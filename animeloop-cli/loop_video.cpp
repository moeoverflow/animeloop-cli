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
#include <boost/date_time.hpp>

#include "utils.hpp"
#include "algorithm.hpp"


al::LoopVideo::LoopVideo(std::string input, std::string output) {
    this->input_path = input;
    
    auto path = boost::filesystem::path(input);
    this->name = path.stem().string();
    
    this->output_path = boost::filesystem::path(output).append(this->name).string();
    
    this->min_duration = 0.8;
    this->max_duration = 4;

    
    if (!boost::filesystem::exists(this->output_path)) {
        boost::filesystem::create_directories(this->output_path);
    }
}

al::LoopDurations al::LoopVideo::find_loop_parts() {
    this->dHash_strings = al::get_hash_strings(this, "dHash");
    
    // Find all possile loop video duration.
    std::cout << "Finding possile loop parts..." << std::endl;
    al::LoopDurations possible_durations;
    std::vector<double> variances;
    
    int min_duration_frame = this->min_duration * this->fps;
    int max_duration_frame = this->max_duration * this->fps;
    
    for (auto it = this->dHash_strings.begin(); it != (this->dHash_strings.end() - min_duration_frame); ++it) {
        
        long begin = std::distance(this->dHash_strings.begin(), it);
        long end = -1;
        
        std::vector<int> distances;
        for (int i = min_duration_frame; i < max_duration_frame; ++i) {
            if (it+i+1 == this->dHash_strings.end()) {
                break;
            }
            
            int distance = hamming_distance(*it, *(it+i));
            distances.push_back(distance);
            
            if (distance == 0) {
                end = begin+i;
            }
        }
        
        if (end == -1) {
            continue;
        }
        
        std::vector<int> nearby_distances;
        
        for (int i = 0; i < end - begin - 1; ++i) {
            int distance = hamming_distance(*(it+i), *(it+i+1));
            nearby_distances.push_back(distance);
        }
        
        double variance = al::get_variance_of_distances(nearby_distances);
        
        if (variance > 1.0) {
            variances.push_back(variance);
            possible_durations.push_back(std::make_tuple(begin, end, variance));
        }
    }

    // filter loop video duration
    std::cout << "Filtering possile loop parts..." << std::endl;
    al::LoopDurations durations;
    
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
        this->capture.release();
        
        double mean1 = al::get_mean_of_images(begin_image);
        if (mean1 < 0.001) {
            continue;
        }
        double mean2 = al::get_mean_of_images(end_image);
        if (mean2 < 0.001) {
            continue;
        }
        
        std::string begin_hash = pHash(begin_image, 8);
        std::string end_hash = pHash(end_image, 8);
        int distance = hamming_distance(begin_hash, end_hash);
        if (distance > 0) {
            continue;
        }
        
        durations.push_back(*it);
    }
    return durations;
}


/**
 Write loop videos file.
 */
void al::LoopVideo::save_loop_parts(al::LoopDurations durations) {
    auto path = boost::filesystem::path(output_path);
    if (!boost::filesystem::exists(path)) {
        boost::filesystem::create_directory(path);
    }
    
    std::for_each(durations.begin(), durations.end(), [&](const LoopDuration duration) {
        int start_frame = std::get<0>(duration);
        int end_frame = std::get<1>(duration);
        
        this->open_capture();
        this->capture.set(CV_CAP_PROP_POS_FRAMES, std::get<0>(duration));
        
        std::string filename = this->output_path + "/" + this->name + "_loop_from_" + time_string(start_frame / this->fps) + "_to_" + time_string(end_frame/this->fps) + ".mp4";
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

void al::LoopVideo::close_capture() {
    this->capture.release();
}



std::vector<std::string> al::get_hash_strings(al::LoopVideo *loop_video, std::string hash_type) {
    std::cout << "Calculating hash value..." << std::endl;

    std::string filename = boost::filesystem::path(loop_video->output_path).append(loop_video->name + "_" + hash_type + ".txt").string();
    
    // Read video frames hash string if data file exists.
    al::HashVector hashs = al::restore_hash_strings(filename);
    bool hash_from_file = !hashs.empty();
    
    // Calculate hash string per frame when data file does not exist.
    loop_video->open_capture();
    if (!hash_from_file) {
        cv::Mat image;
        while (loop_video->capture.read(image)) {
            std::string hash = al::hash(hash_type, image, 8);
            hashs.push_back(hash);
            image.release();
        }
        // Write video frames hash string when data file does not exist.
        al::save_hash_strings(filename, hashs);
    }
    loop_video->close_capture();
    
    return hashs;
}

std::vector<std::string> al::restore_hash_strings(std::string filepath) {
    std::cout << "Restore hash value from file..." << std::endl;

    std::ifstream input_file(filepath);
    std::vector<std::string> hashs;
    
    int count;
    count = 0;
    while (input_file) {
        std::string line;
        std::getline(input_file, line);
        hashs.push_back(line);
    }
    input_file.close();
    
    return hashs;
}

void al::save_hash_strings(std::string filepath, std::vector<std::string> hashs) {
    std::cout << "Save hash value into file..." << std::endl;
    std::ofstream output_file(filepath);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(hashs.begin(), hashs.end(), output_iterator);
    output_file.close();
}

double al::get_variance_of_distances(std::vector<int> distances) {
    double sum = std::accumulate(distances.begin(), distances.end(), 0.0);
    double average =  sum / distances.size();
    double accum  = 0.0;
    std::for_each(distances.begin(), distances.end(), [&](const double distance) {
        accum += (distance-average) * (distance-average);
    });
    double variance = sqrt(accum / (distances.size() - 1));
    return variance;
}

double al::get_mean_of_images(cv::Mat image) {
    int64 sum = std::accumulate(image.begin<uchar>(), image.end<uchar>(), 0);
    double mean = sum / double(image.cols * image.rows);
    return mean;
}

std::string al::time_string(double seconds) {
    auto sec = boost::posix_time::seconds(seconds);
    auto time = boost::posix_time::time_duration(sec);
    auto time_string = boost::posix_time::to_simple_string(time);
    return time_string;
}
