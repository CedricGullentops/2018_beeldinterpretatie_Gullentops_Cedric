#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc, argv,
    "{ help h usage ? |  | show this message }"
    "{ image_grey ig  |  | (required) path to image }"
    "{ image_colour ic|  | (required) path to colour image }"
    );

    if (parser.has("help")){
        cerr << "Please use absolute paths." << endl;
        parser.printMessage();
        return -1;
    }

    if (!parser.check()){
        parser.printErrors();
        return -1;
    }

    //Images inlezen en opslagen in Mat variabelen.
    string image_colour(parser.get<string>("imageColor"));
    if (image_gray_colour.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    Mat g_image;
    Mat c_image;
    g_image = imread(image_gray_location, CV_LOAD_IMAGE_GRAYSCALE);
    c_image = imread(image_colour_location, CV_LOAD_IMAGE_COLOR );
    if (g_image.empty() || c_image.empty()){
        cerr << "image not found" << endl;
        return -1;
    }

    //Toon de twee images.
    namedWindow("EAvise Logo - grayscale", WINDOW_AUTOSIZE);
    namedWindow("EAvise Logo - colour", WINDOW_AUTOSIZE);
    imshow("EAvise Logo - grayscale", g_image);
    imshow("EAvise Logo - colour", c_image);
    waitKey(0);

    //Splits de kleur foto op in 3 kanalen en laat elks van deze zien.
    vector<Mat> channels;
    split(c_image,channels);

    namedWindow("Blue", WINDOW_AUTOSIZE);
    imshow("Blue", channels[0]);
    namedWindow("Green", WINDOW_AUTOSIZE);
    imshow("Green", channels[1]);
    namedWindow("Red", WINDOW_AUTOSIZE);
    imshow("Red", channels[2]);

    waitKey(0);

    //Converteer de kleurenfoto naar een zwart-wit foto en toon deze en print elke pixelwaarde af.
    Mat converted;
    cvtColor(c_image, converted, COLOR_BGR2GRAY);
    namedWindow("Converted picture", WINDOW_AUTOSIZE);
    imshow("Converted picture", converted);

    for (int i=0; i<converted.rows; i++){
        for (int j=0; j<converted.cols; j++){
            cout << (int)converted.at<uchar>(i, j);
            cout << " ";
        }
        cout << endl;
    }
    cout << endl;

    waitKey(0);

    //Laatste foto met enkele willekeurige figuren.
    Mat canvas = Mat::zeros(Size(250,250), CV_8UC3);
    rectangle(canvas, Point(50,50), Point(150,150), Scalar(255,255,100), 2);
    circle(canvas, Point(150,150), 75, Scalar(50,50,50), 2);
    line(canvas, Point(0, 0), Point(150, 150), Scalar(100, 100, 100), 5);
    namedWindow("canvas", WINDOW_AUTOSIZE);
    imshow("canvas", canvas);

    waitKey(0);
    return 0;
}


