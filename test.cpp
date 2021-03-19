#include "ObjLoader.h"

int main() {
    std::vector<ObjMeshPrimitive> mesh = {ObjLoader::loadMeshFromFile("teapot.obj"), ObjLoader::loadMeshFromFile("simple.obj")};
    std::unique_ptr<ObjMesh> masterMesh = ObjLoader::combinePrimitives(mesh);
}