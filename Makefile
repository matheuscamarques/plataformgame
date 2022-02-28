all:
	echo "#-#-# Compile #-#-#"
	g++ -c src/main.cpp 
	g++ -c src/game/game.cpp 
	g++ -c src/window/window.cpp
	g++ -c src/entities/entity/entity.cpp
	g++ -c src/level/Level/level.cpp
	g++ main.o -o plataformer  game.o window.o entity.o level.o -lsfml-graphics -lsfml-window -lsfml-system
	echo "#-#-# Moving files #-#-#"
	mv *.o compiled/
	mv plataformer build/
clear:
	rm -rf compiled/
	rm -rf build/
	mkdir compiled/
	mkdir build/
start:
	./build/plataformer
watch-build:
	watch -n 10 make
watch-start:
	watch -n 10 ./build/plataformer