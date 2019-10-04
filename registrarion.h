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
static vector<Point2f> pts_src;
static vector<Point2f> pts_dst;
//static Mat image1_g;
//static Mat image2_g;
void register2image(Mat &image1,Mat &image2,Mat&image1_g,Mat&image2_g);
Mat register2image2(Mat &im1, Mat &im2);
