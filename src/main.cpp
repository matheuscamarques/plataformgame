#include "game/game.h"
#include "iostream"

int main()
{
   try{
     Game::main();
   }catch(std::exception &e){
     std::cout << e.what() << std::endl;
   }
};