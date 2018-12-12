/*Cédric Gullentops*/
/*
Build options: `pkg-config opencv --libs`

-video_2=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_6/people.mp4
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

vector<Point> path;

void detectAndDisplay (Mat frame){
    HOGDescriptor HOG_descriptor;
    HOG_descriptor.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    std::vector<Rect> person,found_filtered;
    std::vector<double> score;
    HOG_descriptor.detectMultiScale(frame, person, score,1.05);

    size_t i;

    for (i=0; i<person.size(); i++)
    {
        rectangle(frame, person[i], Scalar(255,0,0), 2);
        Point center( person[i].x + person[i].width/2, person[i].y + person[i].height/2 );
        putText(frame, std::to_string(score[i]), center, FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,255), 1, CV_AA);
        path.push_back(Point(person[i].x+person[i].width/2,person[i].y+person[i].height/2));
    }

    for(size_t i=1; i<path.size(); i++){
        line(frame,path[i-1], path[i], Scalar(255,0,0),2);
    }

    imshow("Person Detection",frame);
}

void detectPerson(string video, string name){
    VideoCapture capture(video);
    Mat frame;

    for(;;)
    {
        capture >> frame;
        if(frame.empty()){
            break;
        }
        detectAndDisplay(frame);
        if( waitKey(10) == 27 )
        {
            break;
        }
    }
    waitKey(0);
}

int main(int argc,const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  | show this message }"
    "{video_2 v2       |  | (required) absolute path to second video }"
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

    string person(parser.get<string>("video_2"));

    cout << "Starting video, press Escape to quit\n";
    detectPerson(person, "person");

    waitKey(0);
}
