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

/*ZIE https://docs.opencv.org/3.4/de/da9/tutorial_template_matching.html*/
/*TO DO: maak aantal vindingen variabel*/

using namespace std;
using namespace cv;

bool use_mask;
Mat img[2]; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

void MatchingMethod( int, void* );

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

    namedWindow( image_window, WINDOW_AUTOSIZE );
    namedWindow( result_window, WINDOW_AUTOSIZE );
    imshow("img",img[0]);
    imshow("img2",img[1]);
    imshow("img3",templ);
    waitKey(0);
    destroyAllWindows();

    MatchingMethod( 0, 0 );
    waitKey(0);
    destroyAllWindows();
    return 0;
}

void MatchingMethod( int, void* )
{
    Mat img_display;
    img[0].copyTo( img_display );

    int result_cols =  img[0].cols - templ.cols + 1;
    int result_rows = img[0].rows - templ.rows + 1;

    result.create( result_rows, result_cols, CV_32FC1 );

    matchTemplate( img[0], templ, result, TM_SQDIFF);
    //threshold(result, result, 0.8, 1., CV_THRESH_TOZERO);

    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    double threshold = 0.9;

    while(true){
        minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
        matchLoc = minLoc;
        if (maxVal>=threshold){
            rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
            rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

            floodFill(result, matchLoc, Scalar(0), 0, Scalar(.1), Scalar(0.5));
        }
        else {break;}
    }

    imshow( image_window, img_display );
    imshow( result_window, result );
    waitKey(0);
    destroyAllWindows();
    return;
}

