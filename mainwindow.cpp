#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playercard.h"
#include "card.h"
#include <QtNetwork>
#include <QDebug>
#include <vector>
#include <QTime>
#include <algorithm>
#include <QPixmap>
#include <QThread>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("斗地主 Player_0");
    ui->stackedWidget->setCurrentIndex(1);
    ui->labelPlayer1->adjustSize();
    ui->labelPlayer2->adjustSize();

    tcpServer = nullptr;

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(initServer()));
    connect(ui->pushButtonNo, SIGNAL(clicked()), this, SLOT(landLordNo()));
    connect(ui->pushButtonYes, SIGNAL(clicked()), this, SLOT(landLordYes()));
    connect(ui->labelCard, SIGNAL(chooseCard(int)), this, SLOT(chooseCard(int)));
    connect(ui->pushButtonOut, SIGNAL(clicked()), this, SLOT(giveOutCard()));
    connect(ui->pushButtonNoCard, SIGNAL(clicked()), this, SLOT(noCard()));
    connect(ui->pushButtonQuit, SIGNAL(clicked()), this, SLOT(quit()));
    connect(ui->pushButtonAgain, SIGNAL(clicked()), this, SLOT(continuePlay()));

    QString color, value;
    int num = 0;
    int w = ui->label->width();
    int h = ui->label->height();
    for(; num < 20;){
        labelOut[num] = new QLabel(ui->label);
        labelOut[num]->setPixmap(QPixmap(QString(":/png/cards/PADDING.png")).scaled(w,h,Qt::KeepAspectRatio));
        labelOut[num]->move(20 * num, 0);
        labelOut[num]->hide();
        num ++;
    }
    for(int i = 0; i < 3; i ++){
        labelLandlord[i] = new QLabel(ui->labelCard_0);
        labelLandlord[i]->setPixmap(QPixmap(QString(":/png/cards/PADDING.png")).scaled(ui->labelCard_0->width(),ui->labelCard_0->height(),Qt::KeepAspectRatio));
        labelLandlord[i]->move(60 * i, 0);
        labelLandlord[i]->show();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


//生成初始牌面
void MainWindow::init(){
    ui->stackedWidget->setCurrentIndex(0);
    memset(cardsClass, 0, sizeof (cardsClass));
    memset(landlord, 0, sizeof (landlord));
    memset(cardcount, 0, sizeof (cardcount));
    for(int i = 0; i < 20; i ++){
        labelOut[i]->hide();
    }
    card_0.clear();
    for(int i = 0; i < 3; i ++){
        card[i].clear();
    }
    for(int i = 0; i < 54; i ++){
        card_0.push_back(i);
    }
    QTime time;
    for(int k =0; k < 3; k ++){
        for(int i = 0; i < 17; i ++){
            time= QTime::currentTime();
            qsrand(time.msec()+time.second()*1000);
            int n = qrand() % (card_0.size());
            card[k].push_back(card_0[n]);
            auto iter = card_0.begin();
            iter += n;
            card_0.erase(iter);
        }
    }
    for(int i = 0; i < 3; i ++){
        std::sort(card[i].begin(), card[i].end());
    }
    qDebug() << "complete";
    ui->pushButtonNoCard->setVisible(0);
    ui->pushButtonOut->setVisible(0);
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
    player.playCards.clear();
    for(int i = 0; i < 17; i ++){
        player.playCards.push_back(Card(card[0][i]));
    }
    drawCard();
    QByteArray arr[2];
    for(int i = 0; i < 2; i++){
        arr[i].clear();
        int struct_size, total_size;
        struct_size = 18;
        QByteArray array;
        array.append(2);
        for(int j = 0; j < 17; j ++){
                array.append(card[i + 1][j]);
        }
        total_size = array.size();
        arr[i].append(total_size).append(struct_size);
        for(int k = 0; k < struct_size; k ++){
            arr[i].append(array[k]);
        }
    }
    writeMessageTo1(&arr[0]);
    tcpSocket_1->waitForBytesWritten();
    QThread::msleep(100);
    writeMessageTo2(&arr[1]);
    tcpSocket_2->waitForBytesWritten();
    QThread::msleep(100);


    for(int i = 0; i < 3; i ++){
        labelLandlord[i]->setPixmap(QPixmap(QString(":/png/cards/PADDING.png")).scaled(ui->labelCard_0->width(),ui->labelCard_0->height(),Qt::KeepAspectRatio));
    }
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int n = qrand() % 3;
    seizingLandlords(n);
}


//抢地主
void MainWindow::seizingLandlords(int n){
    int struct_size, total_size;
    QByteArray arr;
    switch (n % 3) {
    case 0:
        ui->pushButtonNo->setVisible(1);
        ui->pushButtonYes->setVisible(1);
        break;
    case 1:
        struct_size = 1;
        total_size = 1;
        arr.append(struct_size).append(total_size).append(3);
        writeMessageTo1(&arr);
        break;
    default:
        struct_size = 1;
        total_size = 1;
        arr.append(struct_size).append(total_size).append(3);
        writeMessageTo2(&arr);
        break;
    }
}


void MainWindow::initServer(){
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 8888);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}


