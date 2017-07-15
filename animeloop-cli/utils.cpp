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
#include <boost/regex.hpp>
#include <openssl/md5.h>
#include <numeric>
#include <sys/wait.h>


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


int fork_resize_video(path input, path temp_filename, Size size) {
    int pid = fork();
    if(pid != 0) {
        /* We're in the parent process, return the child's pid. */
        return pid;
    }
    /* Otherwise, we're in the child process, so let's exec curl. */
    execlp("ffmpeg", "ffmpeg", "-loglevel", "panic", "-i", input.string().c_str(), "-s", (to_string(size.width) + "x" + to_string(size.height)).c_str(), "-an", temp_filename.string().c_str(), NULL);

    exit(100);
}

void al::resize_video(path input, path output, Size size) {
    path temp_output = output.parent_path().append("temp");
    path temp_filename = path(temp_output).append(output.filename().string());
    if (!exists(temp_output)) {
        create_directories(temp_output);
    } else {
        if (exists(temp_filename)) {
            remove(temp_filename);
        }
    }

    auto if_exists = exists(output);
    
    if (!if_exists) {
        if (system("which ffmpeg") == 0) {
            cout << "Resizing video..." << endl;

            int cpid = fork_resize_video(input, temp_filename, size);
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
                cerr << "The child was killed or segfaulted or something\n" << endl;
            }

            status = WEXITSTATUS(status);


            cout << "done." << endl;

        } else {
            // Calculate hash string per frame.
            cout << "Resizing video... 0%" << endl;
            VideoCapture capture;
            capture.open(input.string());

            VideoInfo info = get_info(capture);

            VideoWriter writer(temp_filename.string(), CV_FOURCC('H', '2', '6', '4'), info.fps, size);


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

        system(("mv " + temp_filename.string() + " " + output.string()).c_str());
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

void al::get_hash_strings(path filename, string type, HashVector &hash_strings, path hash_file) {
    path temp_path = hash_file.parent_path().append("temp");
    path temp_filename = path(temp_path).append(hash_file.filename().string());

    if (!exists(temp_path)) {
        create_directories(temp_path);
    } else {
        if (exists(temp_filename)) {
            remove(temp_filename);
        }
    }

    auto if_exists = exists(hash_file);
    VideoCapture capture;
    if (if_exists) {
        // Read video frames hash string from file.
        std::cout << "Restore " << type << " value from file..." << std::endl;
        
        std::ifstream input_file(hash_file.string());
        HashVector hashs;

        while (input_file) {
            string line;
            getline(input_file, line);
            hashs.push_back(line);
        }
        input_file.close();
        
        hash_strings = hashs;
    } else {
        // Calculate hash string per frame.
        VideoCapture capture;
        capture.open(filename.string());
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
        std::ofstream output_file(temp_filename.string());
        ostream_iterator<string> output_iterator(output_file, "\n");
        std::copy(hashs.begin(), hashs.end(), output_iterator);
        output_file.close();

        system(("mv " + temp_filename.string() + " " + hash_file.string()).c_str());
    }
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
