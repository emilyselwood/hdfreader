#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Widget *widget = new Widget(this);
    this->setCentralWidget(widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
