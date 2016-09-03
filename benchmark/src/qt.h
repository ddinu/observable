#include <QObject>

class Sender : public QObject
{
    Q_OBJECT

signals:
    void inc();
};

class Receiver : public QObject
{
    Q_OBJECT

public slots:
    void inc() { ++dummy; }

private:
    unsigned long long dummy;
};
