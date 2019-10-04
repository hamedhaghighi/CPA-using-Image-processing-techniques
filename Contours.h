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
#include<QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;
using namespace cv;
extern QString* fname_list;
QPixmap MattoPix(Mat &inMat);
void throwError();
void SetmainWidow(Ui::MainWindow *u);
void showMat(Mat input);
Mat contourfind_S(Mat input, int type );
Mat contourfind_F(Mat input, int type );
void Linefind(Mat src);
void contourTakeapart2(vector<Point> cnt,Size s);
