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
}

MainWindow::~MainWindow()
{
    delete ui;
}


//生成初始牌面
void MainWindow::init(){
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
    writeMessageTo2(&arr[1]);
    tcpSocket_2->waitForBytesWritten();


    for(int i = 0; i < 3; i ++){
        labelLandlord[i] = new QLabel(ui->labelCard_0);
        labelLandlord[i]->setPixmap(QPixmap(QString(":/png/cards/PADDING.png")).scaled(ui->labelCard_0->width(),ui->labelCard_0->height(),Qt::KeepAspectRatio));
        labelLandlord[i]->move(60 * i, 0);
        labelLandlord[i]->show();
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
    if(tcpSocket->peerPort() == 8880){
        this->tcpSocket_1 = tcpSocket;
        connect(tcpSocket_1, SIGNAL(readyRead()), this , SLOT(recvMessageFrom1()));
    }else{
        this->tcpSocket_2 = tcpSocket;
        connect(tcpSocket_2, SIGNAL(readyRead()), this , SLOT(recvMessageFrom2()));
    }
    connectNum ++;
    if(connectNum == 2){
        ui->stackedWidget->setCurrentIndex(0);
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

    tcpSocket_1->waitForBytesWritten();
}


void MainWindow::writeMessageTo2(QByteArray *array){
     tcpSocket_2->write(array->data());
     tcpSocket_2->waitForBytesWritten();

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
         qDebug() << "ok";
         datas.clear();
         for (int i = 0; i < struct_count; i ++) {
             datas.push_back(m_buffer_car[i + 2]);
         }

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
         arr = m_buffer_car.right(total_length - total_bytes - 2);
         total_length = arr.size();
         m_buffer_car = arr;
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
         qDebug() << "ok";
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
        labels[num]->move(30 * num, 20 - 20 * player.playCards[num].choosed);
        labels[num]->show();
        num ++;
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
        labels[num]->move(30 * num, 20 - 20 * player.playCards[num].choosed);
        if(player.playCards[num].exist == 0)
            labels[num]->setVisible(0);
        num ++;
    }
}


void MainWindow::landLordNo(){
    landlord[0] = 2;
    ui->labelPlayerStatus->setText("不抢");
    QByteArray arr;
    int struct_size, total_size;
    struct_size = 3;
    arr.append(4);
    arr.append('0');
    arr.append(2);
    total_size = arr.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr[i]);
    }
    writeMessageTo1(&data);
    writeMessageTo2(&data);
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
    QByteArray arr;
    int struct_size, total_size;
    struct_size = 3;
    arr.append(4);
    arr.append('0');
    arr.append(1);
    total_size = arr.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr[i]);
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
    QByteArray arr;
    int struct_size, total_size;
    if(isLandLord == 0){
        for(int i = 0; i < 3; i ++){
            player.playCards.push_back(Card(card_0[i]));
        }
        sort(player.begin(), player.end());
        drawCard();
        struct_size = 25;
        arr.append(5);
        arr.append('0');
    }else if(isLandLord == 1){
        struct_size = 5;
        arr.append(5);
        arr.append(1);
    }else{
        struct_size = 5;
        arr.append(5);
        arr.append(2);
    }
    for(int i = 0; i < 3; i ++){
        arr.append(card_0[i]);
    }
    total_size = arr.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr[i]);
    }
    writeMessageTo1(&data);
    writeMessageTo2(&data);
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
    QByteArray arr;
    int struct_size, total_size;
    struct_size = 1;
    arr.append(6);
    total_size = arr.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    data.append(arr[0]);
    if(n == 1)
        writeMessageTo1(&data);
    else if(n == 2)
        writeMessageTo2(&data);
    else{
        ui->pushButtonOut->setVisible(1);
        ui->pushButtonNoCard->setVisible(1);
    }
}


void MainWindow::judgeCard(){

}
