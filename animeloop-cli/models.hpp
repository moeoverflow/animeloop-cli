//
//  models.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/4/3.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef models_hpp
#define models_hpp

#include <iostream>
#include <opencv2/opencv.hpp>

namespace al {
    typedef std::tuple<long, long> LoopDuration;
    typedef std::vector<LoopDuration> LoopDurations;
    typedef std::vector<std::string> HashVector;
    typedef std::vector<cv::Mat> FrameVector;
    typedef std::vector<int> CutVector;
        
    struct VideoInfo {
        double fps;
        double fourcc;
        cv::Size size;
        int frame_count;
    };
}


#endif /* models_hpp */
