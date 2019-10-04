
#include "Contours.h"
#include "preprocessing.h"
//#include "extra.h"
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <QDir>
#include "math.h"
Ui::MainWindow* u2;
#define  minArea 1000
#define Maxdistance 21
typedef std::pair<double,Point> mypair;
//int thresh = 100;
//int a = 2;
QString* fname_list = new QString();
Mat image;
Scalar color(0,0,255);
Scalar color1(100,0,100);
int* two_largest_contours(vector<vector<Point>>& contours)
{
    int index = -1,next_index=-1,j=0;
    float largest_area=-1,next_largest_area=-1;
    float* areas;
    areas = new float[contours.size()];
    for( int i = 0; i< contours.size(); i++ )
    {
        vector<Point> poly;
        convexHull(contours[i],poly,true);
        areas[i]=contourArea( poly,false);
    }
    while (((next_largest_area /largest_area) > 0.9 || next_largest_area==-1) && j <= contours.size())
    {
        for( int i = 0; i< contours.size(); i++ )
        {
            double a=areas[i];
            //approxPolyDP(contours[i],poly,2,true);
            //double a=contourArea( contours[i],false);
            bool con;
            con = j ? a > next_largest_area : a > largest_area;
            if(con)
            {
                if (j == 0)
                {
                    index = i;
                    largest_area = a;
                }
                else
                {
                    next_index = i;
                    next_largest_area = a;
                }
            }
        }
        int Indice = j ? next_index : index;
        areas[Indice] = -1;
        j++;
    }
    if ( j > contours.size())
        next_index = -1;
    int a[2]={index,next_index};
    return a;
}
QPixmap MattoPix(Mat& inMat)
{

    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_ARGB32 );

        return QPixmap::fromImage(image);
    }
    case CV_8UC3:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_RGB888 );

        return QPixmap::fromImage(image.rgbSwapped());
    }
    }
}

void throwError()
{
    QMessageBox messageBox;
    messageBox.critical(0,"Error","Bad ROI");
}
void SetmainWidow(Ui::MainWindow *u)
{
    u2 = u;
}
double maxDepth(vector<Vec4i> defects)
{
    double maxdepth = -1;
    for (int i=0; i < defects.size();i++)
    {
        if (defects[i][3]/256.0 > maxdepth)
            maxdepth = defects[i][3]/256.0;
    }
    return maxdepth;
}
vector<Point> Best(Mat& input)
{
    vector<vector<Point>> contours;
    vector<vector<Point>> saved_cnt;
    vector<double> saved_md;
    vector<Vec4i> hierarchy;
    int md_thresh = 3;
    for(int erode = 4;erode < 15 ;erode++)
    {
        Mat canny_output=preprocess_F(input,0,erode,0,false);
        findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        int* index=two_largest_contours(contours);
        int next_index = index[0];
        if (next_index != -1 )
        {
            saved_cnt.push_back(contours[next_index]);
            vector<int> ind;
            convexHull(contours[next_index],ind,true);
            vector<Vec4i> defects;
            convexityDefects(contours[next_index],ind,defects);
            double md=maxDepth(defects);
            saved_md.push_back(md);
            if (md < md_thresh )//&& hierarchy[next_index][2] > 0)
                return contours[next_index];
        }
        else if(erode == 4)
            break;
    }
    if (saved_cnt.size() > 0)
    {
        for (md_thresh = 4; md_thresh < 10;md_thresh++)
        {
            for (int i=0;i < saved_cnt.size();i++)
            {
                if (saved_md[i] < md_thresh )//&& hierarchy[next_index][2] > 0)
                    return saved_cnt[i];
            }
        }
    }
    return vector<Point>();
}
void showMat(Mat input)
{

    QPixmap p = MattoPix(input);
    u2->label->setScaledContents(true);
    u2->label->setPixmap(p);


}
bool comparator ( const mypair& l, const mypair& r)
{ return l.first < r.first; }
double vectorAngle(Vec2f a , Vec2f b)
{
    return acos(a.dot(b)/(norm(a,NORM_L2)*norm(b,NORM_L2))) * 180/CV_PI;
}


double axisAngle(Vec2f v)
{
    double t = vectorAngle(v,Vec2f(1,0));
    if(v[1] < 0)
        t=360 - t;
    return t;
}
void sortContour(vector<Point> &cnt)
{
    vector<mypair> obj;
    Moments m = moments(cnt);
    int cx= int(m.m10/m.m00);
    int cy = int(m.m01/m.m00);

    for (int i=0; i < cnt.size(); i++)
    {
        Vec2f v (cnt[i].x - cx,cy -cnt[i].y);
        obj.push_back(mypair(axisAngle(v),cnt[i]));
    }
    std::sort (obj.begin(), obj.end(), comparator);
    cnt.clear();
    for (int i=0; i < obj.size(); i++)
    {
        if( i == 0 || obj[i].second != cnt[cnt.size()-1])
        {
            cnt.push_back(Point(obj[i].second.x,obj[i].second.y));
        }
    }
}

