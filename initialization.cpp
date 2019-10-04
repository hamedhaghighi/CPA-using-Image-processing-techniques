#include <QFileDialog>
#include <QDebug>
#include "registrarion.h"
#include "Contours.h"
#include <QSettings>

Ui::MainWindow* u;
//vector<Point2f> pts_src;
//vector<Point2f> pts_dst;
Rect roi;
int flag=0,lbuttonflag=0,lbuttonflag0=0;
Mat image1_g;
Mat image2_g;
double ratio;
int type=1;
void showBack()
{   if(image2_g.cols > 0)
        imshow("Warped Source Image2", image2_g);
}

void throwMsg(QString msg)
{
    QMessageBox messageBox;
    messageBox.warning(0 ,"Warning" , msg);
}
void mouseHandler4(int event, int x, int y, int flags, void* param)
{
    switch(event)
    {
    case CV_EVENT_LBUTTONDOWN:
        roi.x=x;
        roi.y=y;
        lbuttonflag =1;
        break;
    case CV_EVENT_MOUSEMOVE:
        if(lbuttonflag)
        {
            Mat temp;
            image2_g.copyTo(temp);
            rectangle(temp,Point(roi.x,roi.y),Point(x,y),Scalar(255,255,255),2,8);

            imshow("Warped Source Image2", temp);
        }
        break;
    case CV_EVENT_LBUTTONUP:
        lbuttonflag = 0;
        roi.width=x-roi.x;
        roi.height=y-roi.y;
        if (roi.width > 0 && roi.height > 0)
        {
            Mat roiimage2 = image2_g(roi);
            Mat drawing1;
            if (type == 1)
                drawing1=contourfind_S(roiimage2,type);
            else
                drawing1=contourfind_F(roiimage2,type);

            imshow("Warped Source Image2", roiimage2);

        }
        else
        {
            throwError();
            imshow("Warped Source Image2", image2_g);
        }
        break;
    }
}
class QFileDialogTester : public QWidget
{
public:
    QStringList openFile()
    {
        const QString DEFAULT_DIR_KEY("default_dir");
        QSettings MySettings;
        QStringList SelectedFiles = QFileDialog::getOpenFileNames(
                    this,
                    "Select two images",
                    MySettings.value(DEFAULT_DIR_KEY).toString(),
                    "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

        if (SelectedFiles.size() > 1) {
            QDir CurrentDir;
            MySettings.setValue(DEFAULT_DIR_KEY,
                                CurrentDir.absoluteFilePath(SelectedFiles[0]));

        }
        return SelectedFiles;
    }
};


void initialization()
{

    Mat imaget_1,imaget_2;
    int mouseParam= CV_EVENT_FLAG_LBUTTON;
    u->label_2->setText(QString("Please Select two images."));
    SetmainWidow(u);

    QFileDialogTester test;
    QStringList SelectedFiles=test.openFile();
    if (SelectedFiles.size() > 0) {

//        (*fname_list) = SelectedFiles[1];
        imaget_2=imread(SelectedFiles[1].toStdString());
//        imaget_2=imread(SelectedFiles[1].toStdString());

        //        ratio = (double)imaget_1.cols/(double)imaget_1.rows;
        //        Size size(1366,cvRound((double)1366/ratio));
        //        resize(imaget_1,imaget_1,size);
        //        resize(imaget_2,imaget_2,size);
//        Mat res=register2image2(imaget_1,imaget_2);
//        imaget_1.copyTo(image1_g);
        imaget_2.copyTo(image2_g);

        namedWindow("Warped Source Image2",WINDOW_NORMAL);
        cvSetMouseCallback("Warped Source Image2",mouseHandler4,&mouseParam);
        moveWindow("Warped Source Image2",0,0);
        resizeWindow("Warped Source Image2",480,600);
        imshow("Warped Source Image2", imaget_2);
        u->label_2->setText(QString("Registration Done"));
//        showMat(res);
    }
    else
        throwMsg("Please Select two images");
    return;
}






