#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <Windows.h>

using namespace std;


//add black jack criteria. if player had natural and dealer does not then dealer pays 1.5
//if dealer had natural then dealer immediately collects bet of anyone without naturals

//CLASSES--------------------------------------------------------------------

typedef struct Card
{ //struct which contains inforamtion on each card
    bool inDeck; 
    bool isAce;
    int num;
    char suit;
}Cards;

class Deck
{   
    public:
    //constructor creates the deck and stores the cards into deck_array
        Deck();
        Cards deck_array[52];
};

class Player
{
    public:

        Player(Deck *);
        void printCards(bool);
        void checkAce();
        void sumCards();
        void botSetAce();

        //number of cards the player has
        int numcard;
        int cardSum;
        int bank, bet;
        //array which holds player's cards, 
        //uses struct card which conatins information on the num and suit
        Cards playerCards[10];

};

//---------------------------------------------------------------------------


//PROTOTYPES-----------------------------------------------------------------

int randomCard(Deck *);
void dealCard(Deck *, Player *);
bool botChoice(Player *, int);
bool dealerChoice(Player *);
void printTable(Player *, Player *, Player *, int, bool);
void printBets(Player *, Player *, int);
int getNumPlayer(string);
void setPlayerBet(Player [], Player &, string, int);
void recordPlayerBank(Player [], Player *, string, int);

