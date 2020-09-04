#ifndef CARD_H
#define CARD_H


class Card
{
    int num;
    int value;  //牌面值 0~14：3~K,A,2,BlackJoker,RedJoker
    int color;  //花色  0：草花/BlackJoker 1：方块/RedJoker 2：黑桃 3：红桃
    void setColor();
    void setValue();
public:
    Card();
    Card(int i);
    int choosed = 0;
    int exist = 1;
    friend bool operator> (Card c1, Card c2);
    friend bool operator< (Card c1, Card c2);
    int getColor();
    int getNum();
    int getValue();
};

#endif // CARD_H
