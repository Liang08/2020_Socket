#include "mylabel.h"
#include <QMouseEvent>
#include <QDebug>

MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{

}


void MyLabel::mousePressEvent(QMouseEvent *ev){
    QPoint p = ev->globalPos();
    p = this->mapFromGlobal(p);
    int x = p.x() / 30;
    qDebug() << x;
    emit chooseCard(x);
}
