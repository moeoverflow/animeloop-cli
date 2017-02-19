//
//  utils.hpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/9.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <iostream>
#include <vector>



std::vector<std::string> read_vector_of_string_from_file(std::string filepath);
void write_vector_of_string_to_file(std::string filepath, std::vector<std::string>);

std::string convert_seconds_to_time(double seconds);

#endif /* utils_hpp */
