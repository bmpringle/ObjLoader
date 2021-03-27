test:
	g++ -g -std=c++2a test.cpp lib/objLoaderLib.a -o test
library:
ifeq ($(OS),Windows_NT) 
	#todo: add support for garbage-tm os
endif
ifneq ($(OS),Windows_NT) 
	mkdir -p obj
	mkdir -p lib
	g++ -g -std=c++2a -c ObjLoader.cpp -O3 -o ./obj/ObjLoader.o
	g++ -g -std=c++2a -c ObjMesh.cpp -O3 -o ./obj/ObjMesh.o
	ar rvs ./lib/objLoaderLib.a ./obj/ObjLoader.o ./obj/ObjMesh.o
endif

library-with-printdebug:
ifeq ($(OS),Windows_NT) 
	#todo: add support for garbage-tm os
endif
ifneq ($(OS),Windows_NT) 
	mkdir -p obj
	mkdir -p lib
	g++ -g -std=c++2a -c ObjLoader.cpp -O3 -o ./obj/ObjLoader.o -DPRINT_MESH_OUTPUT
	g++ -g -std=c++2a -c ObjMesh.cpp -O3 -o ./obj/ObjMesh.o -DPRINT_MESH_OUTPUT
	ar rvs ./lib/objLoaderLib.a ./obj/ObjLoader.o ./obj/ObjMesh.o -DPRINT_MESH_OUTPUT
endif

clean: 
	rm -rf obj
	rm -rf lib
	rm test
library-cross:
	mkdir -p obj
	mkdir -p lib
	/usr/local/bin/x86_64-w64-mingw32-g++ -g -std=c++2a -c ObjLoader.cpp -O3 -o ./obj/ObjLoader.o
	/usr/local/bin/x86_64-w64-mingw32-g++ -g -std=c++2a -c ObjMesh.cpp -O3 -o ./obj/ObjMesh.o
	/usr/local/bin/x86_64-w64-mingw32-ar rvs ./lib/objLoaderLib.a ./obj/ObjLoader.o ./obj/ObjMesh.o

library-cross-with-printdebug:
	mkdir -p obj
	mkdir -p lib
	/usr/local/bin/x86_64-w64-mingw32-g++ -g -std=c++2a -c ObjLoader.cpp -O3 -o ./obj/ObjLoader.o -DPRINT_MESH_OUTPUT
	/usr/local/bin/x86_64-w64-mingw32-g++ -g -std=c++2a -c ObjMesh.cpp -O3 -o ./obj/ObjMesh.o -DPRINT_MESH_OUTPUT
	/usr/local/bin/x86_64-w64-mingw32-ar rvs ./lib/objLoaderLib.a ./obj/ObjLoader.o ./obj/ObjMesh.o

all:
	make clean
	make library
	make test

all-cross:
	make clean
	make library-cross
	make test