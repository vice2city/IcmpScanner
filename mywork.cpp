#include "mywork.h"

MyWork::MyWork(QObject *parent) : QThread(parent)
{
    isFinished = false;
}

MyWork::~MyWork()
{

}

void MyWork::run()
{
    // 业务处理代码
    IcmpEcho* icmp = new IcmpEcho;
    icmp->tri = tri;
    int s = startIP.section('.', 3, 3).toInt();
    int e = endIP.section('.', 3, 3).toInt();


    for(int i=s;i<e;i++){
        if(isFinished) break;
        std::string str = std::to_string(i);
        QString num(str.c_str());
        QString head = startIP.section('.', 0 , 2);
        QString ip = head+'.'+num;
        icmp->ping(ip.toStdString().c_str());
        QThread::usleep(1);
    }

    tri->scanFinish(true);
}
