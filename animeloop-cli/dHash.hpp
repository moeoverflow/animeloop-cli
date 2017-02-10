//
//  dHash.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/9.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef dHash_hpp
#define dHash_hpp

#include <iostream>

#include <opencv2/opencv.hpp>


/**
 calculate dHash string of an image.

 @param image target image
 @param rows resize height
 @param cols resize width
 @return dHash string
 */
std::string dHash(cv::Mat image, int rows, int cols);


#endif /* dHash_hpp */