void MainWindow::acceptConnection(){
    tcpSocket = this->tcpServer->nextPendingConnection();
    if(tcpSocket->peerPort() == 9000){
        this->tcpSocket_1 = tcpSocket;
        connect(tcpSocket_1, SIGNAL(readyRead()), this , SLOT(recvMessageFrom1()));
    }else{
        this->tcpSocket_2 = tcpSocket;
        connect(tcpSocket_2, SIGNAL(readyRead()), this , SLOT(recvMessageFrom2()));
    }
    connectNum ++;
    if(connectNum == 2){
//        QByteArray arr;
//        int struct_size, total_size;
//        struct_size = 1;
//        arr.append(1);
//        total_size = arr.size();
//        QByteArray data;
//        data.append(total_size).append(struct_size);
//        data.append(arr[0]);
//        writeMessageTo1(&data);
//        writeMessageTo2(&data);
        init();
    }
}



void MainWindow::writeMessageTo1(QByteArray *array){
    tcpSocket_1->write(array->data());
    QThread::msleep(100);
    tcpSocket_1->waitForBytesWritten();
}


void MainWindow::writeMessageTo2(QByteArray *array){
     tcpSocket_2->write(array->data());
     QThread::msleep(100);
     tcpSocket_1->waitForBytesWritten();
}


