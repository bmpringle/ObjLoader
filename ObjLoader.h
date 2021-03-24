#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "ObjMesh.h"
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>

namespace ObjLoader {
    ObjMeshPrimitive loadMeshFromFile(std::string pathToFile);

    /*
     * @brief
     * unique ptr is used because it prevents copying of the data, which would be very bad(tm).
     */
    std::unique_ptr<ObjMesh> combinePrimitives(std::vector<ObjMeshPrimitive>& objmeshprimitives);

    /*
     * @brief
     * Triangulate a primitive mesh
     */
    void triangulatePrimitiveMesh(ObjMeshPrimitive& mesh);
};
#endif