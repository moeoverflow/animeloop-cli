//
//  aHash.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/10.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef aHash_hpp
#define aHash_hpp

#include <iostream>

#include <opencv2/opencv.hpp>

/**
 calculate aHash string of an image.

 @param image target image
 @param rows resize height
 @param cols resize width
 @return aHash tring
 */
std::string aHash(cv::Mat image, int rows, int cols);

#endif /* aHash_hpp */
