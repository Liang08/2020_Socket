#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(1);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(connectHost()));
}

ClientWindow::~ClientWindow()
{
    delete ui;
}


void ClientWindow::connectHost(){
    this->readWriteSocket = new QTcpSocket;
    this->readWriteSocket->connectToHost(QHostAddress("127.0.0.1"),8888);
    connect(readWriteSocket, SIGNAL(readyRead()), this, SLOT(recvMessage()));

}


void ClientWindow::recvMessage(){
     QByteArray arr = readWriteSocket->readAll();
     switch (arr[0]) {
     case 1:
         ui->stackedWidget->setCurrentIndex(0);
         break;
     }
}


void ClientWindow::writeMessage(){

}
