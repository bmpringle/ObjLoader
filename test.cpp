#include "ObjLoader.h"

int main() {
    std::vector<ObjMeshPrimitive> mesh = {ObjLoader::loadMeshFromFile("teapot.obj"), ObjLoader::loadMeshFromFile("cube-non-triangle.obj"), ObjLoader::loadMeshFromFile("test.obj")};
    std::cout << "-------------------------------" << std::endl;
    std::cout << "original version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    mesh[0].print();
    ObjLoader::triangulatePrimitiveMesh(mesh[0]);
    std::cout << "-------------------------------" << std::endl;
    std::cout << "triangulated version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    mesh[0].print();
    std::unique_ptr<ObjMesh> masterMesh = ObjLoader::combinePrimitives(mesh);
}