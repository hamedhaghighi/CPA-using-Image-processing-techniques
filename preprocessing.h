#pragma once
#include<iostream>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\core\core.hpp>
#include<opencv2\video\background_segm.hpp>
#include<Windows.h>


using namespace cv;
using namespace std;
Mat preprocess_S(Mat& src , int _erode = 4);
Mat preprocess_F(Mat& src,int _dilate=0,int _erode = 4,int _maxthresh=31,bool dothresh = true,int _blur=4,int H_min=10,int S_max =93,int V_min=150);