//---------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    
    char choice;
    int numPlayers;
    bool playGame = true;

    string dataFile = "bank_data.txt";

    
    

    while (playGame)
    {
        //initalize deck, this will also create the deck
        Deck deck1;
        //initalize dealer
        Player dealer1(&deck1);


        //initialize players
        int numPlayers = getNumPlayer(dataFile);
        Player* players = (Player*)malloc(sizeof(Player) * numPlayers);
       
        for (int i = 0; i < numPlayers; i++){
            players[i] = Player(&deck1);
        }
        
        
        //initialize main player
        Player player1(&deck1);
        

        int tempBank[numPlayers+2];

        
        

        setPlayerBet(players, player1,  dataFile, numPlayers);

        printBets(&player1, players, numPlayers);

        //intialize bets
        do{
            printf("How much would you like to bet?: ");
            scanf("%i", &(player1.bet));
            getchar();

            if (player1.bet > player1.bank){
                printf("\nBet amount is greater than what you have in the bank\n");
                printf("Bet a lower amount!\n");
            }

        } while (player1.bet > player1.bank);

        player1.bank -= player1.bet;


        srand(time(NULL));
        for (int i=0; i<numPlayers; i++)
        {
         
            if (players[i].bank == 0){
                players[i].bet = 0;

            } else {

                do {
                    players[i].bet = ((rand() % 999 + 1)+1)*10;

                } while (players[i].bet > players[i].bank);

                players[i].bank -= players[i].bet;
            }
        }

        //print initial table
        printTable(&dealer1, &player1, players, numPlayers, false);

        //player decision
        while (true)
        {
            
            printf("MAIN PLAYER:\n");
            printf("'h': hit | 's': stand\nMAKE YOUR CHOICE:");
            choice = getchar();
            getchar();

            Sleep(1500);

            if (choice == 'h')
            {
                
                dealCard(&deck1, &player1);

                printTable(&dealer1, &player1, players, numPlayers, false);

                if (player1.cardSum > 21)
                {
                    break;
                }
            }
            else if (choice == 's')
            {
                printf("\nMAIN PLAYER choose to pass\n\n");
                break;
            }
            else
            {
                printf("\nNot a valid command. Please choose 'h' or 's'\n");
            }

        }
        
        player1.checkAce();
        if (player1.cardSum > 21)
        {
            Sleep(1500);
            printf("\nMAIN PLAYER busted!!\n\n");
        }

        cout << "PRESS ENTER TO CONTINUE..." << endl;
        getchar();
        
        //other player's decisions
        Sleep(1500);

        for (int g=0; g<numPlayers; g++)
        {
            while (botChoice(&players[g] , g + 1))
            {
                dealCard(&deck1, &players[g]);
                if (players[g].cardSum > 21)
                {
                    Sleep(1500);
                    printf("PLAYER %i busted!!\n\n", g+1);
                    break;
                }
                
            }
            cout << "PRESS ENTER TO PRINT TABLE...";
            getchar();
            printTable(&dealer1, &player1, players, numPlayers, false);
            cout << "PRESS ENTER TO CONTINUE...";
            getchar();
            cout << endl;
        }

        //dealer decision
        cout << endl;
        
        while (dealerChoice(&dealer1))
        {
            dealCard(&deck1, &dealer1);
            if (dealer1.cardSum > 21)
            {
                Sleep(1500);
                printf("DEALER busted!!\n\n");
                break;
            }
        }


        cout << "PRESS ENTER TO PRINT TABLE...";
        getchar();
        printTable(&dealer1, &player1, players, numPlayers, true);

        //calc sum to 21
        int sumTo21[numPlayers+2];

        sumTo21[0] = 21 - dealer1.cardSum;
        sumTo21[1] = 21 - player1.cardSum;

        //print results
        cout << endl;
        if (dealer1.cardSum <= 21)
        {
            printf("DEALER: %i\n", dealer1.cardSum);
        }
        else
        {
            printf("DEALER Busted\n");
        }
        for (int i=0; i<numPlayers; i++)
        {   
            sumTo21[i+2] = 21 - players[i].cardSum;
            if (players[i].cardSum <=21)
            {
                printf("PLAYER %i: %i\n", i+1, players[i].cardSum);
            }
            else
            {
                printf("PLAYER %i Busted\n", i+1);
            }
        }
        if (player1.cardSum <= 21)
        {
            printf("MAIN PLAYER: %i\n", player1.cardSum);
        }
        else
        {
            printf("MAIN PLAYER Busted\n");
        }


        //hand out bets
        cout << endl;
        if (sumTo21[0] >= 0)
        {
            if ((sumTo21[1] < sumTo21[0]) && (sumTo21[1] > 0))
            {
                printf("DEALER pays MAIN PLAYER\n");
                player1.bank += 2*(player1.bet);
                player1.bet = 0;
            }
            else if (((sumTo21[1] > sumTo21[0]) && (sumTo21[1] > 0)) || (sumTo21[1] < 0))
            {
                printf("MAIN PLAYER pays DEALER\n");
                player1.bet = 0;
            }
            else
            {
                player1.bank += player1.bet;
                player1.bet = 0;
            }

            for (int i = 2; i<numPlayers+2; i++)
            {
                if ((sumTo21[i] < sumTo21[0]) && (sumTo21[i] > 0))
                {
                    printf("DEALER pays PLAYER %i\n", i-1);
                    players[i-2].bank += 2*(players[i-2].bet);
                    players[i-2].bet = 0;
                }
                else if (((sumTo21[i] > sumTo21[0]) && (sumTo21[i] > 0)) || (sumTo21[i] < 0))
                {
                    printf("PLAYER %i pays DEALER\n", i-1);
                    players[i-2].bet = 0;
                }
                else
                {
                    players[i-2].bank += players[i-2].bet;
                    players[i-2].bet = 0;
                }
            }
        }
        else if (sumTo21[0] < 0)
        {
            if (sumTo21[1] >= 0)
            {
                printf("DEALER pays MAIN PLAYER\n");
                player1.bank += 2*(player1.bet);
                player1.bet = 0;
            }
            else if (sumTo21[1] < 0)
            {
                player1.bank += player1.bet;
                player1.bet = 0;
            }

            for (int i=2; i<numPlayers+2; i++)
            {
                if (sumTo21[i] >= 0)
                {
                    printf("DEALER pays PLAYER %i\n", i-1);
                    players[i-2].bank += 2*(players[i-2].bet);
                    players[i-2].bet = 0;
                }
                else if (sumTo21[i] < 0)
                {
                    players[i-2].bank += players[i-2].bet;
                    players[i-2].bet = 0;
                }
            }
        }
        /*
        for (int a=0; a<numPlayers+2; a++)
        {
            cout << sumTo21[a] << endl;
        }
        */

        cout << "PRESS ENTER TO PRINT FINAL TABLE...";
        getchar();
        printTable(&dealer1, &player1, players, numPlayers, true);
        cout << endl << endl;

        recordPlayerBank(players, &player1, dataFile, numPlayers);

        int gameChoice;
        printf("'p': play again | 'q': quit\nMAKE YOUR CHOICE:");
        gameChoice = getchar();
        getchar();

        if (gameChoice == 'q')
        {
            playGame = false;
        }

    }

    return 0;
}


//CONSTRUCTORS---------------------------------------------------------------

Deck::Deck()
{ //create deck
    char suits[4] = {'D', 'H', 'S', 'C'};
    int a = 0;
    //loop thought the suits and the numbers and store them into deck_array struct
    //each index of deck_array is a different card
    for (int s = 0; s < 4; s++)
    {
        for (int n = 1; n <=13; n++)
        {
            deck_array[a].num = n;
            deck_array[a].suit = suits[s];
            deck_array[a].inDeck = true;
            deck_array[a].isAce = false;
            a++;
        }
    }
    
    
}

