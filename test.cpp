#include "ObjLoader.h"

int main() {
    std::vector<ObjMesh> mesh = {ObjLoader::loadMeshFromFile("teapot.obj"), ObjLoader::loadMeshFromFile("cube-non-triangle.obj"), ObjLoader::loadMeshFromFile("test.obj")};//, ObjLoader::loadMeshFromFile("../MinecraftCloneSimple/src/assets/models/entity/high_quality_teapot.obj")};

    std::vector<ObjMeshPrimitive> prims = std::vector<ObjMeshPrimitive>();

    ObjMesh masterMesh = ObjLoader::combineMeshes(mesh, "combinedmesh");

    std::cout << "-------------------------------" << std::endl;
    std::cout << "original version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    prims = masterMesh.getPrimitives();
    
    for(ObjMeshPrimitive prim : prims) {
        prim.print();
    }
    ObjLoader::triangulateMesh(masterMesh);
    std::cout << "-------------------------------" << std::endl;
    std::cout << "triangulated version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    prims = masterMesh.getPrimitives();;
    for(ObjMeshPrimitive prim : prims) {
        prim.print();
    }

    std::cout << "-------------------------------" << std::endl;
    std::cout << "get cube model specifically version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    prims = masterMesh.getMeshWithName("Cube").getPrimitives();
    for(ObjMeshPrimitive prim : prims) {
        prim.print();
    }
    
}