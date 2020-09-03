#include "card.h"

Card::Card()
{

}


Card::Card(int i){
    num = i;
    setValue();
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


int Card::getValue(){
    return value;
}


bool operator>(Card c1, Card c2){
    return c1.getNum() > c2.getNum();
}
