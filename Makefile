all:
	echo "#-#-# Compile #-#-#"
	g++ -c src/main.cpp
	g++ -c src/game/game.cpp 
	g++ -c src/window/window.cpp
	g++ -c src/component/component.cpp
	g++ -c src/entities/entity/entity.cpp
	g++ -c src/entities/player/player.cpp
	g++ -c src/level/Level/level.cpp
	g++ -c src/camera/Camera.cpp
	g++ main.o -o plataformer  game.o window.o component.o entity.o player.o level.o Camera.o -lsfml-graphics -lsfml-window -lsfml-system
	echo "#-#-# Moving files #-#-#"
	mv *.o compiled/
	mv plataformer build/
clear:
	rm -rf compiled/
	rm -rf build/
	mkdir compiled/
	mkdir build/
start:
	make
	./build/plataformer
watch:
	watch -n 10 make all