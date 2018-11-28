/*Cédric Gullentops*/
/*
Build options: `pkg-config opencv --libs`
-image_1=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_4/fitness_image.png
-image_2=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_4/fitness_object.png
-image_3=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_4/kinderbueno_image.png
-image_4=/home/student/Github/2018_beeldinterpretatie_Gullentops_Cedric/sessie_4/kinderbueno_object.png
*/

#include <iostream>
#include <opencv2/opencv.hpp>\"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

void detectKeypointsORB(Mat object, Mat image);
void detectKeypointsBRISK(Mat object, Mat image);
void detectKeypointsAKAZE(Mat object, Mat image);
void KeypointMatching(string type, Mat object, Mat image, std::vector<KeyPoint> object_keypoints, std::vector<KeyPoint> image_keypoints, Mat object_descriptor, Mat image_descriptor);

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

    //Haal de 3 image locaties
    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    string image_3_location(parser.get<string>("image_3"));
    string image_4_location(parser.get<string>("image_4"));
    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty() || image_4_location.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    //Lees de 3 foto's in
    Mat fitness[2], bueno[2];
    fitness[0] = imread(image_1_location);
    fitness[1] = imread(image_2_location);
    bueno[0] = imread(image_3_location);
    bueno[1] = imread(image_4_location);

    if(fitness[0].empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(fitness[1].empty()){
        cerr <<"Image2 not found";
        return -1;
    }
    if(bueno[0].empty()){
        cerr <<"Image3 not found";
        return -1;
    }
    if(bueno[1].empty()){
        cerr <<"Image4 not found";
        return -1;
    }

    imshow("fitness image",fitness[0]);
    imshow("fitness object", fitness[1]);
    imshow("bueno image",bueno[0]);
    imshow("bueno object",bueno[1]);
    waitKey(0);
    destroyAllWindows();

    detectKeypointsORB(fitness[1].clone(), fitness[0].clone());
    detectKeypointsBRISK(fitness[1].clone(), fitness[0].clone());
    detectKeypointsAKAZE(fitness[1].clone(), fitness[0].clone());

    detectKeypointsORB(bueno[1].clone(), bueno[0].clone());
    detectKeypointsBRISK(bueno[1].clone(), bueno[0].clone());
    detectKeypointsAKAZE(bueno[1].clone(), bueno[0].clone());
}

void detectKeypointsORB(Mat object, Mat image)
{
    //create ORB detector
    Ptr<ORB> detector = ORB::create();
    std::vector<KeyPoint> object_keypoints;
    std::vector<KeyPoint> image_keypoints;
    Mat object_descriptor;
    Mat image_descriptor;

    //search keypoints and store them in given variables
    detector->detectAndCompute(object, Mat(), object_keypoints, object_descriptor);
    detector->detectAndCompute(image, Mat(), image_keypoints, image_descriptor);

    //draw the keypoints
    Mat image_result;
    Mat object_result;
    drawKeypoints(object.clone(), object_keypoints, object_result);
    drawKeypoints(image.clone(), image_keypoints, image_result);

    //show result
    imshow("ORB keypoints: object", object_result);
    imshow("ORB keypoints: image", image_result);
    waitKey(0);
    destroyAllWindows();

    KeypointMatching("ORB", object, image, object_keypoints, image_keypoints, object_descriptor, image_descriptor);
}

void detectKeypointsBRISK(Mat object, Mat image)
{
    //BRISK
    Ptr<BRISK> detector = BRISK::create();
    std::vector<KeyPoint> object_keypoints;
    std::vector<KeyPoint> image_keypoints;
    Mat object_descriptor;
    Mat image_descriptor;

    //search keypoints and store them in given variables
    detector->detectAndCompute(object, Mat(), object_keypoints, object_descriptor);
    detector->detectAndCompute(image, Mat(), image_keypoints, image_descriptor);

    //draw the keypoints
    Mat image_result;
    Mat object_result;
    drawKeypoints(object.clone(), object_keypoints, object_result);
    drawKeypoints(image.clone(), image_keypoints, image_result);

    //show result
    imshow("BRISK keypoints: object", object_result);
    imshow("BRISK keypoints: image", image_result);
    waitKey(0);
    destroyAllWindows();

    KeypointMatching("BRISK", object, image, object_keypoints, image_keypoints, object_descriptor, image_descriptor);
}

void detectKeypointsAKAZE(Mat object, Mat image)
{
    //AKAZE
    Ptr<AKAZE> detector = AKAZE::create();
    std::vector<KeyPoint> object_keypoints;
    std::vector<KeyPoint> image_keypoints;
    Mat object_descriptor;
    Mat image_descriptor;

    //search keypoints and store them in given variables
    detector->detectAndCompute(object, Mat(), object_keypoints, object_descriptor);
    detector->detectAndCompute(image, Mat(), image_keypoints, image_descriptor);

    //draw the keypoints
    Mat image_result;
    Mat object_result;
    drawKeypoints(object.clone(), object_keypoints, object_result);
    drawKeypoints(image.clone(), image_keypoints, image_result);

    //show result
    imshow("AKAZE keypoints: object", object_result);
    imshow("AKAZE keypoints: image", image_result);
    waitKey(0);
    destroyAllWindows();

    KeypointMatching("AKAZE", object, image, object_keypoints, image_keypoints, object_descriptor, image_descriptor);
}

void KeypointMatching(string type, Mat object, Mat image, std::vector<KeyPoint> object_keypoints, std::vector<KeyPoint> image_keypoints, Mat object_descriptor, Mat image_descriptor)
{
    //find matches in the found keypoints
    std::vector<DMatch> keypoint_matches;
    BFMatcher matcher(NORM_L2);
    matcher.match(object_descriptor, image_descriptor, keypoint_matches);

    //remove bad matches
    std::sort(keypoint_matches.begin(), keypoint_matches.end());
    const int n = 0.10f*keypoint_matches.size();
    keypoint_matches.erase(keypoint_matches.begin()+n, keypoint_matches.end());

    //draw matches
    Mat img_matches;
    drawMatches(object, object_keypoints, image, image_keypoints, keypoint_matches, img_matches);
    imshow(type, img_matches);
    waitKey(0);
    destroyAllWindows();

    //get location of matches
    std::vector<Point2f> object_p, image_p;
    for(size_t i=0; i<keypoint_matches.size(); i++){
        object_p.push_back(object_keypoints[keypoint_matches[i].queryIdx].pt);
        image_p.push_back(image_keypoints[keypoint_matches[i].trainIdx].pt);
    }
    Mat homography = findHomography(object_p, image_p, RANSAC);

    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = CvPoint(object.cols);
    obj_corners[2] = cvPoint(object.cols, object.rows);
    obj_corners[3] = cvPoint(0, object.rows);

    std::vector<Point2f> scene_corners(4);
    perspectiveTransform(obj_corners, scene_corners, homography);
    scene_corners[0].x += object.cols;
    scene_corners[1].x += object.cols;
    scene_corners[2].x += object.cols;
    scene_corners[3].x += object.cols;

    line(img_matches,scene_corners[0],scene_corners[1],Scalar(255,0,0),2);
    line(img_matches,scene_corners[1],scene_corners[2],Scalar(255,0,0),2);
    line(img_matches,scene_corners[2],scene_corners[3],Scalar(255,0,0),2);
    line(img_matches,scene_corners[3],scene_corners[0],Scalar(255,0,0),2);

    imshow("Gevonden object met "+type+":", img_matches);
    waitKey(0);
    destroyAllWindows();
}
