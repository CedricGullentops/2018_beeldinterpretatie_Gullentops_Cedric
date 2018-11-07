/*Cédric Gullentops*/

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
	"{ imageColorAdapted ica  |  | (required) path to colour image adapted }"
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
	string image_colour_adapted(parser.get<string>("imageColorAdapted"));
    if (image_colour.empty() || image_bimo.empty() || image_colour_adapted.empty()){
        cerr << "Something wrong in the given arguments" << endl;
        parser.printMessage();
        return -1;
    }

    Mat c_image;
    c_image = imread(image_colour, CV_LOAD_IMAGE_COLOR );
    Mat b_image;
    b_image = imread(image_bimo, CV_LOAD_IMAGE_COLOR );
	Mat ca_image;
	ca_image = imread(image_colour_adapted, CV_LOAD_IMAGE_COLOR);
    if (c_image.empty() || b_image.empty() || ca_image.empty()){
        cerr << "image not found" << endl;
        return -1;
    }
    resize(c_image,c_image,Size(c_image.cols/2,c_image.rows/2));
    resize(b_image,b_image,Size(b_image.cols/2,b_image.rows/2));

    //Toon de drie images.
    namedWindow("colour image", WINDOW_AUTOSIZE);
    imshow("colour image", c_image);
    namedWindow("Bimo image", WINDOW_AUTOSIZE);
    imshow("Bimo image", b_image);
	namedWindow("Colour image adapted", WINDOW_AUTOSIZE);
	imshow("Colour image adapted", ca_image);
    waitKey(0);

    //Splits de kleur foto op in 3 kanalen en laat elks van deze zien.
	vector<Mat> channels;
	split(ca, channels);
	imshow("Blauw", channels[0]);
	imshow("Groen", channels[1]);
	imshow("Rood", channels[2]);
	waitKey(0);

	//Mask aanmaken
	Mat mask = Mat::zeros(ca.rows, ca.cols, CV_8UC1);
	Mat BLUE = channels[0]; 
	Mat GREEN = channels[1]; 
	Mat RED = channels[2];
	mask = (RED > 95) & (GREEN > 40) & (BLUE > 20) & ((max(RED, max(GREEN, BLUE)) - min(RED, min(GREEN, BLUE))) > 15) &
		(abs(RED - GREEN) > 15) & (RED > GREEN) & (RED > BLUE);

	mask = mask * 255;
	imshow("Mask", mask);
	waitKey(0);

	//Erode
	erode(mask.clone(), mask, Mat(), Point(-1, -1), 2);
	imshow("Eroded", mask);
	waitKey(0);

	//Dilate
	dilate(mask.clone(), mask, Mat(), Point(-1, -1), 2);
	imshow("Dilated", mask);
	waitKey(0);

	//Verbinding van stukken
	dilate(mask.clone(), mask, Mat(), Point(-1, -1), 5);
	erode(mask.clone(), mask, Mat(), Point(-1, -1), 5);
	vector<vector<Point>> contour;
	findcontour(mask.clone(), contour, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<vector<Point>> hulls;
	for (size_t i=0; i<contour.size(); i++) {
		vector<Point> hull;
		convexHull(contour[i], hull);
		hulls.push_back(hull);
	}
	drawcontour(mask, hulls, -1, 255, -1);
	imshow("Final mask", mask);
	waitKey(0);

	//Combineren masker + originele foto
	Mat finaal(ca.rows, ca.cols, CV_8UC3);
	Mat pixels_b = BLUE & mask;
	Mat pixels_g = GREEN & mask;
	Mat pixels_r = RED & mask;
	vector<Mat> channels_mix;
	channels_mix.push_back(pixels_b);
	channels_mix.push_back(pixels_g);
	channels_mix.push_back(pixels_r);
	merge(channels_mix, finaal);
	imshow("Masker + foto gecombineerd", finaal);
	waitKey(0);

	//OTSU
	Mat greyIm;
	Mat otsuIm;
	cvtColor(b_image, greyIm, CV_RGB2GRAY);
	threshold(greyIm, otsuIm, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	imshow("ticket OTSU", otsuIm);
	waitKey(0);

	//Equalizing
	Mat greyImHist;
	equalizeHist(greyIm.clone(), greyImHist);
	imshow("ticket histogram", greyImHist);
	waitKey(0);

	//OTSU threshold
	Mat greyImHistOtsu;
	threshold(greyImHist, greyImHistOtsu, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	imshow("OTSU ticket met Histogram equalization", greyImHistOtsu);
	waitKey(0);

	//CLAHE
	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(4);
	Mat greyImClahe;
	clahe->apply(greyIm.clone(), greyImClahe);
	imshow("CLAHE ticket", greyImClahe);
	waitKey(0);

	//OTSU threshold
	Mat greyImClaheOtsu;
	threshold(greyImClahe, greyImClaheOtsu, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	imshow("OTSU ticket met CLAHE", greyImClaheOtsu);
	waitKey(0);

	return 0;
}


