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

/**
 filter all_loops
 Find all loop fragments that the begin frame is same as end one, then remove the repetition.

 @param loop LoopVideo instance
 @param durations reference of durations value
 */
void al::filter::all_loops(al::LoopVideo *loop, al::LoopDurations &durations) {
    LoopDurations _durations;
    int min_duration_frame = loop->min_duration * loop->info.fps;
    int max_duration_frame = loop->max_duration * loop->info.fps;
    auto hashs = loop->phash_strings;
    
    for (auto it = hashs.begin(); it != (hashs.end() - min_duration_frame); ++it) {
        long begin = distance(hashs.begin(), it);

        long end = -1;
        for (int i = min_duration_frame; i < max_duration_frame; ++i) {
            if (it+i+1 == hashs.end()) { break; }

            int distance = hamming_distance(*it, *(it+i));
            double similar = 1.00 - double(distance) / (*it).size();
            if (similar >= 0.98) {
                end = begin + i;
            }
        }
        if (end != -1) {
            _durations.push_back(make_tuple(begin, end));
        }
    }

    auto end_unique = unique(_durations.begin(), _durations.end(), [](LoopDuration prev, LoopDuration next) {
        return get<0>(prev) == get<0>(next) || get<1>(prev) == get<1>(next);
    });
    _durations.erase(end_unique, _durations.end());
    
    durations = _durations;
}

/**
 filter cut_in_loop
 Check if there is a cut in loop.

 @param loop LoopVideo instance
 @param durations reference of durations value
 */
void al::filter::cut_in_loop(al::LoopVideo * loop, LoopDurations &durations)
{
    LoopDurations _durations;

    for (auto duration : durations) {
        long begin, end;
        tie(begin, end) = duration;

        bool cut_in_loop = false;
        for (auto cut : loop->cuts) {
            if (cut >= begin && cut <= end) {
                cut_in_loop = true;
                break;
            }
        }

        if (!cut_in_loop) {
            _durations.push_back(duration);
        }
    }

    durations = _durations;
}

/**
 filter loop_nearby
 find the loop fragments that is same as nearby one, then remove it.

 @param loop LoopVideo instance
 @param durations reference of durations value
 */
void al::filter::loop_nearby(al::LoopVideo *loop, al::LoopDurations &durations) {
    LoopDurations _durations;
    auto hashs = loop->phash_strings;

    
    for (auto it = durations.begin(); it != durations.end()-1; ++it) {
        long prev_begin, prev_end, next_begin, next_end;
        tie(prev_begin, prev_end) = *it;
        tie(next_begin, next_end) = *(it+1);
        
        int distance_begin = hamming_distance(hashs[prev_begin], hashs[next_begin]);
        int distance_end = hamming_distance(hashs[prev_end], hashs[next_end]);

        int filter_n = hashs[0].size() / 2.5;
        if (distance_begin >= filter_n && distance_end >= filter_n) {
            _durations.push_back(*it);
        }
    }
    durations = _durations;
}

/**
 filter loop_frame_tiny_change
 find the loop fragments with a small frame pixels change.

 @param loop LoopVideo instance
 @param durations reference of durations value
 */
void al::filter::loop_tiny_frame_change(al::LoopVideo * loop, al::LoopDurations &durations) {
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
        const double filter_n = 2.0;
        if (variance > filter_n) {
            _durations.push_back(*it);
        }
    }
    durations = _durations;
}


/**
 filter loop_white_or_black
 find the loop fragments that the color of begin/end frame is most black or white, then remove it.

 @param loop LoopVideo instance
 @param durations reference of durations value
 */
void al::filter::loop_white_or_black(al::LoopVideo *loop, al::LoopDurations &durations) {
    LoopDurations _durations;

    VideoCapture capture;
    capture.open(loop->resized_video_filepath.string());

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

/**
 filter loop_same_color
 check if the color in the begin frame is the same as the end one.

 @param loop LoopVideo instance
 @param durations reference of durations value
 */
void al::filter::loop_same_color(al::LoopVideo * loop, al::LoopDurations &durations) {
    LoopDurations _durations;

    for (auto duration : durations) {
        long start_frame, end_frame;
        tie(start_frame, end_frame) = duration;

        Vec3b mean_rgb_start = get_mean_rgb(loop->resized_frames[start_frame]);
        Vec3b mean_rgb_end = get_mean_rgb(loop->resized_frames[end_frame]);

        if (mean_rgb_start[0] == mean_rgb_end[0] &&
            mean_rgb_start[1] == mean_rgb_end[1] &&
            mean_rgb_start[2] == mean_rgb_end[2] ) {
            _durations.push_back(duration);
        }
    }

    durations = _durations;
}
