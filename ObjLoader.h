#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "ObjMesh.h"
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>

namespace ObjLoader {
    ObjMesh loadMeshFromFile(std::string pathToFile);

    /*
     * @brief
     * unique ptr is used because it prevents copying of the data, which would be very bad(tm).
     */
    ObjMesh combineMeshes(std::vector<ObjMesh> submeshes, std::string name);

    /*
     * @brief
     * Triangulate a primitive mesh
     */
    void triangulateMesh(ObjMesh& mesh);

    static ObjMesh noMesh = ObjMesh("noMesh");
};
#endif