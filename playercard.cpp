#include "playercard.h"
#include <algorithm>
#include <vector>

PlayerCard::PlayerCard()
{

}


Card& PlayerCard::operator[](int num){
    for(int i = 0; i < playCards.size(); i ++){
        if(playCards[i].getNum() == num)
            return playCards[i];
    }
}
