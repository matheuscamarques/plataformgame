#include "game/game.h"
#include "core/Log.h"

int main()
{
   try{
     Game::main();
   }catch(std::exception &e){
     LOG_ERROR("Main", e.what());
   }
};