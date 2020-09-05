#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include "playercard.h"
#include "card.h"
#include <QMainWindow>
#include <QtNetwork>
#include <QLabel>

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT
    QString Name;

public:
    explicit ClientWindow(QString name_0, QWidget *parent = nullptr);
    ~ClientWindow();

public slots:
    void connectHost();
    void recvMessage();   //接受来自服务器的消息
                          /*
                           *arr[0]:代表数据的类型
                           * 1：建立连接完成信号
                           * 2：发牌信号
                           * 3：开始抢地主信号
                           * 4：个体抢地主结果
                           * 5：抢地主最终结果
                           * 6：开始出牌信号
                           */
    void writeMessage(QByteArray *arr);
    void landLordNo();
    void landLordYes();
    void chooseCard(int i);
    void giveCard();
    void noCard();

private:
    Ui::ClientWindow *ui;
    PlayerCard player;
    QByteArray m_buffer_car;
    QTcpSocket *readWriteSocket;
    int isLandLord;
    int id;
    int cardCount[3] = {17, 17, 17};
    PlayerCard landLordCard;
    void drawCard();
    void drawCardAgain();
    QLabel* labels[20];
    QLabel* labelLandlord[3];

};

#endif // CLIENTWINDOW_H