void traceContour (vector<Point> cnt1, vector<Point> cnt2 , Mat& drawing)
{

    QString f_name = fname_list->remove(".jpg");
    QStringList parts = f_name.split("/");
    f_name = parts.at(parts.size()-1);
    cout << f_name.toStdString() << "\n";
    ofstream out(QDir::currentPath().toStdString() + "\\result\\" + f_name.toStdString() + ".txt");
    double thresh = 3;
    Moments m = moments(cnt2);
    sortContour(cnt1);
    sortContour(cnt2);
    int cx= int(m.m10/m.m00);
    //int cy = int(m.m01/m.m00);
    int cy = max(cnt1[0].y,cnt2[0].y);


    for(int i =0,j=0; i < cnt1.size() && j < cnt2.size();)
    {
        Vec2f v1 (cnt1[i].x - cx,cy -cnt1[i].y) ,v2(cnt2[j].x - cx,cy -cnt2[j].y);
        double t1 = axisAngle(v1);
        double t2 = axisAngle(v2);

        if(t1 - t2 > thresh)
            j++;
        else if(t2 - t1 > thresh)
            i++;
        else if(fabs(t1 - t2) <= thresh)
        {

            double diff = fabs(norm(v1,NORM_L2) - norm(v2,NORM_L2));
            if (diff > Maxdistance)
            {
                line(drawing,cnt1[i],cnt2[j],color,1);

            }
            i++;j++;
            out << "at "<<t1<<" degree, distance is: "<<fabs(norm(v1,NORM_L2) - norm(v2,NORM_L2))<<"\n";

        }
    }
    String str = "The results were written to \"" + f_name.toStdString() + ".txt\"";
    u2->label_2->setText(QString::fromStdString(str));

}
Mat contourfind_S(Mat input,int type)
{
    RNG rng(12345);
    Mat canny_output,filtered;
    vector<vector<Point> > contours;
    vector<vector<Point> > vec;
    vector<Vec4i> hierarchy;
    canny_output=preprocess_S(input);
    image = input;
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    Mat drawing = Mat::zeros(input.size(),CV_8UC1);
    /// Draw contours

    double area1 = 0;
    vector<Point> cnt = Best(input);
    vector<Point> poly;
    if(cnt.size() > 0)
    {
        convexHull(cnt,poly,true);
        area1=contourArea( poly,false);
    }
    if( area1 > minArea)
    {

        contourTakeapart2(cnt,input.size());
        vec.push_back(cnt);
    }

    else
        throwError();


    /// Show in a window
    return drawing;
}

vector<Point> secondBest(Mat& input)
{
    vector<vector<Point>> contours;
    vector<vector<Point>> saved_cnt;
    vector<double> saved_md;
    vector<Vec4i> hierarchy;
    int md_thresh = 3;
        for(int maxthresh=23; maxthresh < 70;maxthresh++)
        {
            for(int dilate = 7;dilate < 31;dilate++)
            {
                Mat canny_output=preprocess_F(input,dilate,4,maxthresh);
                Mat drawing = Mat::zeros( input.size(), CV_8UC3 );
                findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                int* index=two_largest_contours(contours);
                int next_index = index[1];

                if (next_index != -1 )
                {
                    drawContours( drawing, contours, next_index, Scalar(255,255,255), 0.3, 8, hierarchy, 0, Point() );
                    //namedWindow("test",WINDOW_NORMAL);
                    //imshow("test",drawing);
                    saved_cnt.push_back(contours[next_index]);
                    vector<int> ind;
                    convexHull(contours[next_index],ind,true);
                    vector<Vec4i> defects;
                    convexityDefects(contours[next_index],ind,defects);
                    double md=maxDepth(defects);
                    saved_md.push_back(md);
                    if (md < md_thresh )//&& hierarchy[next_index][2] > 0)
                        return contours[next_index];
                }
            }
        }
    for (md_thresh = 4; md_thresh < 10;md_thresh++)
    {
        for (int i=0;i < saved_cnt.size();i++)
        {
            if (saved_md[i] < md_thresh )//&& hierarchy[next_index][2] > 0)
                return saved_cnt[i];
        }
    }
    return vector<Point>();

}
Mat contourfind_F(Mat input,int type )
{
    vector<Vec4i> hierarchy;

    image = input;


    /// Find contours
    /// Draw contours
    Mat drawing;
    input.copyTo(drawing);
    //int* index =two_largest_contours(contours);
    //two_largest_contours_index = index[0];
    double area1 = 0;
    vector<Point> poly;
    vector<Point> cnt = Best(input);
    vector<vector<Point>> vec;
    if(cnt.size() > 0)
    {
        convexHull(cnt,poly,true);
        area1=contourArea( poly,false);
        cout << "\n" << area1;
    }
    if(area1 > minArea)

    {

        vector<Point> c = secondBest(input);
        convexHull(c,poly,true);
        double area2=contourArea( poly,false);
        cout << "\n" << area2;
        vec.push_back(cnt);
        if (c.size() > 0 && (area2/area1) > 0.4 && (area2/area1) < 0.9)
        {

            vec.push_back(c);
            drawContours( drawing, vec, 0, color1, 0.3, 8, hierarchy, 0, Point() );
            drawContours( drawing, vec, 1, color1, 0.3, 8, hierarchy, 0, Point() );
            traceContour(cnt,c,drawing);
            showMat(drawing);

            //namedWindow("contours",WINDOW_NORMAL);
            //imshow("contours",drawing);
        }
        else
            throwError();
    }
    else
        throwError();

    return drawing;
}

