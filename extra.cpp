#include "Contours.h"
#include "preprocessing.h"
#include <stdlib.h>
int thresh = 100;
int a = 2;

bool isColorinNeighbour(Mat ROI,int x, int y)
{
	int H_MIN = 166,H_MAX = 181,window=3;
	int b= y- (window/2) ,t=y+ (window/2),l = x- (window/2) ,r = x + (window/2);
	if(b >= 0  && r <= ROI.cols && l>=0 && t <= ROI.rows)
	{
		for(int i=b;i <=t;i++)
			for(int j=l;j <=r;j++)
				if(ROI.at<Vec3b>(i,j)[0] >= H_MIN && ROI.at<Vec3b>(i,j)[0] <= H_MAX)
					return true;
	}
	else
		return true;
	return false;
}
Mat allContours(Mat input,int type )
{
	RNG rng(12345);
	Mat canny_output,filtered;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int largest_area=-1,next_largest_area=-1;
	int largest_contour_index=-1,next_largest_index=-1;
	/// Detect edges using canny

	cvtColor( input, input, CV_BGR2GRAY );
	equalizeHist(input,input);
	bilateralFilter(input,filtered,9,75,75);
	input = filtered;
	//blur( input, input, Size(3,3));
	Canny( input, canny_output, thresh, a*thresh,3);
	//threshold(input, canny_output, 128, 255, CV_THRESH_BINARY);
	/// Find contours
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	/// Draw contours
	Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	for( int i = 0; i< contours.size(); i++ )
	{
		vector<Point> poly;
		convexHull(contours[i],poly,true);
		Scalar color = Scalar( 255, 255, 255 );
		drawContours( drawing, contours, i, color, 0.3, 8, hierarchy, 0, Point() );
		//polylines(drawing,poly,true,color,0.3,8);

	}

	/// Show in a window
	return drawing;
}
vector<Point> contourRemove(Mat ROI,vector<Point> contour)
{
	vector<Point> newcontour;
	Mat hsv;
	cvtColor(ROI, hsv, COLOR_BGR2HSV);
	for (int i=0;i<contour.size();i++)
	{
		if (!isColorinNeighbour(hsv,contour[i].x,contour[i].y))
			newcontour.push_back(contour[i]);
	}
	return newcontour;
}

void contourTakeapart(vector<Point> cnt,Size s)
{
	vector<Point> linecnt1,linecnt2;
	Vec4f line1 , line2;
	Moments m = moments(cnt);
	int cx= int(m.m10/m.m00),windowx =4,windowy=2,miny=1000;
	int cy = int(m.m01/m.m00);

	for(int j=0;j < cnt.size(); j++)
	{
		if(fabs((double)(cnt[j].x - cx)) <= windowx )
		{
			miny=min(miny,cnt[j].y);
			break;
		}
	}
	if(miny ==1000)
	{
		int maxy=0;
		for(int i=0; i < cnt.size(); i++)
			maxy = max(maxy,cnt[i].y);
		for(int i=0; i < cnt.size(); i++)
		{
			if( cnt[i].x <= cx && cnt[i].y < maxy - windowy)
				linecnt1.push_back(cnt[i]);
			else if( cnt[i].x > cx && cnt[i].y < maxy - windowy)
				linecnt2.push_back(cnt[i]);
		}
	}
	else
	{
		for(int i=0; i < cnt.size(); i++)
		{
			if(cnt[i].y < (miny -2 ) && cnt[i].x <= cx)
				linecnt1.push_back(cnt[i]);
			else if(cnt[i].y < (miny-2 )&& cnt[i].x > cx)
				linecnt2.push_back(cnt[i]);
		}
	}
	Mat drawing = Mat::zeros( s, CV_8UC3 );
	for(int i=0 ; i < linecnt1.size(); i++)
		circle(drawing,linecnt1[i],1,Scalar(255,255,255),0.3,8);
	for(int i=0 ; i < linecnt2.size(); i++)
		circle(drawing,linecnt2[i],1,Scalar(255,255,255),0.3,8);
	fitLine(linecnt1,line1,CV_DIST_FAIR,0,0.01,0.01);
	fitLine(linecnt2,line2,CV_DIST_FAIR,0,0.01,0.01);
	double t1 = atan(fabs(line1[1]/line1[0]));
	double t2 = atan(fabs(line2[1]/line2[0]));
	namedWindow("win",WINDOW_NORMAL);
	imshow("win",drawing);
	cout << "degree between lines :"<<(CV_PI - (t1 + t2)) * (180/CV_PI)<<"\n";
}
/*void grad()
{
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y,grad;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( input, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(equalized, grad_x, CV_16S, 1, 0, 3, 1,0, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );

	/// Gradient Y
	//Scharr( input, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(equalized, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );

	/// Total Gradient (approximate)
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
	//namedWindow("gradient",WINDOW_NORMAL);
	//imshow("gradient",grad);
}*/
/*vector<Point> editLine(vector<Point> cnt,int cx , int cy,Vec2f& pre)
{
	int miny = 1000,index,flag = 1;
	bool f = false;
	double thresh = 70,maxthresh=15,coeff = 2;
	vector<Point> line;
	pre = Vec2f(0,0);
	for(int i=0; i < cnt.size(); i++)
	{
		if(fabs((double)(cnt[i].y - cy)) < miny)
		{
			miny= fabs((double)(cnt[i].y - cy));
			index = i;
		}
	}
	line.push_back(cnt[index]);
	bool flag_b=true , flag_f=true;
	int count = 0;
	for(int l = 1;count<cnt.size() && (flag_b || flag_f) ;flag *= -1)
	{
		if((flag==-1 && flag_b) || (flag==1 && flag_f))
		{
			Vec4f ln;
			int ind=index + flag * l; 
			if ( ind < 0) ind += cnt.size();
			if (ind >= cnt.size()) ind -= cnt.size();
			line.push_back(cnt[ind]);
			fitLine(line,ln,CV_DIST_FAIR,0,0.01,0.01);
			ln[1] *=-1;
			if(ln[1] < 0)
			{
				ln[0]*=-1;
				ln[1]*=-1;
			}
			//if(!f && atan(fabs(ln[1]/ln[0]))> CV_PI/8)
				f = true;
			double t = vectorAngle(Vec2f(ln[0],ln[1]),pre);
			if (pre != Vec2f(0,0) && f && t > thresh)
			{
				line.pop_back();
				if (flag==-1) flag_b = false;
				else flag_f = false;
			}
			else 
			{
				if(f)
					pre = Vec2f(ln[0],ln[1]);
				
				thresh /=coeff;
				if(flag_b && flag_f && count%2==0)
					thresh *=coeff;
				count++;
				
			}
		}
		l =(flag==-1) ? l+1 : l;
	}
		

	return line;

}*/
