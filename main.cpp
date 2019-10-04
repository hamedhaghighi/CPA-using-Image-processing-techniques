#include <QApplication>
#include "mainwindow.h"
#include <QStyle>
#include <QDesktopWidget>


int main( int argc, char** argv)
{

    QApplication a(argc, argv);
    a.setApplicationName("hamed");
    a.setOrganizationName("myorg");
    a.setOrganizationDomain("myorg.com");
    MainWindow w;
//    w.setGeometry(
//        QStyle::alignedRect(
//            Qt::LeftToRight,
//            Qt::AlignRight,
//            w.size(),
//            a.desktop()->availableGeometry()
//        )
//    );
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width()-w.width()) / 2;
    int y = (screenGeometry.height()-w.height()) / 2;
    w.move(x + 150, y - 50);
    w.show();
    return a.exec();
}