void MainWindow::recvMessageFrom1(){
    if(tcpSocket_1->bytesAvailable() <= 0)
        return;
     QByteArray arr = tcpSocket_1->readAll();
     m_buffer_car.append(arr);
     int struct_count;
     int total_bytes;
     std::vector<int> datas;

     int total_length = m_buffer_car.size();
     while(total_length){
         total_bytes = m_buffer_car[0];
         struct_count = m_buffer_car[1];
         if(total_length < 2)
             break;
         if(total_length < total_bytes + 2)
             break;
         datas.clear();
         for (int i = 0; i < struct_count; i ++) {
             datas.push_back(m_buffer_car[i + 2]);
         }
         arr = m_buffer_car.right(total_length - total_bytes - 2);
         total_length = arr.size();
         m_buffer_car = arr;
         if(datas[0] == 1){
             if(datas[1] == 2){
                 landlord[1] = 2;
                 ui->labelPlayerstatus1->setText("不抢");
             }else{
                 landlord[1] = 1;
                 ui->labelPlayerstatus1->setText("抢地主");
             }
             QByteArray arr_2;
             int struct_size, total_size;
             struct_size = 3;
             arr_2.append(4);
             arr_2.append(1);
             arr_2.append(datas[1]);
             total_size = arr_2.size();
             QByteArray data;
             data.append(total_size).append(struct_size);
             for(int i = 0; i < struct_size; i ++){
                 data.append(arr_2[i]);
             }
             writeMessageTo2(&data);
             if(landlord[2] == 0){
                 seizingLandlords(2);
             }else{
                 if(landlord[1] == 1){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("地主");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 1;

                 }else if(landlord[0] == 1){
                     ui->labelPlayerStatus->setText("地主");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 0;
                 }else{
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("地主");
                     isLandLord = 2;
                 }
                 decidedLandlord();
             }
             break;
         }
         else if(datas[0] == 2){
             int flag = 1;
             if(struct_count == 1){
                 if(cardsClass[0] == 0)
                     flag = 0;
             }
             else{
                 receivedCards.clear();
                 receivedValue.clear();
                 for (int i = 1; i < struct_count; i ++) {
                     receivedCards.push_back(int(datas[i]));
                     receivedValue.push_back(checkAmount(datas[i]));
                 }
                 flag = judgeCard(receivedValue, receivedValue.size());
             }
             if(flag == 0){
                 QByteArray arr_2;
                 int struct_size, total_size;
                 struct_size = 1;
                 arr_2.append(7);
                 total_size = arr_2.size();
                 QByteArray data;
                 data.append(total_size).append(struct_size);
                 data.append(arr_2[0]);
                 writeMessageTo1(&data);
             }else if(struct_count != 1){
                 cardcount[1] -= receivedCards.size();
                 if(cardcount[1] == 0){
                     win(1);
                 }
                 ui->labelPlayerCard1->setNum(cardcount[1]);
                 outCard.playCards.clear();
                 for(auto a : receivedCards){
                     outCard.playCards.push_back(Card(a));
                 }
                 drawOutCard();
                 QByteArray arr_2;
                 int struct_size, total_size;
                 struct_size = 2;
                 arr_2.append(8);
                 arr_2.append(1);
                 for(auto a : receivedCards){
                     arr_2.append(a);
                     struct_size ++;
                 }
                 total_size = arr_2.size();
                 QByteArray data;
                 data.append(total_size).append(struct_size);
                 for(auto a : arr_2){
                     data.append(a);
                 }
                 writeMessageTo1(&data);
                 writeMessageTo2(&data);
                 lastGiven = 1;
                 giveCard(2);
             }
             else{
                 QByteArray arr_2;
                 int struct_size, total_size;
                 struct_size = 2;
                 arr_2.append(8);
                 arr_2.append(1);
                 total_size = arr_2.size();
                 QByteArray data;
                 data.append(total_size).append(struct_size);
                 for(auto a : arr_2){
                     data.append(a);
                 }
                 writeMessageTo1(&data);
                 writeMessageTo2(&data);
                 if(lastGiven == 2){
                     cardsClass[0] = cardsClass[1] = cardsClass[2] = 0;
                 }
                 giveCard(2);
             }
         }
         else if(datas[0] == 3){
             playAgain ++;
             if(playAgain == 3){
                 init();
             }
         }
     }
}


