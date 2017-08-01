//
//  filter.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/4/3.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef filter_hpp
#define filter_hpp

#include <iostream>
#include "models.hpp"

namespace al {
    namespace filter {
        void all_loops(al::LoopVideo * loop, al::LoopDurations &durations);
        void cut_in_loop(al::LoopVideo * loop, al::LoopDurations &durations);
        void loop_nearby(al::LoopVideo * loop, al::LoopDurations &durations);
        void loop_tiny_frame_change(al::LoopVideo * loop, al::LoopDurations &durations);
        void loop_white_or_black(al::LoopVideo * loop, al::LoopDurations &durations);
        void loop_same_color(al::LoopVideo * loop, al::LoopDurations &durations);
    }
}

#endif /* filter_hpp */
