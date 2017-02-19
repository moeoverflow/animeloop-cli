//
//  algorithm.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/14.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef algorithm_hpp
#define algorithm_hpp

#include <iostream>

#include <opencv2/opencv.hpp>

namespace al {
    /**
     choose which hash type
     value can be "ahash", "dhash" or "phash"
     
     @param type which type of hash
     @param image target image
     @param length resize height and width
     @return aHash tring
     */
    std::string hash(std::string type, cv::Mat image, int length);
    
    /**
     calculate aHash string of an image.
     
     @param image target image
     @param length resize height and width
     @return aHash tring
     */
    std::string aHash(cv::Mat image, int length);
    
    /**
     calculate dHash string of an image.
     
     @param image target image
     @param length resize height and width
     @return dHash string
     */
    std::string dHash(cv::Mat image, int length);
    
    /**
     calculate pHash string of an image.
     
     @param image target image
     @param length resize height and width(length > 8)
     @return pHash string
     */
    std::string pHash(cv::Mat image, int length);
    
    
    /**
     Calculate hamming distance between two image hash strings.
     
     @param str1 a image hash string
     @param str2 another image hash string
     @return hamming distance value
     */
    unsigned int hamming_distance(std::string str1, std::string str2);
    
    /**
     Calculate hamming distance between two image hash int64 value.
     
     @param str1 a image hash int64 value
     @param str2 another image hash int64 value
     @return hamming distance value
     */
    unsigned int hamming_distance(int64_t n1, int64_t n2);
}


#endif /* algorithm_hpp */
