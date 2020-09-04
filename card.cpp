#include "card.h"

Card::Card()
{

}


Card::Card(int i){
    num = i;
    setValue();
    setColor();
}


int Card::getNum(){
    return num;
}


void Card::setValue(){
    if(num <= 52){
        value = num / 4;
    }else
        value = 14;
}


void Card::setColor(){
    color = num % 4;
}


int Card::getValue(){
    return value;
}


int Card::getColor(){
    return color;
}


bool operator>(Card c1, Card c2){
    return c1.getNum() > c2.getNum();
}