void MainWindow::recvMessageFrom2(){
    if(tcpSocket_2->bytesAvailable() <= 0)
        return;
     QByteArray arr = tcpSocket_2->readAll();
     m_buffer_car.append(arr);
     int struct_count;
     int total_bytes;
     std::vector<int> datas;

     int total_length = m_buffer_car.size();
     while(total_length){
         total_bytes = m_buffer_car[0];
         struct_count = m_buffer_car[1];
         if(total_length < 2)
             break;
         if(total_length < total_bytes + 2)
             break;
         datas.clear();
         for (int i = 0; i < struct_count; i ++) {
             datas.push_back(m_buffer_car[i + 2]);
         }
         arr = m_buffer_car.right(total_length - total_bytes - 2);
         total_length = arr.size();
         m_buffer_car = arr;
         if(datas[0] == 1){
             if(datas[1] == 2){
                 landlord[2] = 2;
                 ui->labelPlayerstatus1->setText("不抢");
             }else{
                 landlord[2] = 1;
                 ui->labelPlayerstatus1->setText("抢地主");
             }
            QByteArray arr_2;
            int struct_size, total_size;
            struct_size = 3;
            arr_2.append(4);
            arr_2.append(2);
            arr_2.append(datas[1]);
            total_size = arr_2.size();
            QByteArray data;
            data.append(total_size).append(struct_size);
            for(int i = 0; i < struct_size; i ++){
                data.append(arr_2[i]);
            }
            writeMessageTo1(&data);
            if(landlord[0] == 0){
                seizingLandlords(0);
            }else{
                if(landlord[2] == 1){
                    ui->labelPlayerStatus->setText("农民");
                    ui->labelPlayerstatus1->setText("农民");
                    ui->labelPlayerstatus2->setText("地主");
                    isLandLord = 2;
                }else if(landlord[1] == 1){
                    ui->labelPlayerStatus->setText("农民");
                    ui->labelPlayerstatus1->setText("地主");
                    ui->labelPlayerstatus2->setText("农民");
                    isLandLord = 1;
                }else{
                    ui->labelPlayerStatus->setText("地主");
                    ui->labelPlayerstatus1->setText("农民");
                    ui->labelPlayerstatus2->setText("农民");
                    isLandLord = 0;
                }
                decidedLandlord();
            }
            break;
        }

         else if(datas[0] == 2){
             int flag = 1;
             if(struct_count == 1){
                 if(cardsClass[0] == 0)
                     flag = 0;
             }
             else{
                 receivedCards.clear();
                 receivedValue.clear();
                 for (int i = 1; i < struct_count; i ++) {
                     receivedCards.push_back(int(datas[i]));
                     receivedValue.push_back(checkAmount(datas[i]));
                 }
                 flag = judgeCard(receivedValue, receivedValue.size());
             }
             if(flag == 0){
                 QByteArray arr_2;
                 int struct_size, total_size;
                 struct_size = 1;
                 arr_2.append(7);
                 total_size = arr_2.size();
                 QByteArray data;
                 data.append(total_size).append(struct_size);
                 data.append(arr_2[0]);
                 writeMessageTo2(&data);
             }else if(struct_count != 1){
                 cardcount[2] -= receivedCards.size();
                 if(cardcount[2] == 0){
                     win(2);
                 }
                 ui->labelPlayerCard2->setNum(cardcount[2]);
                 outCard.playCards.clear();
                 for(auto a : receivedCards){
                     outCard.playCards.push_back(Card(a));
                 }
                 drawOutCard();
                 QByteArray arr_2;
                 int struct_size, total_size;
                 struct_size = 2;
                 arr_2.append(8);
                 arr_2.append(2);
                 for(auto a : receivedCards){
                     arr_2.append(a);
                     struct_size ++;
                 }
                 total_size = arr_2.size();
                 QByteArray data;
                 data.append(total_size).append(struct_size);
                 for(auto a : arr_2){
                     data.append(a);
                 }
                 writeMessageTo1(&data);
                 writeMessageTo2(&data);
                 lastGiven = 2;
                 giveCard(0);
             }
             else{
                 QByteArray arr_2;
                 int struct_size, total_size;
                 struct_size = 2;
                 arr_2.append(8);
                 arr_2.append(2);
                 total_size = arr_2.size();
                 QByteArray data;
                 data.append(total_size).append(struct_size);
                 for(auto a : arr_2){
                     data.append(a);
                 }
                 writeMessageTo1(&data);
                 writeMessageTo2(&data);
                 if(lastGiven == 0){
                     cardsClass[0] = cardsClass[1] = cardsClass[2] = 0;
                 }
                 giveCard(0);
             }
         }
         else if(datas[0] == 3){
             playAgain ++;
             if(playAgain == 3){
                 init();
             }
         }
     }
}


