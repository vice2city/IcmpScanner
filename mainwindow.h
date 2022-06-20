#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThreadPool>
#include "IcmpEcho.h"
#include "mytrigger.h"
#include "mywork.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void printLog(char *);
    void insertList(char *);
    void updateDiagram(char *);
    void updateScanState(bool);

public slots:
    void toggleButton();

private:
    Ui::MainWindow *ui;
    MyTrigger *tri;
    bool isFinished;
    MyWork *cur_work;

};
#endif // MAINWINDOW_H
