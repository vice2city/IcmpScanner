#ifndef MYTRIGGER_H
#define MYTRIGGER_H

#include <QObject>

class MyTrigger : public QObject
{
    Q_OBJECT
public:
    MyTrigger();
signals:
    void newLog(char*);
    void newIP(char*);
    void scanFinish(bool);
};
#endif // MYTRIGGER_H