void MainWindow::drawCard(){
    QString color, value;
    int num = 0;
    int w = ui->labelCard->width();
    int h = ui->labelCard->height();
    for(auto a : player){
        if(a.getValue() <= 7){
            value = QString::number(a.getValue() + 3);
        }else{
            switch (a.getValue()){
            case 8:
                value = "J";
                break;
            case 9:
                value = "Q";
                break;
            case 10:
                value = "K";
                break;
            case 11:
                value = "1";
                break;
            case 12:
                value = "2";
                break;
            case 13:
                color = "BLACK";
                value = " JOKER";
                break;
            default:
                color = "RED";
                value = " JOKER";
                break;
            }
        }
        if (a.getValue() < 13){
            switch (a.getColor()) {
            case 0:
                color = "C";
                break;
            case 1:
                color = "D";
                break;
            case 2:
                color = "H";
                break;
            default:
                color = "S";
                break;
            }
        }
        labels[num] = new QLabel(ui->labelCard);
        labels[num]->setPixmap(QPixmap(QString(":/png/cards/%1%2.png").arg(color).arg(value)).scaled(w,h-60,Qt::KeepAspectRatio));
        labels[num]->move(30 * num, 20);
        labels[num]->show();
        num ++;
    }
    for (; num < 20; num ++) {
        labels[num] = new QLabel(ui->labelCard);
        labels[num]->setPixmap(QPixmap(QString(":/png/cards/PADDING.png")).scaled(w,h-60,Qt::KeepAspectRatio));
        labels[num]->move(30 * num, 20);
        labels[num]->hide();
    }
}


void MainWindow::drawCardAgain(){
    QString color, value;
    int num = 0;
    int w = ui->labelCard->width();
    int h = ui->labelCard->height();
    for(auto a : player){
        if(a.getValue() <= 7){
            value = QString::number(a.getValue() + 3);
        }else{
            switch (a.getValue()){
            case 8:
                value = "J";
                break;
            case 9:
                value = "Q";
                break;
            case 10:
                value = "K";
                break;
            case 11:
                value = "1";
                break;
            case 12:
                value = "2";
                break;
            case 13:
                color = "BLACK";
                value = " JOKER";
                break;
            default:
                color = "RED";
                value = " JOKER";
                break;
            }
        }
        if (a.getValue() < 13){
            switch (a.getColor()) {
            case 0:
                color = "C";
                break;
            case 1:
                color = "D";
                break;
            case 2:
                color = "H";
                break;
            default:
                color = "S";
                break;
            }
        }
        labels[num]->hide();
        labels[num]->setPixmap(QPixmap(QString(":/png/cards/%1%2.png").arg(color).arg(value)).scaled(w,h-60,Qt::KeepAspectRatio));
        labels[num]->move(30 * num, 20);
        labels[num]->move(30 * num, 20 - 20 * player.playCards[num].choosed);
        if(player.playCards[num].exist == 0)
            labels[num]->hide();
        else
            labels[num]->show();
        num ++;
    }
}

void MainWindow::landLordNo(){
    landlord[0] = 2;
    ui->labelPlayerStatus->setText("不抢");
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 3;
    arr_2.append(4);
    arr_2.append('0');
    arr_2.append(2);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessageTo1(&data);
    QThread::msleep(50);
    writeMessageTo2(&data);
    QThread::msleep(50);
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
    if(landlord[1] == 0){
        seizingLandlords(1);
    }if(landlord[1] == 0){
        seizingLandlords(1);
    }else{
        if(landlord[0] == 1){
            ui->labelPlayerStatus->setText("地主");
            ui->labelPlayerstatus1->setText("农民");
            ui->labelPlayerstatus2->setText("农民");
            isLandLord = 0;

        }else if(landlord[2] == 1){
            ui->labelPlayerStatus->setText("农民");
            ui->labelPlayerstatus1->setText("农民");
            ui->labelPlayerstatus2->setText("地主");
            ui->labelPlayerCard2->setText("20");
            isLandLord = 2;
        }else{
            ui->labelPlayerStatus->setText("农民");
            ui->labelPlayerstatus1->setText("地主");
            ui->labelPlayerCard1->setText("20");
            ui->labelPlayerstatus2->setText("农民");
            isLandLord = 1;
        }
        decidedLandlord();
    }
}


