#include "ObjMesh.h"
#include "ObjLoader.h"

ObjMesh ObjMesh::getMeshWithName(std::string name) {
    if(this->name == name) {
        return *this;
    }else {
        for(std::pair<const std::string, ObjMesh> submesh : submeshes) {
            ObjMesh msh = submesh.second.getMeshWithName(name);
            return msh;
        }
        return ObjLoader::noMesh;
    }
}

std::vector<ObjMeshPrimitive> ObjMesh::getPrimitives() {
    std::vector<ObjMeshPrimitive> prims = std::vector<ObjMeshPrimitive>();
    if(isLeaf()) {
        prims.push_back(mesh);
    }else {
        for(std::pair<const std::string, ObjMesh> submesh : submeshes) {
            std::vector<ObjMeshPrimitive> tempPrims = std::vector<ObjMeshPrimitive>();
            tempPrims = submesh.second.getPrimitives();
            for(ObjMeshPrimitive prim : tempPrims) {
                prims.push_back(prim);
            }
        }
    }
    return prims;
} 

ObjMeshPrimitive& ObjMesh::getPrimitive() {
    if(!isLeaf()) {
        throw std::invalid_argument("no primitve");
    }
    return mesh;
}

void ObjMesh::setIsLeaf(bool isLeafBool) {
    isLeafNode = isLeafBool;
}

bool ObjMesh::isLeaf() {
    return isLeafNode;
}

ObjMesh::ObjMesh(std::string name) : name(name), isLeafNode(false) {

}

ObjMesh::ObjMesh() : isLeafNode(false) {

}