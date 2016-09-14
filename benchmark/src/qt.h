#include <QObject>
#include "utility.h"

class Sender : public QObject
{
    Q_OBJECT

signals:
    NOINLINE void inc(int);
};

class Receiver : public QObject
{
    Q_OBJECT

public slots:
    NOINLINE void inc(int v) { dummy += v; }

private:
    volatile unsigned long long dummy;
};
