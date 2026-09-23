/**
 * @file src/main.cpp
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Ponto de entrada que inicia o jogo com captura de erros.
 * @details Chama Game::main em try catch e loga exceção com LOG_ERROR, usado pelo build como executável.
 */

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