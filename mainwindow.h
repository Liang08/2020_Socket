#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();                             //初始化牌面

public slots:
    void initServer();                       //初始化连接
    void acceptConnection();                 //接受连接
    void writeMessageTo1(QByteArray *array);
    void writeMessageTo2(QByteArray *array);
    void recvMessage();


private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket_1, *tcpSocket_2;
    int connectNum = 0;
    std::vector<int> card[3];
};
#endif // MAINWINDOW_H