void MainWindow::landLordYes(){
    landlord[0] = 1;
    ui->labelPlayerStatus->setText("抢地主");
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 3;
    arr_2.append(4);
    arr_2.append('0');
    arr_2.append(1);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessageTo1(&data);
    writeMessageTo2(&data);
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
    if(landlord[1] == 0){
        seizingLandlords(1);
    }else{
        if(landlord[0] == 1){
            ui->labelPlayerStatus->setText("地主");
            ui->labelPlayerstatus1->setText("农民");
            ui->labelPlayerstatus2->setText("农民");
            isLandLord = 0;

        }else if(landlord[2] == 1){
            ui->labelPlayerStatus->setText("农民");
            ui->labelPlayerstatus1->setText("农民");
            ui->labelPlayerstatus2->setText("地主");
            ui->labelPlayerCard2->setText("20");
            isLandLord = 2;
        }else{
            ui->labelPlayerStatus->setText("农民");
            ui->labelPlayerstatus1->setText("地主");
            ui->labelPlayerCard1->setText("20");
            ui->labelPlayerstatus2->setText("农民");
            isLandLord = 1;
        }
        decidedLandlord();
    }
}


void MainWindow::decidedLandlord(){
    for(int i = 0; i < 3; i ++){
        landLordCard.playCards.push_back(Card(card_0[i]));
    }
    QString color, value;
    int num = 0;
    int w = ui->labelCard_0->width();
    int h = ui->labelCard_0->height();
    for(auto a : landLordCard){
        if(a.getValue() <= 7){
            value = QString::number(a.getValue() + 3);
        }else{
            switch (a.getValue()){
            case 8:
                value = "J";
                break;
            case 9:
                value = "Q";
                break;
            case 10:
                value = "K";
                break;
            case 11:
                value = "1";
                break;
            case 12:
                value = "2";
                break;
            case 13:
                color = "BLACK";
                value = " JOKER";
                break;
            default:
                color = "RED";
                value = " JOKER";
                break;
            }
        }
        if (a.getValue() < 13){
            switch (a.getColor()) {
            case 0:
                color = "C";
                break;
            case 1:
                color = "D";
                break;
            case 2:
                color = "H";
                break;
            default:
                color = "S";
                break;
            }
        }
        labelLandlord[num]->setPixmap(QPixmap(QString(":/png/cards/%1%2.png").arg(color).arg(value)).scaled(w,h,Qt::KeepAspectRatio));
        num ++;
    }
    cardcount[isLandLord] = 20;
    ui->labelPlayerCard1->setNum(cardcount[1]);
    ui->labelPlayerCard2->setNum(cardcount[2]);
    QByteArray arr_2;
    int struct_size, total_size;
    if(isLandLord == 0){
        for(int i = 0; i < 3; i ++){
            player.playCards.push_back(Card(card_0[i]));
        }
        sort(player.begin(), player.end());
        drawCardAgain();
        struct_size = 25;
        arr_2.append(5);
        arr_2.append('0');
        cardcount[0] = 20;
    }else if(isLandLord == 1){
        struct_size = 5;
        arr_2.append(5);
        arr_2.append(1);
        cardcount[1] = 20;
    }else{
        struct_size = 5;
        arr_2.append(5);
        arr_2.append(2);
        cardcount[2] = 20;
    }
    for(int i = 0; i < 3; i ++){
        arr_2.append(card_0[i]);
    }
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessageTo1(&data);
    QThread::msleep(20);
    writeMessageTo2(&data);
    QThread::msleep(20);
    gameStart();
}


void MainWindow::chooseCard(int n){
    player.playCards[n].choosed = 1 - player.playCards[n].choosed;
    drawCardAgain();
}


void MainWindow::gameStart(){
    giveCard(isLandLord);
}


void MainWindow::giveCard(int n){
    if(lastGiven == n){
        memset(cardsClass, 0, sizeof(cardsClass));
    }
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 1;
    arr_2.append(6);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    data.append(arr_2[0]);
    if(n == 1)
        writeMessageTo1(&data);
    else if(n == 2)
        writeMessageTo2(&data);
    else{
        ui->pushButtonOut->setVisible(1);
        ui->pushButtonNoCard->setVisible(1);
    }
}


