#include "ObjMesh.h"
#include "ObjLoader.h"

//never gauranteed to be valid post getMeshWithName call.
ObjMesh& ObjMesh::getMeshWithName(std::string name) {
    if(this->name == name) {
        return *this;
    }else {
        for(std::pair<const std::string, ObjMesh>& submesh : submeshes) {
            return submesh.second.getMeshWithName(name);
        }
        return ObjLoader::noMesh;
    }
}

void ObjMesh::getPrimitives(std::vector<ObjMeshPrimitive>& prims) {
    if(isLeaf()) {
        prims.push_back(mesh);
    }else {
        for(std::pair<const std::string, ObjMesh>& submesh : submeshes) {
             submesh.second.getPrimitives(prims);
        }
    }
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