//
//  utils.cpp
//  animeloop-cli
//
//  Created by ShinCurry on 2017/2/9.
//  Copyright © 2017年 ShinCurry. All rights reserved.
//

#include "utils.hpp"
#include <fstream>


unsigned int hamming_distance(std::string str1, std::string str2) {
    if (str1.empty() || str2.empty()) {
        return 0;
    }
    
    unsigned long len1 = str1.length();
    unsigned long len2 = str2.length();
    
    if (len1 != len2) {
        return 0;
    }
    
    unsigned int dist = 0;
    for (int i = 0; i < len1; ++i) {
        dist += (str1.at(i) != str2.at(i)) ? 1 : 0;
    }
    return dist;
}

unsigned int hamming_distance(int64_t n1, int64_t n2) {
    int64_t difference;
    
    unsigned int count = 0;
    for (difference = n1 ^ n2; difference; difference >>= 1) {
        count += difference & 1;
    }
    
    return count;
}

std::vector<std::string> read_vector_of_string_from_file(std::string filepath) {
    std::ifstream input_file(filepath);
    std::vector<std::string> hashs;
    
    int count;
    count = 0;
    while (input_file) {
        std::string line;
        std::getline(input_file, line);
        hashs.push_back(line);
        std::cout << "reading hash file [" << ++count << "].\n";
    }
    std::cout << "reading hash file finished.\n";
    
    
    input_file.close();
    
    return hashs;
}

void write_vector_of_string_to_file(std::string filepath, std::vector<std::string> hashs) {
    std::ofstream output_file(filepath);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(hashs.begin(), hashs.end(), output_iterator);
    output_file.close();
    std::cout << "writing hash file.\n";
}