Player::Player(Deck *deck)
{ //assign default number of cards to a player, 2
  //assign 2 random cards to player
    numcard = 2;

    bet = 0;
    bank = 10000;
    int randCard = randomCard(deck);
    playerCards[0].num = deck->deck_array[randCard].num;
    playerCards[0].suit = deck->deck_array[randCard].suit;
    playerCards[0].isAce = deck->deck_array[randCard].isAce;

    randCard = randomCard(deck);
    playerCards[1].num = deck->deck_array[randCard].num;
    playerCards[1].suit = deck->deck_array[randCard].suit;
    playerCards[1].isAce = deck->deck_array[randCard].isAce;

    sumCards();
}

//---------------------------------------------------------------------------


//FUNCTIONS------------------------------------------------------------------

void Player::printCards(bool isPlayer)
{ //print your cards 

    int numocard = 1;

    if (isPlayer)
    {    
        numocard = numcard;
    }

    for(int i = 0; i < numcard; i++)
    {
        printf("+---+\t");
        
    }
    printf("\n");
    for(int i = 0; i < numocard; i++)
    {
        if ((playerCards[i].num > 1) && (playerCards[i].num < 10))
        {
            printf("|%i  |\t", playerCards[i].num);
        }
        else if (playerCards[i].num == 10)
        {
            printf("|10 |\t");
        }
        else if (playerCards[i].num == 1)
        {
            printf("|A  |\t");
        }
        else if (playerCards[i].num == 11)
        {
            if (playerCards[i].isAce)
            {
                printf("|A  |\t");
            }
            else
            {
                printf("|J  |\t");
            }
        }
        else if (playerCards[i].num == 12)
        {
            printf("|Q  |\t");
        }
        else if (playerCards[i].num == 13)
        {
            printf("|K  |\t");
        }
        if (!isPlayer)
        {
            for (int i = 0; i < (numcard-1);i++)
            {
                printf("|###|\t");
            }
        }
    }
    printf("\n");
    for(int i = 0; i < numocard; i++)
    {   
        if (playerCards[i].suit == 'H')
        {
            printf("| %c |\t", (char)3);
        }
        else if (playerCards[i].suit == 'D')
        {
            printf("| %c |\t", (char)4);
        }
        else if (playerCards[i].suit == 'C')
        {
            printf("| %c |\t", (char)5);
        }
        else if (playerCards[i].suit == 'S')
        {
            printf("| %c |\t", (char)6);
        }                
        if (!isPlayer)
        {
            for (int i = 0; i < (numcard-1);i++)
            {
                printf("|###|\t");
            }
        }
    }
    printf("\n");
    for(int i = 0; i < numocard; i++)
    {
        if ((playerCards[i].num > 1) && (playerCards[i].num < 10))
        {
            printf("|  %i|\t", playerCards[i].num);
        }
        else if (playerCards[i].num == 10)
        {
            printf("| 10|\t");
        }
        else if (playerCards[i].num == 1)
        {
            printf("|  A|\t");
        }
        else if (playerCards[i].num == 11)
        {
            if (playerCards[i].isAce)
            {
                printf("|  A|\t");
            }
            else
            {
                printf("|  J|\t");
            }

        }
        else if (playerCards[i].num == 12)
        {
            printf("|  Q|\t");
        }
        else if (playerCards[i].num == 13)
        {
            printf("|  K|\t");
        }
        if (!isPlayer)
        {
            for (int i = 0; i < (numcard-1);i++)
            {
                printf("|###|\t");
            }
        }
    }
    printf("\n");
    for(int i = 0; i < numcard; i++)
    {
        printf("+---+\t");
    }
    printf("\n\n");

    
    
  
/*
♣
♦
♥
♠
*/


}

void Player::checkAce()
{
    int ace,b=1;

    for(int a=0; a<numcard; a++)
    {
        if ((playerCards[a].num == 1) && (playerCards[a].isAce == false))
        {
            printf("Do you want your ACE %i to be 1 or 11?:", b);
            b++;
            scanf("%i", &ace);
            getchar();

            if (ace == 1)
            {
                playerCards[a].isAce = true;
            }
            else if (ace == 11)
            {
                playerCards[a].num = 11;
                playerCards[a].isAce = true;

            }
            else
            {
                printf("Please input valid number (1 or 11): ");
            }

        }
        
    }

    sumCards();



}

void Player::sumCards()
{
    cardSum = 0;
    for (int i = 0; i < numcard; i++)
    {
        if ((playerCards[i].num == 11) || (playerCards[i].num == 12) || (playerCards[i].num == 13))
        {
            cardSum += 10;
        }
        else
        {
            cardSum += playerCards[i].num;
        }
    }
}

void Player::botSetAce(){

    

    
}

