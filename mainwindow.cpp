#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QDebug>
#include <vector>
#include <QTime>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("斗地主_1");
    ui->stackedWidget->setCurrentIndex(1);

    tcpServer = nullptr;

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(initServer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


//生成初始牌面
void MainWindow::init(){
    std::vector<int> card_0;
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
}


void MainWindow::initServer(){
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 8888);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void MainWindow::acceptConnection(){
    if(connectNum == 0){
        this->tcpSocket_1 = this->tcpServer->nextPendingConnection();
    }else{
        this->tcpSocket_2 = this->tcpServer->nextPendingConnection();
        ui->stackedWidget->setCurrentIndex(0);
        QByteArray arr;
        arr.append(1);
        writeMessageTo1(&arr);
        writeMessageTo2(&arr);
        init();
    }
    connectNum ++;
}



void MainWindow::writeMessageTo1(QByteArray *array){
    tcpSocket_1->write(array->data());
    tcpSocket_1->waitForBytesWritten();
}


void MainWindow::writeMessageTo2(QByteArray *array){
    tcpSocket_2->write(array->data());
    tcpSocket_2->waitForBytesWritten();
}


void MainWindow::recvMessage(){

}
