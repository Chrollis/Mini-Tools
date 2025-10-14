#include "mainwindow.h"
#include "arithwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->addTab(new ArithWidget, "口算");
}

MainWindow::~MainWindow()
{
    delete ui;
}
