#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<string> imagepaths = { "/home/shincurry/Dropbox/Paper/animeloop/Resources/color/p1.jpg",
                    "/home/shincurry/Dropbox/Paper/animeloop/Resources/color/p2.jpg",
                    "/home/shincurry/Dropbox/Paper/animeloop/Resources/color/p3.jpg",
                  "/home/shincurry/Dropbox/Paper/animeloop/Resources/color/p11.jpg",
                  "/home/shincurry/Dropbox/Paper/animeloop/Resources/color/p21.jpg",
                              "/home/shincurry/Dropbox/Paper/animeloop/Resources/color/p31.jpg"};

int main(int argc, char** argv)
{
    for (auto it = imagepaths.begin(); it != imagepaths.end(); ++it)
    {
//        Mat image = imread(*it, CV_LOAD_IMAGE_COLOR);
        Mat image = imread(*it, CV_LOAD_IMAGE_COLOR);

        long total_b = 0, total_g = 0, total_r = 0;
        for (int row = 0; row < image.rows; ++row) {
            for (int col = 0; col < image.cols; ++col) {
                total_b += image.at<cv::Vec3b>(row, col)[0];
                total_g += image.at<cv::Vec3b>(row, col)[1];
                total_r += image.at<cv::Vec3b>(row, col)[2];
            }
        }
        int mean_b = total_b / (image.cols*image.cols);
        int mean_g = total_g / (image.cols*image.cols);
        int mean_r = total_r / (image.cols*image.cols);
        printf("(%d, %d, %d)\n", mean_b, mean_g, mean_r);
    }


    return 0;
}