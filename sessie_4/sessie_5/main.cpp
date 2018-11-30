/*Cédric Gullentops*/
/*
Build options: `pkg-config opencv --libs`
-image_1=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_5/strawberry1.tif
-image_2=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_5/strawberry2.tif
*/

#include <iostream>
#include <opencv2/opencv.hpp>\"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
using namespace std;
using namespace cv;


Mat strawberry[2],img_copy[2],mask[2];
vector<Point> goodPoints;
vector<Point> badPoints;

static void onMouse( int event, int x, int y, int, void* )
{
    Point point = Point(x,y);
    if( event == EVENT_LBUTTONDOWN ){
        cout << "Left clicked @ x: "<<x<<"y: "<<y << endl;
        goodPoints.push_back(point);
    }
    else if(event == EVENT_RBUTTONDOWN ){
        cout << "Right clicked" << endl;
        if (goodPoints.size()!=0){
            goodPoints.pop_back();
        }
        else{
            cout << "List is already empty" << endl;
        }
    }
    else if(event == EVENT_MBUTTONDOWN ){
         cout << goodPoints << endl;
    }
    else return;

    img_copy[0] = strawberry[0].clone();
    img_copy[1] = strawberry[1].clone();

    for (int i=0; i<goodPoints.size(); i++){
        circle(img_copy[0],goodPoints[i],20, Scalar(255,0,0), 1, LINE_8, 0 );
    }
    imshow("first strawberry image", img_copy[0]);
}

int main(int argc,const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  | show this message }"
    "{image_1 i1       |  | (required) absolute path to first image }"
    "{image_2 i2       |  | (required) absolute path to second image }"
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
    //Haal de 2 image locaties
    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));

    if(image_1_location.empty() || image_2_location.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }
    //Lees de 2 foto's in
    strawberry[0] = imread(image_1_location);
    strawberry[1] = imread(image_2_location);

    if(strawberry[0].empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(strawberry[1].empty()){
        cerr <<"Image2 not found";
        return -1;
    }

    imshow("strawberry1 image",strawberry[0]);
    imshow("strawberry2 image",strawberry[1]);

    waitKey(0);
    destroyAllWindows();

    namedWindow( "first strawberry image", 0 );
    imshow("first strawberry image",strawberry[0]);
    setMouseCallback( "first strawberry image", onMouse, 0 );

    for(;;){
        char c = (char)waitKey(0);
        if( c == 27 )
        {
            cout << "Exiting ...\n";
            break;
        }
        /*
        switch( c ){
            case 'c':
                if( isColor )
                {
                    cout << "Grayscale mode is set\n";
                    cvtColor(image0, gray, COLOR_BGR2GRAY);
                    mask = Scalar::all(0);
                    isColor = false;
                }
                else
                {
                    cout << "Color mode is set\n";
                    image0.copyTo(image);
                    mask = Scalar::all(0);
                    isColor = true;
                }
                break;
            case 'm':
                break;
        }*/
     }

     /*
     Mat img_hsv;
     cvtColor(strawberry[0], img_hsv, COLOR_BGR2HSV);

     Mat tdForeground(strawberry[0].size(), 3, CV_32FC1);
     Mat labels_Foreground= Mat::ones(strawberry[0].size(), 1, CV_32SC1);

     for (int i=0; i<strawberry[0].size(); i++){
        Vec3b descriptor = img_hsv.at<Vec3b>(strawberry[i].y, strawberry[i].y);
        tdForeground.at<float>(i,0) = descriptor[0];
        tdForeground.at<float>(i,1) = descriptor[1];
        tdForeground.at<float>(i,2) = descriptor[2];
     }

     Mat tdBackground(strawberry[0].size(), 3, CV_32FC1);
     Mat labels_Background= Mat::ones(strawberry[0].size(), 1, CV_32SC1);

     for (int i=0; i<strawberry[0].size(); i++){
        Vec3b descriptor = img_hsv.at<Vec3b>(strawberry[i].y, strawberry[i].y);
        tdBackground.at<float>(i,0) = descriptor[0];
        tdBackground.at<float>(i,1) = descriptor[1];
        tdBackground.at<float>(i,2) = descriptor[2];
     }

     */
     return 0;
}
