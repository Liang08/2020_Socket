#ifndef PLAYERCARD_H
#define PLAYERCARD_H
#include "card.h"
#include "vector"

class PlayerCard
{

public:
    PlayerCard();
    std::vector<Card> playCards;
    auto begin(){return playCards.begin();}
    auto end(){return playCards.end();}
};

#endif // PLAYERCARD_H