void Linefind(Mat dst)
{
    Mat color_dst =Mat::zeros( dst.size(), CV_8UC3 );
    vector<Vec4i> lines;
    vector<Vec4i> newlines;
    HoughLinesP( dst, lines, 1, CV_PI/180, 9, 0, 50 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        bool infinit = lines[i][0]-lines[i][2] == 0;
        double m=2;
        if(!infinit)
            m = fabs((double)(lines[i][1]-lines[i][3])/(lines[i][0]-lines[i][2]));
        if((atan(m) > CV_PI/4 && atan(m) < CV_PI/2) || infinit)
        {
            line( color_dst, Point(lines[i][0], lines[i][1]),
                    Point(lines[i][2], lines[i][3]), Scalar(255,255,255), 0.3, 8 );
            newlines.push_back(lines[i]);
        }
    }
    if(newlines.size() == 2)
    {
        double t1 = atan(fabs((double)(newlines[0][1]-newlines[0][3])/(newlines[0][0]-newlines[0][2])));
        double t2 = atan(fabs((double)(newlines[1][1]-newlines[1][3])/(newlines[1][0]-newlines[1][2])));
        cout << "degree between lines :"<<fabs(CV_PI - (t1 + t2)) * (180/CV_PI)<<"\n";
    }
    else
        cout <<"not good ROI\n";
    namedWindow( "Detected Lines", WINDOW_NORMAL );
    imshow( "Detected Lines", color_dst );

    //waitKey(0);
}

vector<Point> editLine_second(vector<Point> cnt,int cx , int cy,Vec2f& pre,int flag)
{
    int miny = 1000,index;
    bool f = false;
    double thresh = 70,maxthresh=15,coeff = 1.5;
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
    int a = (index + 1)%cnt.size();
    int count = 1;
    for(int l = 1;count<cnt.size() ;l++)
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
        //f = true;
        double t = vectorAngle(Vec2f(ln[0],ln[1]),pre);
        if (pre != Vec2f(0,0) && t > thresh)
        {
            line.pop_back();
            break;
        }
        else if(line[count-1] != line[count] && line[count-1].y != line[count].y)
        {

            pre = Vec2f(ln[0],ln[1]);
            thresh /=coeff;
            count++;

        }
        else
            line.pop_back();
    }
    return line;
}
void drawLine(vector<Point> l,Mat& input)
{
    int t = 20;
    Vec4f ln;
    fitLine(l,ln,CV_DIST_FAIR,0,0.01,0.01);
    if(ln[1] > 0)
    {
        ln[0]*=-1;
        ln[1]*=-1;
    }
    line(input,Point(ln[2],ln[3]),Point(ln[2]+t*ln[0],ln[3]+t*ln[1]),color,4);
}

void contourTakeapart2(vector<Point> cnt,Size s)
{
    vector<Point> linecnt1,linecnt2;
    Vec2f line1 , line2;
    sortContour(cnt);
    Moments m = moments(cnt);
    int cx= int(m.m10/m.m00),windowx =4,windowy=2,miny=1000;
    int cy = int(m.m01/m.m00);
    for(int i=0; i < cnt.size(); i++)
    {
        if( cnt[i].x <= cx)
            linecnt1.push_back(cnt[i]);
        else
            linecnt2.push_back(cnt[i]);
    }
    //Mat drawing = Mat::zeros( s, CV_8UC3 );
    Mat drawing ;
    image.copyTo(drawing);
    linecnt1=editLine_second(linecnt1,cx,cy,line1,-1);
    linecnt2=editLine_second(linecnt2,cx,cy,line2,1);
    double degree = vectorAngle(line1,line2);
    if (linecnt1.size() > 1 && linecnt1.size() > 1 && degree < 90 )
    {
        QString S("The Angle between lines is :");
        S+=QString::number(floorf(degree * 100) / 100);
        S+=" degree";
        u2->label_2->setText(S);
//        cout << "The angle between lines is : "<<roundf(degree * 100) / 100<< " degree";
        drawLine(linecnt1,drawing);
        drawLine(linecnt2,drawing);
        showMat(drawing);
        //namedWindow("win",WINDOW_NORMAL);
        //imshow("win",drawing);
    }
    else
        throwError();
}
