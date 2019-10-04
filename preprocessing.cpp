#include "preprocessing.h"



void on_trackbar(int, void*);
void createTrackbars();
void showimgcontours(Mat &threshedimg,Mat &original);
void toggle(int key);
void morphit(Mat &img);
void blurthresh(Mat &img);

//function prototypes ends here

//boolean toggles

bool domorph = false;
bool doblurthresh = true;
bool showchangedframe = false;
bool equalized = false;
//boolean toggles end


int H_MIN = 10;
int H_MAX = 255;
//int H_MAX = 52;
int S_MIN = 0;
int S_MAX = 93;
int V_MIN = 208;
int V_MAX = 255;

int kerode = 4;
int kdilate = 0;
int kblur = 4;
int minthreshval = 0;
int maxthreshval = 31;
Mat equalizeIntensity(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;
        cvtColor(inputImage,ycrcb,CV_BGR2YCrCb);

        vector<Mat> channels;
        split(ycrcb,channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels,ycrcb);
        cvtColor(ycrcb,result,CV_YCrCb2BGR);

        return result;
    }

    return Mat();
}

Mat preprocess_S(Mat& src , int _erode)
{
    kerode = _erode;
    /*namedWindow("Camera",WINDOW_NORMAL);
    createTrackbars();
    on_trackbar(0, 0);*/

    Mat hsvsrc, rangesrc,teeth;
    int key;
    //src = equalizeIntensity(src);
    /*while ((key = waitKey(30)) != 'q')
    {
        toggle(key);*/

        cvtColor(src, hsvsrc, COLOR_BGR2HSV);
        inRange(hsvsrc, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), rangesrc);
        src.copyTo(teeth, rangesrc);
        cvtColor(teeth,teeth, CV_BGR2GRAY );
        if (kerode > 0)
        {
            Mat structuringElement = getStructuringElement(MORPH_ELLIPSE, Size(kerode, kerode));
            morphologyEx( teeth, teeth, MORPH_OPEN, structuringElement );
        }
        //blur(teeth, teeth, Size(kblur, kblur), Point(-1, -1), BORDER_DEFAULT);
        //equalizeHist(teeth,teeth);
        //src.copyTo(rangesrc);
        /*if (showchangedframe)
            imshow("Camera", src);
        else
            imshow("Camera", teeth);

    }*/
    return teeth;
}

void on_trackbar(int, void*)
{//This function gets called whenever a
	// trackbar position is changed
	if (kerode == 0)
		kerode = 1;
	if (kdilate == 0)
		kdilate = 1;
	if (kblur == 0)
		kblur = 1;
}

Mat preprocess_F(Mat& src,int _dilate,int _erode ,int _maxthresh,bool dothresh,int _blur,int H_min,int S_max,int V_min)
{
    kblur =_blur;kdilate = _dilate;kerode = _erode;maxthreshval=_maxthresh;H_MIN = H_min;S_MAX = S_max;V_MIN = V_min;
    doblurthresh = dothresh;
    /*namedWindow("Camera",WINDOW_NORMAL);
    createTrackbars();
    on_trackbar(0, 0);*/
    Mat hsvsrc, rangesrc,teeth;
    int key;
    /*while ((key = waitKey(30)) != 'q')
    {
        toggle(key);*/
        cvtColor(src, hsvsrc, COLOR_BGR2HSV);

        inRange(hsvsrc, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), rangesrc);
        src.copyTo(teeth, rangesrc);
        cvtColor(teeth,teeth, CV_BGR2GRAY );

        //teeth.copyTo(equalized_teeth);

        //src.copyTo(rangesrc);
        Mat structuringElement = getStructuringElement(MORPH_ELLIPSE, Size(kerode, kerode));
        morphologyEx( teeth, teeth, MORPH_OPEN, structuringElement );
        if (domorph)
            morphit(teeth);

        if (doblurthresh)
            blurthresh(teeth);
        if(equalized)
            equalizeHist(teeth,teeth);

        /*if (showchangedframe)
            imshow("Camera", src);
        else
            imshow("Camera", teeth);

    }*/
    return teeth;
}


void createTrackbars()
{
	String trackbarWindowName = "TrackBars";
	namedWindow(trackbarWindowName, WINDOW_NORMAL);
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
	createTrackbar("Erode", trackbarWindowName, &kerode, 31, on_trackbar);
	createTrackbar("Dilate", trackbarWindowName, &kdilate, 31, on_trackbar);
	createTrackbar("Blur", trackbarWindowName, &kblur, 31, on_trackbar);
	createTrackbar("minThresh", trackbarWindowName, &minthreshval, 500, on_trackbar);
	createTrackbar("maxThresh", trackbarWindowName, &maxthreshval, 1000, on_trackbar);

}

void morphit(Mat &img)
{
	erode(img, img, getStructuringElement(MORPH_RECT, Size(kerode, kerode)));
	dilate(img, img, getStructuringElement(MORPH_RECT, Size(kdilate, kdilate)));
}
void blurthresh(Mat &img)
{
	//medianBlur(img,img,kblur%2+3+kblur);
	blur(img, img, Size(kblur, kblur), Point(-1, -1), BORDER_DEFAULT);
	//cvtColor(img, img, COLOR_BGR2GRAY);
	Canny( img, img, minthreshval, maxthreshval,3);
	if (kdilate > 0)
	{
		Mat structuringElement = getStructuringElement(MORPH_ELLIPSE, Size(kdilate, kdilate));
		morphologyEx( img, img, MORPH_CLOSE, structuringElement );
	}
	//threshold(img, img, minthreshval, maxthreshval, THRESH_BINARY);
}
void toggle(int key)
{

	//toggle line start
	if (key == 'm')
		domorph = !domorph;
	if (key == 'b')
		doblurthresh = !doblurthresh;
	if (key == 'r')
		showchangedframe = !showchangedframe;
	if (key == 'e')
		equalized = !equalized;
	//toggle line end
}
