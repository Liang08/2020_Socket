#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QtNetwork>

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

public slots:
    void connectHost();
    void recvMessage();   //接受来自服务器的消息
                          /*
                           *arr[0]:代表数据的类型
                           * 1：建立连接完成信号
                           * 2：发牌信号
                           */
    void writeMessage();

private:
    Ui::ClientWindow *ui;
    QTcpSocket *readWriteSocket;
};

#endif // CLIENTWINDOW_H
