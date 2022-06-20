#ifndef MYWORK_H
#define MYWORK_H

#include <QObject>
#include <QThread>
#include "IcmpEcho.h"

class MyWork :public QThread
{
    Q_OBJECT
public:
    explicit MyWork(QObject *parent = nullptr);
    ~MyWork();
    MyTrigger *tri;
    QString startIP;
    QString endIP;
    bool isFinished;

protected:
    void run();

};

#endif // MYWORK_H
