#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../animeloop-cli/algorithm.hpp"

using namespace std;
using namespace cv;

vector<string> imagepaths = { "/home/shincurry/Dropbox/Paper/animeloop/Resources/p1.jpg",
                    "/home/shincurry/Dropbox/Paper/animeloop/Resources/p2.jpg",
                    "/home/shincurry/Dropbox/Paper/animeloop/Resources/p3.jpg",
                    "/home/shincurry/Dropbox/Paper/animeloop/Resources/p4.jpg" };
int main(int argc, char * argv[])
{
    vector<Mat> images;
    for (auto it = imagepaths.begin(); it != imagepaths.end(); ++it)
    {
        Mat image = imread(*it, CV_LOAD_IMAGE_COLOR);
        images.push_back(image);
    }

    vector<pair<int, vector<string>>> hashss;
    hashss.push_back(make_pair(8, vector<string>()));
    hashss.push_back(make_pair(32, vector<string>()));
    hashss.push_back(make_pair(64, vector<string>()));
    hashss.push_back(make_pair(128, vector<string>()));

    for (auto itt = hashss.begin(); itt != hashss.end(); ++itt)
    {
        auto len = itt->first;
        auto hashs = itt->second;

        for (auto image : images)
        {
            string hash = al::pHash(image, len, len);
            hashs.push_back(hash);
        }

        for (auto it = hashs.begin()+1; it != hashs.end(); ++it)
        {
            auto index = distance(hashs.begin(), it);

            int xs = al::hamming_distance(*hashs.begin(), *it);
            cout << "p1 and " << "p" << index+1 << ": " << 1 - double(xs) / (len * len) << endl;
        }
        cout << endl;
    }

    return 0;
}


