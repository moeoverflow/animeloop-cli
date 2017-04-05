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
    void filter_0(const al::LoopVideo * loop, al::LoopDurations &durations);
    void filter_1(const al::LoopVideo * loop, al::LoopDurations &durations);
    void filter_2(const al::LoopVideo * loop, al::LoopDurations &durations);
    void filter_3(const al::LoopVideo * loop, al::LoopDurations &durations);
}

#endif /* filter_hpp */
