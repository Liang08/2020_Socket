#include "clientwindow.h"
#include "ui_clientwindow.h"
#include "card.h"
#include <QDebug>
#include <algorithm>

ClientWindow::ClientWindow(QString name_0, QWidget *parent) :
    QMainWindow(parent),
    Name(name_0),
    ui(new Ui::ClientWindow)

{
    ui->setupUi(this);
    QString s = "斗地主 ";
    s += Name;
    this->setWindowTitle(s);
    ui->stackedWidget->setCurrentIndex(1);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(connectHost()));
    connect(ui->pushButtonNo, SIGNAL(clicked()), this, SLOT(landLordNo()));
    connect(ui->pushButtonYes, SIGNAL(clicked()), this, SLOT(landLordYes()));
    connect(ui->labelCard, SIGNAL(chooseCard(int)), this, SLOT(chooseCard(int)));
    connect(ui->pushButtonOut, SIGNAL(clicked()), this, SLOT(giveCard()));
    connect(ui->pushButtonNoCard, SIGNAL(clicked()), this, SLOT(noCard()));
    connect(ui->pushButtonQuit, SIGNAL(clicked()), this, SLOT(quit()));
    connect(ui->pushButtonAgain, SIGNAL(clicked()), this, SLOT(continuePlay()));
    if(Name == "Player_1"){
        id = 1;
        ui->labelPlayer1->setText("玩家2");
        ui->labelPlayer2->setText("玩家0");
    }else{
        id = 2;
        ui->labelPlayer1->setText("玩家0");
        ui->labelPlayer2->setText("玩家1");
    }

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
}


ClientWindow::~ClientWindow()
{
    delete ui;
}


void ClientWindow::connectHost(){
    this->readWriteSocket = new QTcpSocket;
    if(this->Name == "Player_1")
        readWriteSocket->bind(QHostAddress::Any, 9000);
    else
        readWriteSocket->bind(QHostAddress::Any, 10000);
    this->readWriteSocket->connectToHost(QHostAddress("127.0.0.1"),8888);
    connect(readWriteSocket, SIGNAL(readyRead()), this, SLOT(recvMessage()));

}


