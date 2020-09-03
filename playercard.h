#ifndef PLAYERCARD_H
#define PLAYERCARD_H
#include "card.h"
#include "vector"

class PlayerCard
{

public:
    PlayerCard();
    std::vector<Card> playCards;
};

#endif // PLAYERCARD_H
