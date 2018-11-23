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

#include "opencv2/features2d/features2d.hpp"

using namespace std;
using namespace cv;

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

    //ORB
    Ptr<ORB> detectorORB = ORB::create();
    Ptr<BRISK> detectorBRISK = BRISK::create();
    Ptr<AKAZE> detectorAKAZE = AKAZE::create();

    vector<KeyPoint> keypoints_fitness_ORB, keypoints_bueno_ORB;
    detectorORB->detect(fitness[0], keypoints_fitness_ORB);
    detectorORB->detect(bueno[0], keypoints_bueno_ORB);

    vector<KeyPoint> keypoints_fitness_BRISK, keypoints_bueno_BRISK;
    detectorBRISK->detect(fitness[0], keypoints_fitness_BRISK);
    detectorBRISK->detect(bueno[0], keypoints_bueno_BRISK);

    vector<KeyPoint> keypoints_fitness_AKAZE, keypoints_bueno_AKAZE;
    detectorAKAZE->detect(fitness[0], keypoints_fitness_AKAZE);
    detectorAKAZE->detect(bueno[0], keypoints_bueno_AKAZE);

    Mat fORB, fBRISK, fAKAZE;
    Mat bORB, bBRISK, bAKAZE;
    fORB = fitness[0].clone();
    fBRISK = fitness[0].clone();
    fAKAZE = fitness[0].clone();
    bORB = bueno[0].clone();
    bBRISK = bueno[0].clone();
    bAKAZE = bueno[0].clone();

    drawKeypoints(fORB, keypoints_fitness_ORB, fORB);
    drawKeypoints(fBRISK, keypoints_fitness_BRISK, fBRISK);
    drawKeypoints(fAKAZE, keypoints_fitness_AKAZE, fAKAZE);

    drawKeypoints(bORB, keypoints_bueno_ORB, bORB);
    drawKeypoints(bBRISK, keypoints_bueno_BRISK, bBRISK);
    drawKeypoints(bAKAZE, keypoints_bueno_AKAZE, bAKAZE);

    imshow("fitness ORB",fORB);
    imshow("fitness BRISK",fBRISK);
    imshow("fitness AKAZE",fAKAZE);
    waitKey(0);
    destroyAllWindows();

    imshow("bueno ORB",bORB);
    imshow("bueno BRISK",bBRISK);
    imshow("bueno AKAZE",bAKAZE);
    waitKey(0);
    destroyAllWindows();


    Ptr<ORB> detector = ORB::create();

    std::vector<KeyPoint> keypoints_1_b, keypoints_2_b,keypoints_1_f, keypoints_2_f;
    Mat descriptors_1_b, descriptors_2_b,descriptors_1_f, descriptors_2_f;
    detector->detectAndCompute(fitness[1], Mat(), keypoints_1_f, descriptors_1_f);
    detector->detectAndCompute(fitness[0], Mat(), keypoints_2_f, descriptors_2_f);
    detector->detectAndCompute(bueno[1], Mat(), keypoints_1_b, descriptors_1_b);
    detector->detectAndCompute(bueno[0], Mat(), keypoints_2_b, descriptors_2_b);

    BFMatcher matcher(NORM_L2);
    std::vector<DMatch> matches;
    matcher.match(descriptors_1_f, descriptors_2_f, matches);
    matcher.match(descriptors_1_b, descriptors_2_b, matches);

    Mat img_matches_f, img_matches_b;
    drawMatches(fitness[1], keypoints_1_f,  fitness[0], keypoints_2_f, matches, img_matches_f);
    drawMatches(bueno[1], keypoints_1_b,  bueno[0], keypoints_2_b, matches, img_matches_b);

    imshow("Matches fitness", img_matches_f);
    imshow("Matches bueno", img_matches_b);
    waitKey(0);
    destroyAllWindows();



    return 0;
}