void ClientWindow::recvMessage(){
    if(readWriteSocket->bytesAvailable() <= 0)
        return;
     QByteArray arr = readWriteSocket->readAll();
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
         switch (int(datas[0])) {

         //init
         case 1:
             ui->stackedWidget->setCurrentIndex(0);
             ui->pushButtonNo->setVisible(0);
             ui->pushButtonYes->setVisible(0);
             ui->pushButtonNoCard->setVisible(0);
             ui->pushButtonOut->setVisible(0);
             ui->labelMessage->setText("");
             break;

         //发牌
         case 2:
             ui->stackedWidget->setCurrentIndex(0);
             ui->pushButtonNo->setVisible(0);
             ui->pushButtonYes->setVisible(0);
             ui->pushButtonNoCard->setVisible(0);
             ui->pushButtonOut->setVisible(0);
             ui->labelMessage->setText("");
             player.playCards.clear();
             for(int i = 0; i < 17; i ++){
                 player.playCards.push_back(Card(int(datas[i + 1])));
             }
             drawCard();
             for(int i = 0; i < 3; i ++){
                 labelLandlord[i] = new QLabel(ui->labelCard_0);
                 labelLandlord[i]->setPixmap(QPixmap(QString(":/png/cards/PADDING.png")).scaled(ui->labelCard_0->width(),ui->labelCard_0->height(),Qt::KeepAspectRatio));
                 labelLandlord[i]->move(60 * i, 0);
                 labelLandlord[i]->show();
             }
             break;

         //抢地主
         case 3:
             ui->pushButtonNo->setVisible(1);
             ui->pushButtonYes->setVisible(1);
             break;

         //其他人抢地主信号
         case 4:
             if(Name == "Player_1"){
                 if(datas[1] == 2){
                     if(datas[2] == 2)
                         ui->labelPlayerstatus1->setText("不抢");
                     else
                         ui->labelPlayerstatus1->setText("抢地主");
                 }else if(datas[1] == '0'){
                     if(datas[2] == 2)
                         ui->labelPlayerstatus2->setText("不抢");
                     else
                         ui->labelPlayerstatus2->setText("抢地主");
                 }
             }else{
                 if(datas[1] == '0'){
                     if(datas[2] == 2)
                         ui->labelPlayerstatus1->setText("不抢");
                     else
                         ui->labelPlayerstatus1->setText("抢地主");
                 }else if(datas[1] == 1){
                     if(datas[2] == 2)
                         ui->labelPlayerstatus2->setText("不抢");
                     else
                         ui->labelPlayerstatus2->setText("抢地主");
                 }
             }
             break;

         case 5:
         {
             if(Name == "Player_1"){
                 if(datas[1] == '0'){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("地主");
                     ui->labelPlayerCard2->setText("20");
                     isLandLord = 0;

                 }else if(datas[1] == 1){
                     ui->labelPlayerStatus->setText("地主");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 1;
                 }else{
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("地主");
                     ui->labelPlayerCard1->setText("20");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 2;
                 }
             }if(Name == "Player_2"){
                 if(datas[1] == '0'){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("地主");
                     ui->labelPlayerCard1->setText("20");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 0;
                 }else if(datas[1] == 1){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("地主");
                     ui->labelPlayerCard2->setText("20");
                     isLandLord = 1;
                 }else{
                     ui->labelPlayerStatus->setText("地主");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 2;
                 }
             }
             cardCount[isLandLord] = 20;
             if(isLandLord == id){
                 for(int i = 0; i < 3; i ++){
                     player.playCards.push_back(Card(int(datas[i + 2])));

                 }
                 sort(player.begin(), player.end());
                 drawCardAgain();
             }
             for(int i = 0; i < 3; i ++){
                 landLordCard.playCards.push_back(Card(int(datas[i + 2])));
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
             break;
         }

         case 6:
             ui->pushButtonNoCard->setVisible(1);
             ui->pushButtonOut->setVisible(1);
             break;

         case 7:
             ui->labelMessage->setText("非法出牌");
             break;

         case 8:
         {
             if(id == datas[1]){
                 ui->pushButtonOut->setVisible(0);
                 ui->pushButtonNoCard->setVisible(0);
                 ui->labelMessage->setText("");
             }
             if(struct_count != 2){
                if(id == datas[1]){
                    ui->labelMessage->setText("");
                    for (int i = 2; i < struct_count; i ++) {
                        player[datas[i]].exist = 0;
                    }
                    drawCardAgain();
                    ui->pushButtonOut->setVisible(0);
                    ui->pushButtonNoCard->setVisible(0);
                }
                outCard.playCards.clear();
                for(int i = 2; i < struct_count; i ++){
                    outCard.playCards.push_back(Card(int(datas[i])));
                }
                drawOutCard();
                if(datas[1] == '0')
                    cardCount[0] -= (struct_count - 2);
                else
                    cardCount[int(datas[1])] -= (struct_count - 2);
                if(id == 1){
                    ui->labelPlayerCard1->setNum(cardCount[2]);
                    ui->labelPlayerCard2->setNum(cardCount[0]);
                }
                else if(id == 2){
                     ui->labelPlayerCard1->setNum(cardCount[0]);
                     ui->labelPlayerCard2->setNum(cardCount[1]);
                 }
             }

             break;
         }

         case 9:
         {
             ui->stackedWidget->setCurrentIndex(2);
             if(datas[1] == id){
                 ui->labelWin->setText("You Win");
             }else if(isLandLord == id){
                 ui->labelWin->setText("You Lose");
             }else if(isLandLord == datas[1]){
                 ui->labelWin->setText("You Lose");
             }else{
                 ui->labelWin->setText("You Win");
             }
         }

         default:
             break;
         }
         QByteArray arr_2;
         int struct_size, total_size;
         struct_size = 1;
         arr_2.append('0');
         total_size = arr_2.size();
         QByteArray data_2;
         data_2.append(total_size).append(struct_size);
         for(int i = 0; i < struct_size; i ++){
             data_2.append(arr_2[i]);
         }
         writeMessage(&data_2);
         arr = m_buffer_car.right(total_length - total_bytes - 2);
         total_length = arr.size();
         m_buffer_car = arr;
     }
}


void ClientWindow::writeMessage(QByteArray *arr){
    readWriteSocket->write(arr->data());
    readWriteSocket->waitForBytesWritten();
}


void ClientWindow::drawCard(){
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


void ClientWindow::drawCardAgain(){
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



void ClientWindow::landLordNo(){
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 2;
    arr_2.append(1);
    arr_2.append(2);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    ui->labelPlayerStatus->setText("不抢");
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
    writeMessage(&data);
}


void ClientWindow::landLordYes(){
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 2;
    arr_2.append(1);
    arr_2.append(1);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessage(&data);
    ui->labelPlayerStatus->setText("抢地主");
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
}


void ClientWindow::chooseCard(int n){
    player.playCards[n].choosed = 1 - player.playCards[n].choosed;
    drawCardAgain();
}


void ClientWindow::giveCard(){
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 1;
    arr_2.append(2);
    for(auto a : player){
        if(a.choosed == 1 && a.exist == 1){
            arr_2.append(a.getNum());
            struct_size ++;
        }
    }
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessage(&data);
}


void ClientWindow::noCard(){
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 1;
    arr_2.append(2);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessage(&data);
}


void ClientWindow::drawOutCard(){
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


void ClientWindow::quit(){
    readWriteSocket->disconnectFromHost();
    ui->stackedWidget->setCurrentIndex(1);
}


void ClientWindow::continuePlay(){
    QByteArray arr_2;
    int struct_size, total_size;
    struct_size = 1;
    arr_2.append(3);
    total_size = arr_2.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr_2[i]);
    }
    writeMessage(&data);
}
