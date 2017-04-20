//
//  filter.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/4/3.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "loop_video.hpp"
#include "filter.hpp"
#include "algorithm.hpp"
#include "utils.hpp"

#include <opencv2/opencv.hpp>

using namespace al;
using namespace std;
using namespace cv;


// filter 0 找出所有起始帧和结束帧相同的片段，并去重
void al::filter_0(const LoopVideo * loop, LoopDurations &durations) {
    LoopDurations _durations;
    int min_duration_frame = loop->kMinduration * loop->info.fps;
    int max_duration_frame = loop->kMaxduration * loop->info.fps;
    auto hashs = loop->phash_strings;
    
    for (auto it = hashs.begin(); it != (hashs.end() - min_duration_frame); ++it) {
        long begin = distance(hashs.begin(), it);
        
        for (int i = min_duration_frame; i < max_duration_frame; ++i) {
            if (it+i+1 == hashs.end()) { break; }
            
            int distance = hamming_distance(*it, *(it+i));
            if (distance == 0) {
                _durations.push_back(make_tuple(begin, begin + i));
            }
        }
    }
    
    auto end_unique = unique(_durations.begin(), _durations.end(), [](LoopDuration prev, LoopDuration next) {
        return get<0>(prev) == get<0>(next) || get<1>(prev) == get<1>(next);
    });
    _durations.erase(end_unique, _durations.end());
    
    durations = _durations;
    
    // 这里直接判断循环片段起始帧和终止帧的像素点相同来筛选 temp fix
    _durations = LoopDurations();
    
    for (auto duration : durations) {
        long begin_frame, end_frame;
        tie(begin_frame, end_frame) = duration;
        Mat begin_image = loop->frames.at(begin_frame);
        Mat end_image = loop->frames.at(begin_frame);
        
        int count = 0;
        auto length = begin_image.rows * begin_image.cols;
        for (int i = 0; i < length; i++) {
            if (begin_image.at<uchar>(i) == end_image.at<uchar>(i)) {
                count++;
            }
        }
        if (count == length) {
            _durations.push_back(duration);
        }
    }
    
    durations = _durations;    
}

// filter 1 筛选相近片段相似度高的片段
// MAGIC NUMBER ===> if (distance_begin >= 25 && distance_end >= 25) {
void al::filter_1(const al::LoopVideo * loop, LoopDurations &durations) {
    LoopDurations _durations;
    auto hashs = loop->phash_strings;
    
    for (auto it = durations.begin(); it != durations.end()-1; ++it) {
        long prev_begin, prev_end, next_begin, next_end;
        tie(prev_begin, prev_end) = *it;
        tie(next_begin, next_end) = *(it+1);
        
        int distance_begin = hamming_distance(hashs[prev_begin], hashs[next_begin]);
        int distance_end = hamming_distance(hashs[prev_end], hashs[next_end]);

        const int magic_number = 20;
        if (distance_begin >= magic_number && distance_end >= magic_number) {
            _durations.push_back(*it);
        }
    }
    durations = _durations;
}

// filter 2 筛选临近帧像素变化小的片段
// MAGIC NUMBER ===> if (variance > 1.0) {
void al::filter_2(const al::LoopVideo * loop, al::LoopDurations &durations) {
    LoopDurations _durations;
    auto hashs = loop->phash_strings;
    
    for (auto it = durations.begin(); it != durations.end()-1; ++it) {
        long begin_frame, end_frame;
        tie(begin_frame, end_frame) = *it;
        
        std::vector<int> distances;
        for (long i = begin_frame; i < end_frame-1; ++i) {
            int distance = hamming_distance(hashs[i], hashs[i+1]);
            distances.push_back(distance);
        }
        
        double variance = get_variance(distances);
        const double magic_number = 1.0;
        if (variance > magic_number) {
            _durations.push_back(*it);
        }
    }
    durations = _durations;
}

// filter 3 筛选起始或者结束帧为黑色或白色的片段
void al::filter_3(const al::LoopVideo * loop, al::LoopDurations &durations) {
    LoopDurations _durations;
    
    VideoCapture capture;
    capture.open(loop->resized_video_filename.string());
    
    for (auto it = durations.begin(); it != durations.end(); ++it) {
        long start_frame, end_frame;
        tie(start_frame, end_frame) = *it;
        cv::Mat begin_image;
        cv::Mat end_image;
        
        
        capture.set(CV_CAP_PROP_POS_FRAMES, start_frame);
        capture.read(begin_image);
        capture.set(CV_CAP_PROP_POS_FRAMES, end_frame);
        capture.read(end_image);
        
        double mean_begin = get_mean(begin_image);
        double mean_end = get_mean(end_image);
        
        const int magic_number = 20;
        if (mean_begin < magic_number || mean_begin > 255 - magic_number) {
            continue;
        }
        if (mean_end < magic_number || mean_end > 255 - magic_number) {
            continue;
        }

        _durations.push_back(*it);
    }
    capture.release();

    durations = _durations;
}
