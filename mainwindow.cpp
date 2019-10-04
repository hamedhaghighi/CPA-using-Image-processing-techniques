#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initialization.cpp"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioButton_clicked()
{

}

void MainWindow::on_pushButton_clicked()
{
   u = ui;

   if(ui->radioButton->isChecked())
   {
       type = 1;
       initialization();
   }
   else if (ui->radioButton_2->isChecked())
   {
       type = 2;
       initialization();
   }


}

void MainWindow::on_pushButton_2_clicked()
{
    showBack();
}
