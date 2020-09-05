#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QtNetwork>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void run() override;
    QTcpServer *tcpServer;
    QTcpSocket *readWriteSocket;

signals:

public slots:
};

#endif // MYTHREAD_H
