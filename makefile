test:
	g++ -g -std=c++2a test.cpp lib/objLoaderLib.a -o test
library:
ifeq ($(OS),Windows_NT) 
	#todo: add support for garbage-tm os
endif
ifneq ($(OS),Windows_NT) 
	mkdir -p obj
	mkdir -p lib
	g++ -g -std=c++2a -c ObjLoader.cpp -o ./obj/ObjLoader.o
	ar rvs ./lib/objLoaderLib.a ./obj/ObjLoader.o
endif

library-with-printdebug:
ifeq ($(OS),Windows_NT) 
	#todo: add support for garbage-tm os
endif
ifneq ($(OS),Windows_NT) 
	mkdir -p obj
	mkdir -p lib
	g++ -std=c++2a -c ObjLoader.cpp -o  ./obj/ObjLoader.o -DPRINT_MESH_OUTPUT
	ar rvs ./lib/objLoaderLib.a ./obj/ObjLoader.o
endif

clean: 
	rm -rf obj
	rm -rf lib
	rm test
library-cross:
	mkdir -p obj
	mkdir -p lib
	/usr/local/bin/x86_64-w64-mingw32-g++ -std=c++2a -O3 -c ObjLoader.cpp -o  ./obj/ObjLoader.o
	/usr/local/bin/x86_64-w64-mingw32-ar rvs ./lib/objLoaderLib.lib ./obj/ObjLoader.o

library-cross-with-printdebug:
	mkdir -p obj
	mkdir -p lib
	/usr/local/bin/x86_64-w64-mingw32-g++ -std=c++2a -c ObjLoader.cpp -o  ./obj/ObjLoader.o -DPRINT_MESH_OUTPUT
	/usr/local/bin/x86_64-w64-mingw32-ar rvs ./lib/objLoaderLib.lib ./obj/ObjLoader.o ./lib/objLoaderLib.a ./obj/ObjLoader.o

all:
	make clean
	make library
	make test

all-cross:
	make clean
	make library-cross
	make test