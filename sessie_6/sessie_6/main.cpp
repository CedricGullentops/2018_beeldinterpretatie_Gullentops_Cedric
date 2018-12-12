/*Cédric Gullentops*/
/*
Build options: `pkg-config opencv --libs`
-video_1=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_6/faces.mp4
-cascade_1=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_6/haarcascade_frontalface_alt.xml
-cascade_2=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_6/lbpcascade_frontalface_improved.xml
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

CascadeClassifier HAAR_cascade;
CascadeClassifier LBP_cascade;

void detectAndDisplay (Mat frame){
    Mat lbp_clone = frame.clone();
    std::vector<Rect> haar_faces, lbp_faces;
    std::vector<int> score_haar, score_lbp;
    HAAR_cascade.detectMultiScale(frame,haar_faces,score_haar,1.05,3);
    LBP_cascade.detectMultiScale(frame,lbp_faces,score_lbp,1.05,3);

    for (size_t i = 0;i < haar_faces.size();i++)
    {
        Point center( haar_faces[i].x + haar_faces[i].width/2, haar_faces[i].y + haar_faces[i].height/2 );
        ellipse(frame, center, Size( haar_faces[i].width/2, haar_faces[i].height/2 ), 0, 0, 360, Scalar(255, 0, 0), 2);
        putText(frame, std::to_string(score_haar[i]),center,FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,0,0), 1, CV_AA);
    }

    for (size_t i = 0;i < lbp_faces.size();i++)
    {
        Point center( lbp_faces[i].x + lbp_faces[i].width/2, lbp_faces[i].y + lbp_faces[i].height/2 );
        ellipse(lbp_clone, center, Size( lbp_faces[i].width/2, lbp_faces[i].height/2 ), 0, 0, 360, Scalar(0, 0, 255), 2);
        putText(lbp_clone, std::to_string(score_haar[i]),center,FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,255), 1, CV_AA);
    }

    imshow("Face detection HAAR",frame);
    imshow("Face detection LBP",lbp_clone);
}

void detectFaces(string video, string name){
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
    "{video_1 v1       |  | (required) absolute path to first video }"
    "{cascade_1 c1     |  | (required) absolute path to haarcascade_frontalface }"
    "{cascade_2 c2     |  | (required) absolute path to lbpcascade }"
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

    //Haal de twee video's op
    string faces(parser.get<string>("video_1"));

    String haarcascade = parser.get<String>("cascade_1");
    String lbpcascade = parser.get<String>("cascade_2");

     if(!HAAR_cascade.load(haarcascade))
    {
        cout << "Error loading HAAR cascade\n";
        return -1;
    };
    if(!LBP_cascade.load(lbpcascade))
    {
        cout << "Error loading LBP cascade\n";
        return -1;
    };

    cout << "Starting video, press Escape to quit\n";
    detectFaces(faces, "faces");

    waitKey(0);
}
