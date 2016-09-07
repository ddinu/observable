#include <QObject>
#include "utility.h"

class Sender : public QObject
{
    Q_OBJECT

signals:
    NOINLINE void inc1();
    NOINLINE void inc2(int);
    NOINLINE void inc3(int, int, int, double);
};

class Receiver : public QObject
{
    Q_OBJECT

public slots:
    NOINLINE void inc1() { ++dummy; }
    NOINLINE void inc2(int v) { dummy += v; }
    NOINLINE void inc3(int v1, int v2, int v3, double v4) { dummy += v1 + v2 + v3 + (int)v4; }

private:
    volatile unsigned long long dummy;
};
