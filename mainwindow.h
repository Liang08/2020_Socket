#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playercard.h"
#include "card.h"
#include <QMainWindow>
#include <QtNetwork>
#include <vector>
#include <QLabel>

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
    void recvMessageFrom1();
    void recvMessageFrom2();
    void landLordNo();
    void landLordYes();
    void chooseCard(int n);


private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket_1, *tcpSocket_2, *tcpSocket;
    int connectNum = 0;                      //连接的客户端数量
    PlayerCard player;
    QLabel* labels[20];
    QByteArray m_buffer_car;
    int isLandLord;
    PlayerCard landLordCard;
    QLabel* labelLandlord[3];
    std::vector<int> card[3];                //card[i]  player_i的牌    
    std::vector<int> card_0;                 //地主牌
    void drawCard();
    void drawCardAgain();
    void gameStart();
    void giveCard(int n);
    void judgeCard(int *n, int num);
    void seizingLandlords(int n);
    void decidedLandlord();
    int cardcount[3] = {17, 17, 17};
    int landlord[3] = {0, 0, 0};

};
#endif // MAINWINDOW_H
