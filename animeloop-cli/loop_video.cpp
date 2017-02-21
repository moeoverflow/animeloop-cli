//
//  loop_video.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "loop_video.hpp"
#include "algorithm.hpp"
#include "json.h"

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <locale>
#include <fstream>
#include <numeric>
#include <string>
#include <openssl/md5.h>


al::LoopVideo::LoopVideo(std::string title, std::string input, std::string output) {
    this->title = title;

    this->filename = boost::filesystem::path(input).stem().string();
    this->input_path = boost::filesystem::path(input);
    this->output_path = boost::filesystem::path(output).append("loops").append(this->filename);
    this->caches_path = boost::filesystem::path(output).append("caches").append(this->filename);

    this->md5 = al::md5_of_file(this->input_path.string());
    
    if (!boost::filesystem::exists(this->output_path)) {
        boost::filesystem::create_directories(this->output_path);
    }
    if (!boost::filesystem::exists(this->caches_path)) {
        boost::filesystem::create_directories(this->caches_path);
    }
}

al::LoopDurations al::LoopVideo::find_loop_parts() {
    this->dHash_strings = this->get_hash_strings("dHash");
    
    // Find all possile loop video duration.
    std::cout << "Finding possile loop parts..." << std::endl;
    al::LoopDurations possible_durations;
    std::vector<double> variances;
    
    int min_duration_frame = this->min_duration * this->fps;
    int max_duration_frame = this->max_duration * this->fps;
    
    for (auto it = this->dHash_strings.begin(); it != (this->dHash_strings.end() - min_duration_frame); ++it) {
        
        long begin = std::distance(this->dHash_strings.begin(), it);
        long end;
        std::vector<long> ends;
        
        std::vector<int> distances;
        for (int i = min_duration_frame; i < max_duration_frame; ++i) {
            if (it+i+1 == this->dHash_strings.end()) {
                break;
            }
            
            int distance = hamming_distance(*it, *(it+i));
            distances.push_back(distance);
            
            if (distance == 0) {
                ends.push_back(begin+i);
            }
        }
        
        if (ends.empty()) {
            continue;
        }
        
        end = ends[ends.size()/2];
        
        std::vector<int> nearby_distances;
        
        for (int i = 0; i < end - begin - 1; ++i) {
            int distance = hamming_distance(*(it+i), *(it+i+1));
            nearby_distances.push_back(distance);
        }
        
        double variance = al::get_variance_of_distances(nearby_distances);
        
        if (variance > 1.2) {
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
        if (mean1 < 20) {
            continue;
        }
        double mean2 = al::get_mean_of_images(end_image);
        if (mean2 < 20) {
            continue;
        }
        
        std::string begin_pHash = pHash(begin_image, 8);
        std::string end_pHash = pHash(end_image, 8);
        int distance = hamming_distance(begin_pHash, end_pHash);
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

    Json::Value videos_json;
    videos_json["title"] = this->title;
    
    Json::Value source_json;
    source_json["filename"] = this->filename;
    source_json["md5"] = this->md5;
    videos_json["source"].append(source_json);
    
    std::for_each(durations.begin(), durations.end(), [&](const LoopDuration duration) {
        auto uuid = boost::uuids::random_generator()();
        auto uuid_string = boost::uuids::to_string(uuid);
        std::string output_filename = uuid_string + "." + this->output_type;
    
        int start_frame = std::get<0>(duration);
        int end_frame = std::get<1>(duration);
        
        this->open_capture();
        this->capture.set(CV_CAP_PROP_POS_FRAMES, std::get<0>(duration));
        
        // Save video file.
        auto output_path = this->output_path;
        cv::VideoWriter writer(output_path.append(output_filename).string(), this->fourcc, this->fps, this->size);
        cv::Mat image;
        int frame = start_frame;
        while (capture.read(image)) {
            if (frame > end_frame) {
                break;
            }
            writer.write(image);
            frame++;
        }
        writer.release();
        capture.release();
        
        // Save video info json file.
        Json::Value video_json;
        
        video_json["filename"] = output_filename;

        video_json["duration"] = (end_frame - start_frame) / this->fps;
        
        Json::Value frame_json;
        frame_json["start"] = start_frame;
        frame_json["end"] = end_frame;
        video_json["frame"] = frame_json;
        
        Json::Value time_json;
        time_json["start"] = al::time_string(start_frame / this->fps);
        time_json["end"] = al::time_string(end_frame / this->fps);
        video_json["time"] = time_json;
        
        videos_json["loops"].append(video_json);
    });
    
    std::string json_string = videos_json.toStyledString();
    auto caches_path = this->caches_path;
    std::ofstream out(caches_path.append(this->filename + ".json").string());
    out << json_string;
    out.close();
}

bool al::LoopVideo::open_capture() {
    bool flag = this->capture.open(this->input_path.string());
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


std::vector<std::string> al::LoopVideo::get_hash_strings(std::string hash_type) {
    auto caches_path = this->caches_path;
    std::string filename = caches_path.append(this->md5 + "_" + hash_type + ".txt").string();
    
    auto hash_from_file = boost::filesystem::exists(filename);
    al::HashVector hashs;
    this->open_capture();
    if (hash_from_file) {
        // Read video frames hash string from file.
        hashs = al::restore_hash_strings(filename);
    } else {
        // Calculate hash string per frame.
        std::cout << "Calculating hash value... 0%" << std::endl;
        int percent = 0;
        int count = 0, total = this->frame_count;
        cv::Mat image;
        while (this->capture.read(image)) {
            if (count / double(total) * 100 > percent) {
                percent++;
                std::cout << "Calculating hash value... " << percent << "%" << std::endl;
            }
            
            std::string hash = al::hash(hash_type, image, 8);
            hashs.push_back(hash);
            image.release();
            count++;
        }
        // Write video frames hash string when data file does not exist.
        al::save_hash_strings(filename, hashs);
    }
    this->close_capture();
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
    double mean =  sum / distances.size();
    double accum  = 0.0;
    
    std::for_each(distances.begin(), distances.end(), [&](const double distance) {
        accum += (distance-mean) * (distance-mean);
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
    auto ms = boost::posix_time::milliseconds(seconds * 1000);
    auto time = boost::posix_time::time_duration(ms);
    return boost::posix_time::to_simple_string(time);
}

std::string al::md5_of_file(std::string filename) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    
    std::ifstream ifs(filename, std::ios::binary);
    
    char file_buffer[4096];
    while (ifs.read(file_buffer, sizeof(file_buffer)) || ifs.gcount()) {
        MD5_Update(&ctx, file_buffer, ifs.gcount());
    }
    unsigned char digest[MD5_DIGEST_LENGTH] = {};
    MD5_Final(digest, &ctx);
    
    std::stringstream stream;
    for(unsigned i=0; i <MD5_DIGEST_LENGTH; i++) {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    std::string md5_string = stream.str();
    return md5_string;
}



