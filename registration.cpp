#include "registrarion.h";
void register2image(Mat &image1,Mat &image2,Mat&image1_g,Mat&image2_g)
{
	Mat h = findHomography(pts_src, pts_dst);
	Mat edge1 , edge2;
	Mat im_out;
	// Warp source image to destination based on homography
	warpPerspective(image2, im_out, h, image2.size());
	//Canny(im_out,edge1,100,250);
	//Canny(image2,edge2,100,250);
	//bitwise_or(edge1,edge2,im_out);
	image1.copyTo(image1_g);
	im_out.copyTo(image2_g);
	imshow("Warped Source Image1", image1);
	imshow("Warped Source Image2", im_out);
    Mat temp = Mat::zeros(Size(im_out.cols,im_out.rows),CV_8UC3);
	Mat res;
    addWeighted(temp,0.9,im_out,0,0.0,im_out);
    addWeighted(image1,1,im_out,0,0.0,res);
	namedWindow("Mixed",WINDOW_NORMAL);
	imshow("Mixed", res);
	//while(waitKey(30)!='q');

}


//////////////////////////////method2



Mat register2image2(Mat &im1,Mat &im2)
{
	Mat im_out,im1_gray,im2_gray;
	int minHessian = 400;

	SurfFeatureDetector detector( minHessian );
	cvtColor(im1, im1_gray, CV_BGR2GRAY);
	cvtColor(im2, im2_gray, CV_BGR2GRAY);

	vector<KeyPoint> keypoints_object, keypoints_scene;

	detector.detect( im2_gray, keypoints_object );
	detector.detect( im1_gray, keypoints_scene );

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	extractor.compute( im2_gray, keypoints_object, descriptors_object );
	extractor.compute( im1_gray, keypoints_scene, descriptors_scene );

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	vector< DMatch > matches;
	matcher.match( descriptors_object, descriptors_scene, matches );

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist );
	printf("-- Min dist : %f \n", min_dist );

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches[i].distance < 3*min_dist )
	{ good_matches.push_back( matches[i]); }
	}

	Mat img_matches;

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for( int i = 0; i < good_matches.size(); i++ )
	{
		//-- Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	Mat H = findHomography( obj, scene, CV_RANSAC );
	warpPerspective(im2, im_out, H, im2.size());
	//Canny(im_out,im_out,100,200);
	//Canny(im1,im1,100,200);
	//bitwise_or(edge1,edge2,im_out);
	//im_out.copyTo(image2_g);
    Mat temp = Mat::zeros(Size(im_out.cols,im_out.rows),CV_8UC3);
    Mat res;
    addWeighted(temp,0.5,im_out,0.5,0.0,im_out);
    addWeighted(im1,0.4,im_out,0.6,0.0,res);
//    namedWindow("Mixed",WINDOW_NORMAL);
//    moveWindow("Mixed",500,200);
//    resizeWindow("Mixed",480,600);
//    imshow("Mixed", res);

    //imshow("Warped Source Image1", im1);
    return res;


}
