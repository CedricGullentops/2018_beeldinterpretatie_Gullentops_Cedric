/*Cédric Gullentops*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*To do: voeg sliders (trackbars) toe met static void on_trackbar (int, void*) {h1 = alpha_slider;} (3x keer!)*/

const int alpha_slider_max = 179;
int alpha_slider;
double alpha;

const int beta_slider_max = 359;
int beta_slider;
double beta;

const int gamma_slider_max = 359;
int gamma_slider;
double gamma;

void on_trackbar1( int, void* )
{
    alpha = (double) alpha_slider/alpha_slider_max ;
    addWeighted( , alpha, src2, beta, 0.0, dst);
}

void on_trackbar2( int, void* )
{
    beta = (double) beta_slider/beta_slider_max ;
    addWeighted( , beta, src2, beta, 0.0, dst);
}

void on_trackbar3( int, void* )
{
    gamma = (double) gamma_slider/gamma_slider_max ;
    addWeighted( , gamma, src2, beta, 0.0, dst);
}

int main(int argc, const char** argv)
{
    //Parser voor events te tonen zoals errors.
    CommandLineParser parser(argc, argv,
    "{ help h usage ? |  | show this message }"
    "{ sign1 sn1  |  | (required) path to first sign image }"
    "{ sign2 sn2  |  | (required) path to second sign image }"
    "{ sign3 sn3  |  | (required) path to third sign image }"
    "{ sign4 sn4  |  | (required) path to fourth sign image }"
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
    string s1(parser.get<string>("sn1"));
    string s2(parser.get<string>("sn2"));
    string s3(parser.get<string>("sn3"));
    string s4(parser.get<string>("sn4"));
    if (s1.empty() || s2.empty() || s3.empty() || s4.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    Mat sn1;
    sn1 = imread(s1, CV_LOAD_IMAGE_COLOR );
    Mat sn2;
    sn2 = imread(s2, CV_LOAD_IMAGE_COLOR );
    Mat sn3;
    sn3 = imread(s3, CV_LOAD_IMAGE_COLOR );
    Mat sn4;
    sn4 = imread(s4, CV_LOAD_IMAGE_COLOR );
    if (sn1.empty() || sn2.empty() || sn3.empty() || sn4.empty()){
        cerr << "image not found" << endl;
        return -1;
    }
    //resize(c_image,c_image,Size(c_image.cols/2,c_image.rows/2)); if nescesarry

    //Toon de drie images.
    namedWindow("sn1", WINDOW_AUTOSIZE);
    imshow("sn1", sn1);
    namedWindow("sn2", WINDOW_AUTOSIZE);
    imshow("sn2", sn2);
    namedWindow("sn3", WINDOW_AUTOSIZE);
    imshow("sn3", sn3);
    namedWindow("sn4", WINDOW_AUTOSIZE);
    imshow("sn4", sn4);
    waitKey(0);

    //Splits de kleur foto op in 3 kanalen en laat elks van deze zien.
	vector<Mat> channels;
	split(sn1, channels);
	imshow("Blauw", channels[0]);
	imshow("Groen", channels[1]);
	imshow("Rood", channels[2]);
	waitKey(0);

	Mat Rood_th,Groen_th,Blauw_th;
	threshold(Rood,Rood_th,150,255,THRESH_BINARY);

    imshow("R th", Rood_th);
    waitKey(0);

	Mat tmp1, tmp2, tmp3;
	tmp1 = Rood & Rood_th;
	tmp2 = Groen & Groen_th;
	tmp3 = Rood & Rood_th;

	Mat finaal(tmp1.rows, tmp1.colsm CV_8UC1);
    Mat in[] = {tmp1, tmp2, tmp3};
    int from_to[] = {0,0,1,1,2,2};
    mixChannels(in,3, &finaal,1,from_to,3);

    imshow("masked image", finaal);
    waitKey(0);

    alpha_slider = 0;
    namedWindow("Linear Blend", 1);
    createTrackbar( "trackbar1", "Linear Blend", &alpha_slider, 179, on_trackbar1 );
    createTrackbar( "trackbar2", "Linear Blend", &alpha_slider, 255, on_trackbar2 );
    createTrackbar( "trackbar3", "Linear Blend", &alpha_slider, 255, on_trackbar3 );
    /*in while loop*/

    while(1){
        Mat img_hsv;
        cvtColor(sn1, img_hsv, CV_BGR2HSV);
        Mat channelsHSV[3];
        split(img_hsv,channelsHSV);
        Mat H = channelsHSV[0];
        Mat S = channelsHSV[1];
        Mat V = channelsHSV[2];

        //imshows..

        Mat h_dest1, h_dest2, h_dest3;
        inRange(H,h1,180,h_dest1);
        inRange(H,0,h2,h_dest2);
        inRange(S,s1,255,s_dest);

        H = (h_dest1 | h_dest2) & s_dest;
        //imshow..

        erode(H,H, Mat(), Point(-1,-1) 1);
        dilate(H,H, Mat(), Point(-1,-1) 3);

        //imshow..
        //Mat canvas
    }
}
