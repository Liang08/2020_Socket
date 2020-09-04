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
    if(Name == "Player_1"){
        id = 1;
        ui->labelPlayer1->setText("玩家2");
        ui->labelPlayer2->setText("玩家0");
    }else{
        id = 2;
        ui->labelPlayer1->setText("玩家0");
        ui->labelPlayer2->setText("玩家1");
    }
}


ClientWindow::~ClientWindow()
{
    delete ui;
}


void ClientWindow::connectHost(){
    this->readWriteSocket = new QTcpSocket;
    if(this->Name == "Player_1")
        readWriteSocket->bind(QHostAddress::Any, 8880);
    else
        readWriteSocket->bind(QHostAddress::Any, 8881);
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
         qDebug() << "ok";
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
             for(int i = 0; i < 17; i ++){
                 player.playCards.push_back(Card(int(datas[i + 1])));
                 qDebug() << int(datas[i+1]);
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
             if(Name == "Player_1"){
                 if(datas[1] == '0'){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("地主");
                     isLandLord = 0;

                 }else if(datas[1] == 1){
                     ui->labelPlayerStatus->setText("地主");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 1;
                 }else{
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("地主");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 2;
                 }
             }if(Name == "Player_2"){
                 if(datas[1] == '0'){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("地主");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 0;
                 }else if(datas[1] == 1){
                     ui->labelPlayerStatus->setText("农民");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("地主");
                     isLandLord = 1;
                 }else{
                     ui->labelPlayerStatus->setText("地主");
                     ui->labelPlayerstatus1->setText("农民");
                     ui->labelPlayerstatus2->setText("农民");
                     isLandLord = 2;
                 }
             }
             break;
         case 6:
             if(isLandLord == id){
                 for(int i = 0; i < 3; i ++){
                     player.playCards.push_back(Card(int(datas[i + 1])));
                     landLordCard.playCards.push_back(Card(int(datas[i + 1])));
                 }
                 sort(player.begin(), player.end());
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
                     labelLandlord[num]->setPixmap(QPixmap(QString(":/png/cards/%1%2.png").arg(color).arg(value)).scaled(w,h-60,Qt::KeepAspectRatio));
                     num ++;
                 }
             }

             break;
         default:
             break;
         }
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
}


void ClientWindow::landLordNo(){
    QByteArray arr;
    int struct_size, total_size;
    struct_size = 2;
    arr.append(1);
    arr.append(2);
    total_size = arr.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr[i]);
    }
    ui->labelPlayerStatus->setText("不抢");
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
    writeMessage(&data);
}


void ClientWindow::landLordYes(){
    QByteArray arr;
    int struct_size, total_size;
    struct_size = 2;
    arr.append(1);
    arr.append(1);
    total_size = arr.size();
    QByteArray data;
    data.append(total_size).append(struct_size);
    for(int i = 0; i < struct_size; i ++){
        data.append(arr[i]);
    }
    writeMessage(&data);
    ui->labelPlayerStatus->setText("抢地主");
    ui->pushButtonNo->setVisible(0);
    ui->pushButtonYes->setVisible(0);
}
