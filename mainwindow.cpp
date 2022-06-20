#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tri(new MyTrigger)
{
    ui->setupUi(this);

    connect(ui->ScanButton, &QPushButton::clicked, this, &MainWindow::toggleButton);
    connect(tri, &MyTrigger::newLog, this, &MainWindow::printLog);
    connect(tri, &MyTrigger::newIP, this, &MainWindow::updateDiagram);
    connect(tri, &MyTrigger::scanFinish, this, &MainWindow::updateScanState);

    isFinished = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printLog(char* text)
{
    QString buffer = QString(text);
    ui->LogView->append(buffer);
}

void MainWindow::updateDiagram(char* ip)
{
    QString buffer = QString(ip);

    auto ipList = ui->IPView->findItems(ip, Qt::MatchFixedString);
    if(!ipList.empty()) return;

    QListWidgetItem* item = new QListWidgetItem(buffer);
    item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setBackground(QBrush(QColor(rand()%256, rand()%256, rand()%256, 80)));
    ui->IPView->addItem(item);
}

void MainWindow::updateScanState(bool state)
{
    isFinished = state;
    if(state){
        cur_work = nullptr;
        isFinished = true;
        ui->ScanButton->setText("开始扫描");
        ui->ScanButton->setIcon(QIcon(":/new/prefix1/icon/scan.png"));
    }
}

void MainWindow::toggleButton()
{
    if(isFinished){
        MyWork *work = new MyWork;
        cur_work = work;
        work->tri = tri;
        work->startIP = ui->StartIP->displayText();
        work->endIP = ui->EndIP->displayText();
        work->start();
        isFinished = false;
        ui->ScanButton->setText("停止扫描");
        ui->ScanButton->setIcon(QIcon(":/new/prefix1/icon/cancel.png"));
    }else{
        cur_work->isFinished = true;
        cur_work->quit();
        cur_work = nullptr;
        isFinished = true;
        ui->ScanButton->setText("开始扫描");
        ui->ScanButton->setIcon(QIcon(":/new/prefix1/icon/scan.png"));
    }

}


