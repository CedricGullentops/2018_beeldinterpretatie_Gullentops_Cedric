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
using namespace cv::ml;

Mat strawberry[2],img_copy[2],mask[2];
bool good=true;
vector<Point> goodPoints;
vector<Point> badPoints;

static void onMouse( int event, int x, int y, int, void* )
{
    Point point = Point(x,y);
    if( event == EVENT_LBUTTONDOWN ){
        cout << "Left clicked @ x: "<<x<<"y: "<<y << endl;
        if (good == true){
            goodPoints.push_back(point);
        }
        else{
            badPoints.push_back(point);
        }
    }
    else if(event == EVENT_RBUTTONDOWN ){
        cout << "Right clicked" << endl;
        if (good == true){
            if (goodPoints.size()!=0){
                goodPoints.pop_back();
            }
            else{
                cout << "List is already empty" << endl;
            }
        }
        else{
            if (badPoints.size()!=0){
                badPoints.pop_back();
            }
            else{
                cout << "List is already empty" << endl;
            }
        }
    }
    else if(event == EVENT_MBUTTONDOWN ){
            if (good == true){
                 cout << goodPoints << endl;
            }
            else{
                 cout << badPoints << endl;
            }
    }
    else return;

    img_copy[0] = strawberry[0].clone();
    img_copy[1] = strawberry[1].clone();

    for (int i=0; i<badPoints.size(); i++){
        circle(img_copy[0],badPoints[i],8, Scalar(0,0,255), 1, LINE_8, 0 );
    }

    for (int i=0; i<goodPoints.size(); i++){
        circle(img_copy[0],goodPoints[i],8, Scalar(0,255,0), 1, LINE_8, 0 );
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

    namedWindow("first strawberry image", 0 );
    imshow("first strawberry image",strawberry[0]);
    setMouseCallback("first strawberry image", onMouse, 0 );

    for(;;){
        char c = (char)waitKey(0);
        if( c == 27 )
        {
            cout << "exit\n";
            break;
        }

        switch( c ){
            case 's':
                if (good == true){
                    cout << "switch naar achtergrond\n";
                    good = false;
                }
                else{
                    cout << "switch naar voorgrond\n";
                    good = true;
                }
                break;
        }
    }
    destroyAllWindows();

    Mat gaussian;
    GaussianBlur(strawberry[0],gaussian,Size(13,13),0,0);
    Mat hsv;
    cvtColor(gaussian,hsv,CV_BGR2HSV);

    Mat trainingData(goodPoints.size(), 3, CV_32FC1);
    Mat labels_fg = Mat::ones(goodPoints.size(), 1, CV_32SC1);

    for (int i=0; i < goodPoints.size(); i++){
        Vec3b descriptor = hsv.at<Vec3b>(goodPoints[i].y, goodPoints[i].x);
        trainingData.at<float>(i,0) = descriptor[0];
        trainingData.at<float>(i,1) = descriptor[1];
        trainingData.at<float>(i,2) = descriptor[2];
    }

    Mat trainingDataBck(badPoints.size(), 3, CV_32FC1);
    Mat labels_bck = Mat::zeros(badPoints.size(), 1, CV_32SC1);

    for (int i=0; i < badPoints.size(); i++){
        Vec3b descriptor = hsv.at<Vec3b>(badPoints[i].y, badPoints[i].x);
        trainingDataBck.at<float>(i,0) = descriptor[0];
        trainingDataBck.at<float>(i,1) = descriptor[1];
        trainingDataBck.at<float>(i,2) = descriptor[2];
    }

    Mat data, labels;
    vconcat(trainingData,trainingDataBck,data);
    vconcat(labels_fg,labels_bck,labels);

    Ptr<KNearest> knearest = KNearest::create();
    Ptr<TrainData> dataKnearest = TrainData::create(data, ROW_SAMPLE, labels);
    knearest->setIsClassifier(true);
    knearest->setAlgorithmType(KNearest::BRUTE_FORCE);
    knearest->setDefaultK(3);
    knearest->train(dataKnearest);

    Ptr<NormalBayesClassifier> bayes = NormalBayesClassifier::create();
    bayes->train(data, ROW_SAMPLE, labels);

    Ptr<SVM> supVecMach = SVM::create();
    supVecMach->setType(SVM::C_SVC);
    supVecMach->setKernel(SVM::LINEAR);
    supVecMach->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    supVecMach->train(data, ROW_SAMPLE, labels);

    Mat labelsKnearest, labelsBayes, labelsSvm;
    Mat maskKnearest = Mat::zeros(strawberry[0].rows, strawberry[0].cols, CV_8UC1);
    Mat maskBayes = Mat::zeros(strawberry[0].rows, strawberry[0].cols, CV_8UC1);
    Mat maskSvm = Mat::zeros(strawberry[0].rows, strawberry[0].cols, CV_8UC1);

    for(int i=0; i < strawberry[0].rows; i++){
        for(int j=0; j < strawberry[0].cols; j++){
            Vec3b cpixel = hsv.at<Vec3b>(i,j);
            Mat data_test(1,3,CV_32FC1);
            data_test.at<float>(0,0) = cpixel[0];
            data_test.at<float>(0,1) = cpixel[1];
            data_test.at<float>(0,2) = cpixel[2];

            knearest->findNearest(data_test, knearest->getDefaultK(), labelsKnearest);
            bayes->predict(data_test, labelsBayes);
            supVecMach->predict(data_test,labelsSvm);

            maskKnearest.at<uchar>(i,j) = labelsKnearest.at<float>(0,0);
            maskBayes.at<uchar>(i,j) = labelsBayes.at<uchar>(0,0);
            maskSvm.at<uchar>(i,j) = labelsSvm.at<float>(0,0);
        }
    }

    imshow("segmentatie knearest", maskKnearest*255);
    imshow("segmentatie Normal Bayes", maskBayes*255);
    imshow("segmentatie Support Vector Machine", maskSvm*255);
    waitKey(0);
    destroyAllWindows();

    Mat resKnearest, resBayes, resSvm;
    bitwise_and(strawberry[0], strawberry[0], resKnearest, maskKnearest);
    bitwise_and(strawberry[0], strawberry[0], resBayes, maskBayes);
    bitwise_and(strawberry[0], strawberry[0], resSvm, maskSvm);

    imshow("Resultaat Knearest", resKnearest);
    imshow("Resultaat Normal Bayes", resBayes);
    imshow("Resultaat Support Vector Machine", resSvm);
    waitKey(0);
    destroyAllWindows();

    return 0;
}
