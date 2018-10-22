#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
    CommandLineParser parser(argc, argv,
    "{ help h  |  | show this message }"
    "{ image_grey ig   |  | (required) path to image }"
    "{ gfile 2    |  | second graphic file }"
    );

    if (parser.has("help")){
        parser.printMessage();
        cerr << "Use absolute paths" << endl;
        return -1;
    }

    string image_gray_location(parser.get<string>("image_grey"));
    string image_color_location(parser.get<string>("image_color"));
    if (image_gray_location.empty()){
        cerr << "Something wrong in the given arguments";
        parser.printMessage();
        return -1;
    }

    Mat image;
    image = imread(image_gray_location);
    if (image.empty()){
        cerr << "image not found";
        return -1;
    }
    imshow("EAvise Logo - grayscale", image);
    waitKey(0);

    image = imread(image_color_location);
    if (image.empty()){
        cerr << "image not found";
        return -1;
    }
    imshow("EAvise Logo - colour", image);
    waitKey(0);

    vector<Mat> channels;
    split(image,channels);
    ...

    cvtColor(...)


    ...
    loop over pixels

    Mat canvas = Mat::zeros(Size(250,250), CV_8UC3);
    rectangle(canvas, Rect(150,150), Scalar(255,255,0), 2);
    circle(canvas, Point(150,150), 75, Scalar(255,255,0), 2);
    imshow("canvas", canvas);
    waitKey(0);
}


