#include "ObjLoader.h"

int main() {
    std::vector<ObjMesh> meshes = {ObjLoader::loadMeshFromFile("teapot.obj"), ObjLoader::loadMeshFromFile("cube-non-triangle.obj")/*, ObjLoader::loadMeshFromFile("test.obj")*/};
    std::vector<std::string> names = {"teapot", "cube-non-triangle"/*, "test"*/};
    ObjMeshCollection mesh = ObjLoader::combineMeshes(meshes, names);
    std::cout << "-------------------------------" << std::endl;
    std::cout << "original version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    for(std::pair<const std::string, ObjMesh>& submesh : mesh.getSubmeshMap()) {
        for(std::pair<const std::string, ObjMeshPrimitive>& primitive : submesh.second.primitiveMeshes) {
            primitive.second.print();
        }
    }
    ObjLoader::triangulateMesh(mesh);

    std::cout << "-------------------------------" << std::endl;
    std::cout << "triangulated version:" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    for(std::pair<const std::string, ObjMesh>& submesh : mesh.getSubmeshMap()) {
        for(std::pair<const std::string, ObjMeshPrimitive>& primitive : submesh.second.primitiveMeshes) {
            primitive.second.print();
        }
    }
}