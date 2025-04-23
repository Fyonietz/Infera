#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFrame>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    connect(ui->home,&QPushButton::clicked,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
        ui->home->setStyleSheet("background-color: rgb(52, 36, 91);border:none;");
        ui->an->setStyleSheet("background-color: rgb(73, 51, 127);border:none;");
    });
    connect(ui->an,&QPushButton::clicked,this,[=](){
        ui->stackedWidget->setCurrentIndex(1);
        ui->an->setStyleSheet("background-color: rgb(52, 36, 91);border:none;");
        ui->home->setStyleSheet("background-color: rgb(73, 51, 127);border:none;");
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{

}

