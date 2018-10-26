#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc, argv,
    "{ help h usage ? |  | show this message }"
    "{ imageColor ic  |  | (required) path to colour image }"
    "{ imageBimo ib  |  | (required) path to Bimodal image }"
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
    string image_bimo(parser.get<string>("imageBimo"));
    if (image_colour.empty() || image_bimo.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    Mat c_image;
    c_image = imread(image_colour, CV_LOAD_IMAGE_COLOR );
    Mat b_image;
    b_image = imread(image_bimo, CV_LOAD_IMAGE_COLOR );
    if (c_image.empty() || b_image.empty()){
        cerr << "image not found" << endl;
        return -1;
    }
    resize(c_image,c_image,Size(c_image.cols/2,c_image.rows/2));
    resize(b_image,b_image,Size(b_image.cols/2,b_image.rows/2));

    //Toon de twee images.
    namedWindow("colour image", WINDOW_AUTOSIZE);
    imshow("colour image", c_image);
    namedWindow("Bimo image", WINDOW_AUTOSIZE);
    imshow("Bimo image", b_image);
    waitKey(0);

    //Splits de kleur foto op in 3 kanalen en laat elks van deze zien.
    vector<Mat> channels;
    split(c_image,channels);

    Mat mask_1 = Mat::zeros(c_image.rows,c_image.cols,CV_8UC1);
    Mat B = channels[0]; Mat G = channels[1]; Mat R = channels[2];

    mask_1 = (R>95) & (G>40) & (B>20) & ((max(R,max(G,B)) - min(R, min(G,B)))>15) & (abs(R-G)>15) & (R>G) & (R>B);
    mask_1 = mask_1*255;

    Mat finaal(c_image.rows, c_image.cols, CV_8UC3);
    Mat pixels_blue = channels[0] & mask_1;
    Mat pixels_green = channels[1] & mask_1;
    Mat pixels_red = channels[2] & mask_1;

    Mat in[] = {pixels_blue, pixels_green, pixels_red};
    int from_to[]={0,0,1,1,2,2};
    mixChannels(in,3,&finaal,1,from_to,3);

    cerr << mask_1 << endl;

    imshow("Mat1", mask_1);

    waitKey(0);

    Mat grey_ticket;
    cvtColor(b_image, grey_ticket,COLOR_BGR2GRAY);
    imshow("grey_ticket",grey_ticket);

    waitKey(0);

    Mat maskAA;
    threshold(grey_ticket, maskAA, 0, 255, THRESH_OTSU | THRESH_BINARY);
    imshow("maskAA",maskAA);

    waitKey(0);

    Mat grey_eq;
    equalizeHist(grey_ticket.clone(),grey_eq);
    imshow("grey_eq",grey_eq);

    waitKey(0);

    Mat maskA;
    threshold(grey_eq, maskA, 0, 255, THRESH_OTSU | THRESH_BINARY);
    imshow("maskA",maskA);

    waitKey(0);

    Mat result_CLAHE;
    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15,15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(grey_ticket.clone(),result_CLAHE);
    imshow("result_CLAHE",result_CLAHE);


    //ToDo change last mask
    Mat lastmask;
    threshold(grey_eq, lastmask,0,255,THRESH_OTSU | THRESH_BINARY);
    imshow("lastmask",lastmask);

    waitKey(0);

    /*
    ToDo: dilation etc in apparte file!
        ruisonderdrukking
        functies erode, dilate,

        connecteren van blobs -> zelfde als hierboven maar grotere parameters

        vector<vector<POint>> contours
        findCOuntours(mask.clone(),contours,RETR_EXTERNAL, CHAIN_APPROX_NONE);
        vector<vector<Point>> hulls;

        for (size_t i=0; i< contours.size(); i++...
            vector<Point> hull
            convexHull(contours[i],hull)
            hulls.push_back(hull);

        drawContours(mask, hulls, -1,255,-,);
        imshow

        ...
    */

    waitKey(0);
    return 0;
}


