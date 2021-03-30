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
    ObjMeshCollection combineMeshes(std::vector<ObjMesh>& objmeshprimitives, std::vector<std::string>& names);

    /*
     * @brief
     * Triangulate a primitive mesh
     */
    void triangulateMesh(ObjMeshCollection& masterMesh);

    void triangulateMesh(ObjMesh& masterMesh);
};
#endif