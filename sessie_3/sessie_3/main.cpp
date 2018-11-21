/*Cédric Gullentops*/
/*
Build options: `pkg-config opencv --libs`
-image_1=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_3/recht.jpg
-image_2=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_3/rot.jpg
-image_3=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_3/template.jpg
*/

#include <iostream>
#include <opencv2/opencv.hpp>\"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

void MatchEnkelObject( Mat img, Mat templ );
void MatchMeerderObjecten( Mat img, Mat templ );

const char* image_window = "Source Image";
const char* result_window = "Result window";

int main(int argc,const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  | show this message }"
    "{image_1 i1       |  | (required) absolute path to first image }"
    "{image_2 i2       |  | (required) absolute path to second image }"
    "{image_3 i3       |  | (required) absolute path to third image }"
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

    //Haal de 3 image locaties
    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    string image_3_location(parser.get<string>("image_3"));
    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    //Lees de 3 foto's in
    Mat img[2], templ;
    img[0] = imread(image_1_location);
    img[1] = imread(image_2_location);
    templ = imread(image_3_location);

    if(img[0].empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(img[1].empty()){
        cerr <<"Image2 not found";
        return -1;
    }
    if(templ.empty()){
        cerr <<"Image3 not found";
        return -1;
    }

    imshow("recht",img[0]);
    imshow("rot",img[1]);
    imshow("template",templ);
    waitKey(0);
    destroyAllWindows();

    MatchEnkelObject(img[1], templ);
    MatchMeerderObjecten(img[0], templ);
    return 0;
}

void MatchEnkelObject(Mat img, Mat templ)
{
    Mat result;

    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    matchTemplate(img, templ, result, TM_SQDIFF);
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    imshow("Genormalizeerd", result);
    waitKey(0);
    destroyAllWindows();

    Mat result2;
    img.copyTo(result2);

    double minVal, maxVal;
    Point minLoc, maxLoc, matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    matchLoc = minLoc;

    rectangle(result2, matchLoc, Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows), Scalar(255,0,0), 2, 5, 0);
    rectangle(result, matchLoc, Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows), Scalar(255,0,0), 2, 5, 0);

    imshow("Genormalizeerde met rechthoek", result);
    waitKey(0);
    destroyAllWindows();

    imshow("Een enkele match", result2);
    waitKey(0);
    destroyAllWindows();
}

void MatchMeerderObjecten(Mat img, Mat templ)
{
    Mat mask, temp, result2, result;
    img.copyTo(result2);

    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    matchTemplate(img, templ, result, TM_CCOEFF_NORMED);
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    threshold(result, mask, 0.8, 1, THRESH_BINARY);
    mask.convertTo(mask, CV_8UC1);
    mask = mask*255;
    imshow("Mask", mask);
    waitKey(0);
    destroyAllWindows();

    vector<vector<Point>>  contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    double minVal, maxVal;
    Point minLoc, maxLoc, matchLoc;

    for (size_t i=0; i <contours.size(); i++)
    {
        Rect region = boundingRect(contours[i]);
        Mat temp = result(region);
        Point maxLock;
        Point minLock;
        minMaxLoc(temp, NULL, NULL, NULL, &maxLoc, Mat());
        matchLoc = maxLoc;
        rectangle(result2, Point(region.x+ matchLoc.x, region.y + matchLoc.y), Point(matchLoc.x +region.x + templ.cols, matchLoc.y +region.y + templ.rows), Scalar(255,0,0), 2, 5, 0);
     }

    imshow("Meerdere matches", result2);
    waitKey(0);
    destroyAllWindows();
}

/*
TO DO:
Voeg detectie bij gedraaide objecten toe.
=> getRotationMatrix2D, warpAffine ?
*/
