//
//  utils.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/4/3.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "utils.hpp"
#include "algorithm.hpp"

#include "json.h"
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <locale>
#include <openssl/md5.h>
#include <fstream>
#include <numeric>

using namespace std;
using namespace boost::filesystem;
using namespace cv;
using namespace al;


VideoInfo al::get_info(VideoCapture &capture) {
    VideoInfo info;
    info.fps = capture.get(CV_CAP_PROP_FPS);
    info.fourcc = capture.get(CV_CAP_PROP_FOURCC);
    auto height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    auto width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    info.size = cv::Size(width, height);
    info.frame_count = capture.get(CV_CAP_PROP_FRAME_COUNT);
    return info;
}

VideoInfo al::get_info(std::string filename) {
    VideoCapture capture;
    capture.open(filename);
    return get_info(capture);
}



void al::resize_video(string file, string output, Size size) {
    auto if_exists = exists(output);
    
    if (!if_exists) {
        // Calculate hash string per frame.
        std::cout << "Resizing video... 0%" << std::endl;
        VideoCapture capture;
        capture.open(file);
        
        VideoInfo info = get_info(capture);

        VideoWriter writer(output, CV_FOURCC('H', '2', '6', '4'), info.fps, size);

        
        int percent = 0;
        int count = 0, total = info.frame_count;
        cv::Mat image;
        while (capture.read(image)) {
            if (count / double(total) * 100 > percent) {
                percent++;
                std::cout << "Resizing video... " << percent << "%" << std::endl;
            }
            cv::resize(image, image, size);
            writer.write(image);
            image.release();
            count++;
        }
        
        writer.release();
        capture.release();
    }
}

bool al::get_frames(string file, FrameVector &frames) {
    FrameVector _frames;
    VideoCapture capture;
    capture.open(file);
    
    cv::Mat image;
    while (capture.read(image)) {
        _frames.push_back(image);
    }
    capture.release();
    frames = _frames;
    
    return true;
}

bool al::get_hash_strings(string file, string type, HashVector &hash_strings, string hash_file) {
    
    auto if_exists = exists(hash_file);
    VideoCapture capture;
    if (if_exists) {
        // Read video frames hash string from file.
        std::cout << "Restore " << type << " value from file..." << std::endl;
        
        std::ifstream input_file(hash_file);
        HashVector hashs;
        
        int count;
        count = 0;
        while (input_file) {
            string line;
            getline(input_file, line);
            hashs.push_back(line);
        }
        input_file.close();
        
        hash_strings = hashs;
        return true;
    } else {
        // Calculate hash string per frame.
        VideoCapture capture;
        capture.open(file);
        VideoInfo info = get_info(capture);
        HashVector hashs;
        
        
        cout << "Calculating " << type << " value... 0%" << endl;
        int percent = 0;
        int count = 0, total = info.frame_count;
        cv::Mat image;
        while (capture.read(image)) {
            if (count / double(total) * 100 > percent) {
                percent++;
                std::cout << "Calculating " << type << " value... " << percent << "%" << std::endl;
            }
            
            string hash = al::hash(type, image, 8);
            hashs.push_back(hash);
            image.release();
            count++;
        }
        hash_strings = hashs;
        
        cout << "Save " << type << " value into file..." << endl;
        std::ofstream output_file(hash_file);
        ostream_iterator<string> output_iterator(output_file, "\n");
        std::copy(hashs.begin(), hashs.end(), output_iterator);
        output_file.close();
        return true;
    }
    return false;
}


double al::get_variance(std::vector<int> distances) {
    double sum = std::accumulate(distances.begin(), distances.end(), 0.0);
    double mean =  sum / distances.size();
    double accum  = 0.0;
    
    std::for_each(distances.begin(), distances.end(), [&](const double distance) {
        accum += (distance-mean) * (distance-mean);
    });
    double variance = sqrt(accum / (distances.size() - 1));
    return variance;
}

double al::get_mean(Mat image) {
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
