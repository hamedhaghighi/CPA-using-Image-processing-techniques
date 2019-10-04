#pragma once
#include "opencv2/opencv.hpp" 
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
using namespace std;
using namespace cv;


Mat allContours(Mat input,int type );
void removeBackground(Mat& src);
vector<Point> contourRemove(Mat ROI,vector<Point> contour);
void contourTakeapart(vector<Point> cnt,Size s);