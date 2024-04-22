src := src/main.c src/session_dat.c src/render/render.c src/sprite/sprite.c src/.interpreter/dil.c

#use default C compiler
CC = cc
#default, no extra flags.
FLAGS =

#default and recommended option is lib
all: clean lib install 
freestanding:
#use this option to create an application. The guest game/program shall be integrated into the DGL source
	${CC} ${src} ${FLAGS} -lDGL -lGL -lm -lpthread -ldl -lrt -lX11 -o ./dgl

lib: 
# default: Use this option to generate a static library. Attatch this library to your game. See the manual
	${CC} ${FLAGS} -c -o build/main.o src/main.c
	${CC} ${FLAGS} -c -o build/session_dat.o src/session_dat.c
	${CC} ${FLAGS} -c -o build/render.o src/render/render.c
	${CC} ${FLAGS} -c -o build/sprite.o src/sprite/sprite.c
#	${CC} ${FLAGS} -c -o build/.dil.o src/.interpreter/dil.c # Experimental; under development

	cd build && sudo ar -x /usr/local/lib/libraylib.a

	ar rcs dgl.a build/*.o

install:
# install
	sudo cp dgl.h /usr/local/include/
	mv dgl.a libDGL.a
	sudo cp libDGL.a /usr/local/lib/

uninstall:
	sudo rm -rf /usr/local/include/dgl.h /usr/local/lib/libDGL.a

clean: uninstall
	rm -rf ./dgl.a ./libDGL.a ./build/*

#EXPERIMENTAL
#Dodge Interpreted Language (DIL) interpreter
dil:
	cc ./.dil_main.c -lDGL -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o ./dil
	sudo cp ./dil /usr/bin/