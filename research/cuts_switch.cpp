#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

string basepath = "/home/shincurry/Dropbox/Paper/animeloop/Resources/lens_switch/";
string videopath = basepath + "lens_switch_example_.mkv";
//string videopath = "/home/shincurry/Downloads/[LoliHouse] Kimi no Na wa [BDRip 1920x1080 AVC-yuv420p8 AAC PGS(chs,eng,jpn)].mkv";

int main(int argc, char** argv)
{
    VideoCapture capture(videopath);
    if (!capture.isOpened())
    {
        cout << "No camera or video input!\n" << endl;
        return -1;
    }

    int total_frames = capture.get(CV_CAP_PROP_FRAME_COUNT);

    Mat prevframe, nextframe, differframe;
    int i = 0;


    ofstream file(basepath + "framediff.txt");

    capture.read(prevframe);
    cvtColor(prevframe, prevframe, CV_RGB2GRAY);

    while (capture.read(nextframe)) {
        cvtColor(nextframe, nextframe, CV_RGB2GRAY);

        //帧差法
        absdiff(nextframe, prevframe, differframe);
//        imshow("Prev Frame", prevframe);
//        imshow("Next Frame", nextframe);
//        imshow("Differ Frame", differframe);

        int count = 0;
        int total = differframe.rows * differframe.cols;

        for (int row = 0; row < differframe.rows; ++row) {
            for (int col = 0; col < differframe.cols; ++col) {
                if (differframe.at<uchar>(row, col) > 10) {
                    count++;
                }
            }
        }

        double rate = (total != 0) ? double(count) / total : 0;

        if (rate > 0.85) {
//            cvWaitKey();
            cout << "len switched. " << i << endl;
        }

        if (file.is_open())
        {

            file << rate << endl;
        }

        char c = waitKey(33);
        if (c == 27) break;

        prevframe = nextframe;
        i++;
    }

//    file.close();
    return 0;
}