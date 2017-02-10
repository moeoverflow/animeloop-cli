 //
//  main.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/9.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "loop_video.hpp"
#include "dHash.hpp"
#include "aHash.hpp"
#include "utils.hpp"


const std::string kName = "test";
const std::string kVideoPath = "~/cvworkspace/test.mp4";
const std::string kOutputPath = "~/cvworkspace/test_loops";
const std::string kWorkpath = "~/cvworkspace/animeloop/";


int main(int argc, const char * argv[]) {

    al::LoopVideo loop_video(kName, kVideoPath, kWorkpath);
    loop_video.minDuration = 0.9;
    
    loop_video.find_loop_video();
    loop_video.write_loop_video_files();
    
    return 0;
}