int MainWindow::judgeCard(std::vector<int>card, int num){
    int n[3];
    if(num == 1){
        n[0] = 1;
        n[1] = 0;
        n[2] = card[0];
    }else if(num == 2 && card[0] == 13 && card[1] == 14){
        n[0] = 4;
        n[1] = 0;
        n[2] = 13;
    }
    else if(card[0] == card[num - 1]){
        if(num == 2){
            n[0] = 2;
            n[1] = 0;
            n[2] = card[0];
        }else if(num == 3){
            n[0] = 3;
            n[1] = 0;
            n[2] = card[0];
        }else if(num == 4){
            n[0] = 4;
            n[1] = 0;
            n[2] = card[0];
        }
    }
    else if(num == 4 && (card[0] == card[2] || card[1] == card[3])){
        n[0] = 3;
        n[1] = 1;
        if(card[0] == card[2])
            n[2] = card[0];
        else
            n[2] = card[1];
    }
    else{
        std::vector<int> a[4];
        int count[15];
        memset(count, 0, sizeof(count));
        for(int i = 0; i < num; i ++){
            count[card[i]] ++;
        }
        for(int i = 0; i < 15; i ++){
            if(count[i] > 0){
                a[count[i] - 1].push_back(i);
            }
        }
        if(num == 5 && a[2].size() == 1 && a[1].size() == 1){
            n[0] = 3;
            n[1] = 0;
            n[2] = a[2][1];
        }
        else if(num == 6 && a[3].size() == 1){
            if(a[0].size() == 2){
                n[0] = 4;
                n[1] = 1;
                n[2] = a[3][0];
            }else if(a[1].size() == 1){
                n[0] = 4;
                n[1] = 2;
                n[2] = a[3][0];
            }
        }
        else if(num == 8 && a[3].size() == 1 && a[1].size() == 2){
            n[0] = 4;
            n[1] = 3;
            n[2] = a[3][0];
        }
        else if(num >= 5 && a[0].size() == num && a[0][num - 1] < 12 && a[0][num - 1] - *a[0].begin()== num - 1){
            n[0] = 5;
            n[1] = num;
            n[2] = a[0][0];
        }
        else if(num >= 6 && num % 2 == 0 && a[1].size() == num / 2 && a[1][num / 2 - 1] < 12 && a[1][num / 2 - 1] - *a[1].begin()  == num / 2 - 1){
            n[0] = 6;
            n[1] = num / 2;
            n[2] = a[1][0];
        }
        else if(num >= 6 && num % 3 == 0 && a[2].size() == num / 3 && a[2][num / 3 - 1] < 12 && a[2][num / 3 - 1] - a[2][0] == num / 3 - 1){
            n[0] = 7;
            n[1] = num / 3;
            n[2] = a[2][0];
        }
        else if(num >= 8 && num % 4 == 0 && a[2].size() == num / 4 && a[2][num / 4 - 1] < 12 && a[2][num / 4 - 1] - a[2][0] == num / 4 - 1){
            n[0] = 8;
            n[1] = num / 4;
            n[2] = a[2][0];
        }
        else if(num >= 10 && num % 5 == 0 && a[2].size() == num / 5 && a[1].size() == num / 5 && a[2][num / 5 - 1] < 12 && a[2][num / 5 - 1] - a[2][0] == num / 5 - 1){
            n[0] = 9;
            n[1] = num / 5;
            n[2] = a[2][0];
        }
        else{
            n[0] = 10;
            n[1] = 0;
            n[2] = 0;
        }
    }
    qDebug() << n[0] << ' ' << n[1] << ' ' << n[2];
    if((n[0] == cardsClass[0] && n[1] == cardsClass[1] && n[2] > cardsClass[2]) || (cardsClass[0] == 0 && cardsClass[1] == 0 && cardsClass[2] == 0 && n[0] != 10) || (n[0] == 4 && n[1] == 0 && (cardsClass[0] != 4 || cardsClass[1] != 0 || cardsClass[2] < n[2]))){
        cardsClass[0] = n[0];
        cardsClass[1] = n[1];
        cardsClass[2] = n[2];
        return 1;
    }
    else
        return 0;
}


int MainWindow::checkAmount(int card){
    if(card < 53){
        return card / 4;
    }
    else
        return 14;
}


