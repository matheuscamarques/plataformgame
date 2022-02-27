make: 
	g++ -c main.cpp window/game/game.cpp window/window/window.cpp  
	
	g++ main.o -o sfml-app  game.o window.o -lsfml-graphics -lsfml-window -lsfml-system