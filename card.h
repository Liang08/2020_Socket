#ifndef CARD_H
#define CARD_H


class Card
{
    int num;
    int value;
    void setValue();
public:
    Card();
    Card(int i);
    friend bool operator> (Card c1, Card c2);
    int getNum();
    int getValue();
};

#endif // CARD_H