void MainWindow::drawOutCard(){
    QString color, value;
    int num = 0;
    for(int i = 0; i < 20; i ++){
        labelOut[i]->hide();
    }
    int w = ui->label->width();
    int h = ui->label->height();
    for(auto a : outCard){
        if(a.getValue() <= 7){
            value = QString::number(a.getValue() + 3);
        }else{
            switch (a.getValue()){
            case 8:
                value = "J";
                break;
            case 9:
                value = "Q";
                break;
            case 10:
                value = "K";
                break;
            case 11:
                value = "1";
                break;
            case 12:
                value = "2";
                break;
            case 13:
                color = "BLACK";
                value = " JOKER";
                break;
            default:
                color = "RED";
                value = " JOKER";
                break;
            }
        }
        if (a.getValue() < 13){
            switch (a.getColor()) {
            case 0:
                color = "C";
                break;
            case 1:
                color = "D";
                break;
            case 2:
                color = "H";
                break;
            default:
                color = "S";
                break;
            }
        }
        labelOut[num]->setPixmap(QPixmap(QString(":/png/cards/%1%2.png").arg(color).arg(value)).scaled(w,h,Qt::KeepAspectRatio));
        labelOut[num]->move(20 * num, 0);
        labelOut[num]->show();
        num ++;
    }
}


void MainWindow::giveOutCard(){
    std::vector<int> out, value;
    for(auto a : player){
        if(a.choosed == 1 && a.exist == 1){
            out.push_back(a.getNum());
        }
    }
    for(auto a : out){
        value.push_back(checkAmount(a));
    }
    int flag = judgeCard(value, value.size());
    if(flag == 0){
        ui->labelMessage->setText("非法出牌");
        giveCard(0);
    }
    else{
        cardcount[0] -= out.size();
        if(cardcount[0] == 0){
            win(0);
        }
        outCard.playCards.clear();
        for(auto a : out){
            outCard.playCards.push_back(Card(a));
            player[a].exist = 0;
        }
        drawCardAgain();
        drawOutCard();
        QByteArray arr_2;
        int struct_size, total_size;
        struct_size = 2;
        arr_2.append(8);
        arr_2.append('0');
        for(auto a : out){
            arr_2.append(a);
            struct_size ++;
        }
        total_size = arr_2.size();
        QByteArray data;
        data.append(total_size).append(struct_size);
        for(auto a : arr_2){
            data.append(a);
        }
        writeMessageTo1(&data);
        writeMessageTo2(&data);
        ui->pushButtonOut->setVisible(0);
        ui->pushButtonNoCard->setVisible(0);
        lastGiven = 0;
        giveCard(1);
    }
}


void MainWindow::noCard(){
    if(cardsClass[0] == 0)
        ui->labelMessage->setText("非法出牌");
    else{
        QByteArray arr_2;
        int struct_size, total_size;
        struct_size = 2;
        arr_2.append(8);
        arr_2.append('0');
        total_size = arr_2.size();
        QByteArray data;
        data.append(total_size).append(struct_size);
        for(auto a : arr_2){
            data.append(a);
        }
        writeMessageTo1(&data);
        writeMessageTo2(&data);
        ui->pushButtonOut->setVisible(0);
        ui->pushButtonNoCard->setVisible(0);
        if(lastGiven == 1){
            cardsClass[0] = cardsClass[1] = cardsClass[2] = 0;
        }
        giveCard(1);
    }
}


void MainWindow::win(int n){
    ui->stackedWidget->setCurrentIndex(2);
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 2;
    arr_2.append(9);
    arr_2.append(n);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(auto a : arr_2){
        data.append(a);
    }
    writeMessageTo1(&data);
    writeMessageTo2(&data);
    if(n == 0){
        ui->labelWin->setText("You Win");
    }else if(isLandLord == 0){
        ui->labelWin->setText("You Lose");
    }else if(isLandLord == n){
        ui->labelWin->setText("You Lose");
    }else{
        ui->labelWin->setText("You Win");
    }
}


void MainWindow::quit(){
    tcpSocket_1->disconnectFromHost();
    tcpSocket_2->disconnectFromHost();
}


void MainWindow::continuePlay(){
    playAgain ++;
    if(playAgain == 3){
        init();
    }
}