int randomCard(Deck *deck)
{ //pull random card from deck
    srand(time(NULL));
    int randCard;
    while (true)
    {
        randCard = rand() % 52;
        if (deck->deck_array[randCard].inDeck == true)
        {
            deck->deck_array[randCard].inDeck = false;
            break;
        }
    }
    return randCard;    
}

void dealCard(Deck *deck, Player *player)
{//deals as random card from the deck to a player
    int randCard;
    randCard = randomCard(deck);

    player->playerCards[player->numcard].num = deck->deck_array[randCard].num;
    player->playerCards[player->numcard].suit = deck->deck_array[randCard].suit;
    player->playerCards[player->numcard].isAce = deck->deck_array[randCard].isAce;

    (player->numcard)++;

    player->sumCards();
    

}

bool botChoice(Player *player, int playernum)
{ //bot choices whether to draw a card, return true if decision is yes
    int chance;
    srand(time(NULL));

    chance = rand() % 100 + 1;
    
    if ((player->cardSum) <= 8)
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 9) && (chance < 92))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 10) && (chance < 84))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 11) && (chance < 77))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 12) && (chance < 69))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 13) && (chance < 61))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 14) && (chance < 54))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 15) && (chance < 46))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 16) && (chance < 38))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 17) && (chance < 30))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 18) && (chance < 23))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 19) && (chance < 15))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else if (((player->cardSum) == 20) && (chance < 7))
    {
        printf("PLAYER %i decided to draw a card\n", playernum);
        return true;
    }
    else
    {
        printf("PLAYER %i decided to pass\n", playernum);
        return false;
    }
    

}

bool dealerChoice(Player *dealer)
{
    if ((dealer->cardSum) <= 16)
    {
        for (int i = 0; i<dealer->numcard; i++)
        {
            if ((dealer->playerCards[i].num) == 1)
            {
                if ((((dealer->cardSum) +10) >= 17) && (((dealer->cardSum) +10) <= 21))
                {
                    dealer->playerCards[i].num = 11;
                    dealer->playerCards[i].isAce = true;
                    dealer->sumCards();
                    printf("DEALER decided to pass\n");
                    return false;
                }
            }
        }
        printf("DEALER decided to draw a card\n");
        return true;
    }
    else if ((dealer->cardSum) > 16)
    {
        printf("DEALER decided to pass\n");
        return false;
    }
    printf("DEALER decided to draw a card\n");
    return true;
}

void printTable(Player *dealer1, Player *player1, Player *players, int numPlayers, bool hideCards)
{
    printf("\n\n-----------------------------------------\n\n");
    printBets(player1, players, numPlayers);
    printf("DEALER:\n");
    dealer1->printCards(hideCards);
    for (int f=0; f < numPlayers; f++)
    {
        printf("PLAYER: %i\n" , f+1);
        players[f].printCards(hideCards);
    }
    printf("MAIN PLAYER:\n");
    player1->printCards(true);
    printf("-----------------------------------------\n\n");
}

void printBets(Player *player1, Player *players, int numPlayers)
{
    printf("+==================================+\n");
    printf("|%-13sBank=%-6iBet=%-6i|\n", "MAIN PLAYER:", player1->bank, player1->bet);
    for (int i=0; i<numPlayers; i++)
    {
        printf("|PLAYER %i:%-3s Bank=%-6iBet=%-6i|\n", i+1, "", players[i].bank, players[i].bet);
    }
    printf("+==================================+\n");
}

int getNumPlayer(string fileName){

    fstream file(fileName);
    string line; 
    int playerCount = 0;   

    if (file){
        while(!file.eof()){
                
            getline (file, line);

            if (line.length() == 5){
                string playerCheck = line.substr(0,3);

                if (playerCheck.compare("Bot") == 0){
                    playerCount++;
                }

            }

        }
    } else {
        cout << "file failed to open";
        exit(1);
    }

    file.close();

    return playerCount;

}

void setPlayerBet(Player players[], Player &player1, string fileName, int numPlayers){

    fstream file(fileName);

    string line;
    if (file){

        

        for (int i = 0; i < numPlayers; i++){

            getline(file, line);
            getline(file, line);
            
            players[i].bank = stoi(line);  

            getline(file, line);
        }

        getline(file, line);
        getline(file, line);

        player1.bank = stoi(line);

    } else {
        cout << "file failed to open";
        exit(1); 
    }

    file.close();
}

void recordPlayerBank(Player players[], Player *player1, string fileName, int numPlayers){

    ofstream file(fileName);


    

    for (int i = 1; i < numPlayers + 1; i++){
        file << "Bot " << i << endl;
        file << players[i-1].bank << endl << endl;
    }
        
    file << "User" << endl;
    file << player1->bank;

    file.close();

   
}

//---------------------------------------------------------------------------