/*Cédric Gullentops*/
/*
Build options: `pkg-config opencv --libs`
-image_1=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_2/sign.jpg
-image_2=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_2/sign2.jpg
-image_3=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_2/sign3.jpg
-image_4=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_2/sign4.jpg
*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int alpha_slider_max1 = 180;
int alpha_slider1 = 110;
const int alpha_slider_max2 = 180;
int alpha_slider2 = 77;
const int alpha_slider_max3 = 255;
int alpha_slider3 = 85;
int h1;
int h2;
int s1;

//Slider1
static void on_trackbar1(int, void*)
{
    h1 = alpha_slider1;
}

//Slider2
static void on_trackbar2(int, void*)
{
    h2 = alpha_slider2;
}

//Slider3
static void on_trackbar3(int, void*)
{
    s1 = alpha_slider3;
}


int main(int argc,const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  | show this message }"
    "{image_1 i1       |  | (required) absolute path to first image }"
    "{image_2 i2       |  | (required) absolute path to second image }"
    "{image_3 i3       |  | (required) absolute path to third image }"
    "{image_4 i4       |  | (required) absolute path to fourth image }"
    );

    if(parser.has("help")){
        cerr << "Please use absolute paths." << endl;
        parser.printMessage();
        return -1;
    }

    if(!parser.check()){
        parser.printErrors();
        return -1;
    }

    //Haal de 4 image locaties
    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    string image_3_location(parser.get<string>("image_3"));
    string image_4_location(parser.get<string>("image_4"));
    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty()  || image_4_location.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    //Lees de 4 foto's in
    Mat img[4];
    img[0] = imread(image_1_location);
    img[1] = imread(image_2_location);
    img[2] = imread(image_3_location);
    img[3] = imread(image_4_location);

    if(img[0].empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(img[1].empty()){
        cerr <<"Image2 not found";
        return -1;
    }
    if(img[2].empty()){
        cerr <<"Image3 not found";
        return -1;
    }
    if(img[3].empty()){
        cerr <<"Image3 not found";
        return -1;
    }

    //Tests op foto kleur threshold en range van waarden
    Mat bgr[3],red_threshold,result;
    for(int i=0;i<4;i++){
        split(img[i],bgr);
        threshold(bgr[2],red_threshold,120,255,THRESH_BINARY);
        img[i].copyTo(result,red_threshold);
        imshow("Red img using threshold",result);
        waitKey(0);
        destroyAllWindows();
        inRange(img[i], Scalar(0, 0, 160), Scalar(50, 50, 255), result);
        imshow("Red img using inRange",result);
        waitKey(0);
        destroyAllWindows();
    }

    //Tests op foto kleur met hsv
    Mat hsv[4],mask,mask2;
    for(int i=0;i<4;i++){
        cvtColor(img[i],hsv[i],COLOR_BGR2HSV);
        inRange(hsv[i], Scalar(160, 100, 100), Scalar(180, 255, 255), mask);
        inRange(hsv[i], Scalar(0, 100, 100), Scalar(5, 255, 255), mask2);
        mask = mask|mask2;
        erode(mask,mask,Mat(),Point(-1,-1),1);
        dilate(mask,mask,Mat(),Point(-1,-1),3);
        dilate(mask, mask, Mat(), Point(-1, -1), 5);
        erode(mask, mask, Mat(), Point(-1, -1), 5);
        img[i].copyTo(result,mask);
        imshow("HSV image",result);
        waitKey(0);
        destroyAllWindows();
    }

    //Masker
    inRange(hsv[0], Scalar(160, 100, 100), Scalar(180, 255, 255), mask);
    inRange(hsv[0], Scalar(0, 100, 100), Scalar(5, 255, 255), mask2);
    mask = mask|mask2;
    erode(mask,mask,Mat(),Point(-1,-1),1);
    dilate(mask,mask,Mat(),Point(-1,-1),3);
    dilate(mask, mask, Mat(), Point(-1, -1), 5);
    erode(mask, mask, Mat(), Point(-1, -1), 5);
    imshow("Masker",mask);
    waitKey(0);
    destroyAllWindows();

    //Blob via contours
    vector <vector<Point>>contours;
    findContours(mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    vector<Point> hull;
    convexHull(contours[0],hull);
    vector<Point> blob = hull;
    for (int i = 0; i < contours.size(); i++){
        if (contourArea(contours[i]) > contourArea(blob)){
            convexHull(contours[i], hull);
            blob = hull;
        }
    }

    //Gebruiks maskers en crop via rechthoek
    vector<vector<Point>> temp;
    temp.push_back((blob));
    drawContours(mask, temp, -1, 255, -1);
    Mat contourmask(Mat(img[0].size(), CV_8UC3));
    imshow("Contour masker",mask);
    img[0].copyTo(result,mask);
    imshow("Image + contour", result);
    Rect bound = boundingRect(mask);
    Mat crop = result(bound);
    imshow("Cropped", crop);
    waitKey(0);
    destroyAllWindows();

    //Maak sliders
    namedWindow("Foto met slider");
    resizeWindow("Sliders", 600, 600);
    createTrackbar("max Hue", "Foto met slider", &alpha_slider1, alpha_slider_max1, on_trackbar1);
    createTrackbar("min Hue", "Foto met slider", &alpha_slider2, alpha_slider_max2, on_trackbar2);
    createTrackbar("Saturation", "Foto met slider", &alpha_slider3, alpha_slider_max3, on_trackbar3);
    on_trackbar1(alpha_slider1,0);
    on_trackbar2(alpha_slider2,0);
    on_trackbar3(alpha_slider3,0);

    //Loop voor waarden aan te passen
    while (true)
    {
        inRange(hsv[0], Scalar(h1, s1, 100), Scalar(180, 255, 255), mask);
        inRange(hsv[0], Scalar(0, s1, 100), Scalar(h2, 255, 255), mask2);
        mask = mask|mask2;
        erode(mask,mask,Mat(),Point(-1,-1),1);
        dilate(mask,mask,Mat(),Point(-1,-1),3);
        dilate(mask, mask, Mat(), Point(-1, -1), 5);
        erode(mask, mask, Mat(), Point(-1, -1), 5);
        result = mask;
        img[0].copyTo(result,mask);
        imshow("Foto met slider",result);
        int key = waitKey(10);
        if (key == 13)
        {
            break;
        }
    }
}

