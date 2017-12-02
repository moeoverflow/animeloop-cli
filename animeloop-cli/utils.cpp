//
//  utils.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/4/3.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "utils.hpp"
#include "algorithm.hpp"
#include "progress_bar.hpp"

#include <json/json.h>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
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

VideoInfo al::get_info(boost::filesystem::path filename) {
    VideoCapture capture;
    capture.open(filename.string());
    return get_info(capture);
}


int fork_resize_video(path input_filepath, path temp_filepath, Size size) {
    int pid = fork();
    if(pid != 0) {
        /* We're in the parent process, return the child's pid. */
        return pid;
    }
    /* Otherwise, we're in the child process, so let's exec curl. */
    execlp("ffmpeg", "ffmpeg", "-loglevel", "panic", "-i", input_filepath.string().c_str(), "-s", (to_string(size.width) + "x" + to_string(size.height)).c_str(), "-an", temp_filepath.string().c_str(), NULL);

    exit(100);
}

void al::resize_video(path input_filepath, path output_filepath, Size size) {
    path temp_output = path(output_filepath).parent_path().append("temp");
    path temp_filename = path(temp_output).append(output_filepath.filename().string());
    if (!exists(temp_output)) {
        create_directories(temp_output);
    } else {
        if (exists(temp_filename)) {
            remove(temp_filename);
        }
    }

    auto if_exists = exists(output_filepath);

    if (!if_exists) {
        if (system("which ffmpeg1 &> /dev/null") == 0) {
            cout << ":: resizing video..." << endl;
            int cpid = fork_resize_video(input_filepath, temp_filename, size);
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
            cout << ":: resizing video..." << endl;
            VideoCapture capture;
            capture.open(input_filepath.string());

            VideoInfo info = get_info(capture);
            auto fourcc = CV_FOURCC('a', 'v', 'c', '1');
            VideoWriter writer(temp_filename.string(), fourcc, info.fps, size);

            ProgressBar progressBar(info.frame_count, 35);

            cv::Mat image;
            while (capture.read(image)) {
                cv::resize(image, image, size);
                writer.write(image);
                image.release();

                ++progressBar;
                progressBar.display();
            }

            writer.release();
            capture.release();

            progressBar.done();
            cout << "[o] done." << endl;
        }

        rename(temp_filename, output_filepath);
    }
}

bool al::get_frames(boost::filesystem::path file, FrameVector &frames) {
    cout << ":: get frames from resized video file." << endl;
    FrameVector _frames;
    VideoCapture capture;
    capture.open(file.string());

    VideoInfo info = get_info(capture);
    ProgressBar progressBar(info.frame_count, 35);

    cv::Mat image;
    while (capture.read(image)) {
        _frames.push_back(image);
        ++progressBar;
        progressBar.display();
    }
    capture.release();
    frames = _frames;

    progressBar.done();
    cout << "[o] done." << endl;
    return true;
}

void al::get_hash_strings(boost::filesystem::path filename, string type, int length, int dct_length, HashVector &hash_strings, path hash_file) {
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
        std::cout << ":: restore " << type << " value from file..." << std::endl;
        
        std::ifstream input_file(hash_file.string());
        HashVector hashs;

        while (input_file) {
            string line;
            getline(input_file, line);
            hashs.push_back(line);
        }
        input_file.close();
        
        hash_strings = hashs;
        cout << "[o] done." << endl;
    } else {
        cout << ":: calculating " << type << " value..." << endl;

        VideoCapture capture;
        capture.open(filename.string());
        VideoInfo info = get_info(capture);
        HashVector hashs;

        ProgressBar progressBar(info.frame_count, 35);

        cv::Mat image;
        while (capture.read(image)) {
            string hash = al::pHash(image, length, dct_length);
            hashs.push_back(hash);
            image.release();

            ++progressBar;
            progressBar.display();
        }
        hash_strings = hashs;

        progressBar.done();
        cout << "[o] done." << endl;

        cout << ":: save " << type << " value into file..." << endl;
        std::ofstream output_file(temp_filename.string());
        ostream_iterator<string> output_iterator(output_file, "\n");
        std::copy(hashs.begin(), hashs.end(), output_iterator);
        output_file.close();

        rename(temp_filename, hash_file);
        cout << "[o] done." << endl;
    }
}

void al::get_cuts(boost::filesystem::path resized_video_filepath, CutVector &cuts, boost::filesystem::path cuts_filepath) {
    path temp_path = cuts_filepath.parent_path().append("temp");
    path temp_filename = path(temp_path).append(cuts_filepath.filename().string());
    CutVector _cuts;

    if (!exists(temp_path)) {
        create_directories(temp_path);
    } else {
        if (exists(temp_filename)) {
            remove(temp_filename);
        }
    }

    auto if_exists = exists(cuts_filepath);
    if (if_exists) {
        cout << ":: restore cuts data from file..." << endl;

        std::ifstream input_file(cuts_filepath.string());

        int cut;
        while(input_file >> cut) {
            _cuts.push_back(cut);
        }

        input_file.close();
        cout << "[o] done." << endl;
    } else {
        cout << ":: calculate cuts data..." << endl;

        Mat prevframe, nextframe, differframe;
        int i = 0;

        VideoCapture capture(resized_video_filepath.string());
        VideoInfo info = get_info(capture);

        ProgressBar progressBar(info.frame_count, 35);

        capture.read(prevframe);
        cvtColor(prevframe, prevframe, CV_RGB2GRAY);

        while (capture.read(nextframe)) {
            cvtColor(nextframe, nextframe, CV_RGB2GRAY);

            absdiff(nextframe, prevframe, differframe);

            int count = 0;
            int total = differframe.rows * differframe.cols;

            for (int row = 0; row < differframe.rows; ++row) {
                for (int col = 0; col < differframe.cols; ++col) {
                    if (differframe.at<uchar>(row, col) > 10) {
                        count++;
                    }
                }
            }

            double rate = (total != 0) ? double(count) / total : 0;

            if (rate > 0.8) {
                _cuts.push_back(i);
            }

            prevframe = nextframe;

            ++progressBar;
            progressBar.display();
        }
        capture.release();


        std::ofstream output_file(temp_filename.string());

        for (auto cut : _cuts) {
            output_file << cut << " ";
        }
        output_file.close();

        rename(temp_filename, cuts_filepath);

        progressBar.done();
        cout << "[o] done." << endl;
    }
    cuts = _cuts;
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

Vec3b al::get_mean_rgb(Mat image) {
    int total_b = 0, total_g = 0, total_r = 0;
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            total_b += image.at<Vec3b>(row, col)[0];
            total_g += image.at<Vec3b>(row, col)[1];
            total_r += image.at<Vec3b>(row, col)[2];
        }
    }
    int mean_b = total_b / (image.cols*image.cols);
    int mean_g = total_g / (image.cols*image.cols);
    int mean_r = total_r / (image.cols*image.cols);

    Vec3b rgb(mean_b, mean_g, mean_r);
    return rgb;
}

std::string al::time_string(double seconds) {
    auto ms = boost::posix_time::milliseconds(seconds * 1000);
    auto time = boost::posix_time::time_duration(ms);
    return boost::posix_time::to_simple_string(time);
}