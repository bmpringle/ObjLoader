#ifndef OBJMESH_H
#define OBJMESH_H

#include <map>
#include <vector>

//temp inc from other project
#include "glm/glm/glm.hpp"

#include <iostream>

struct GeometricVertex {
    /*
     * @brief
     * Vertex Geometry: REQUIRED
     * x, y, z, w
     * x is the x component of the vertex in 3d space. REQUIRED.
     * y is the y component of the vertex in 3d space. REQUIRED.
     * z is the z component of the vertex in 3d space. REQUIRED.
     * w is the w component of the vertex in homogenous coordinates, 
     *  used for rational curves and surfaces. OPTIONAL. DEFAULTS TO 1.
     */
    glm::vec4 geometricVertex = glm::vec4(0, 0, 0, 1);

    float& operator[](int index) {
        return geometricVertex[index];
    }
};

struct TextureVertex {
    /*
     * @brief
     * Texture Vertex: OPTIONAL
     * u, v, w
     * u is the horizontal component of the texture. REQUIRED.
     * v is the vertical component of the texture. OPTIONAL. DEFAULTS TO 0.
     * w is the depth component of the texture. OPTIONAL. DEFAULTS TO 0.
     */
    glm::vec3 textureVertex = glm::vec3(0, 0, 0);

    float& operator[](int index) {
        return textureVertex[index];
    }
};

struct NormalVertex {
   /*
     * @brief
     * Normal Vertex: OPTIONAL
     * i, j, k
     * i is the x component of the vertex normal in 3d space. REQUIRED.
     * j is the y component of the vertex normal in 3d space. REQUIRED.
     * k is the z component of the vertex normal in 3d space. REQUIRED.
     */
    glm::vec3 normalVertex = glm::vec3(0, 0, 0);

    float& operator[](int index) {
        return normalVertex[index];
    }
};

struct ObjFace {
    std::vector<int> indicesVG = std::vector<int>();
    std::vector<int> indicesVT = std::vector<int>();
    std::vector<int> indicesVN = std::vector<int>();
};

struct ObjMeshPrimitive {
    std::map<int, GeometricVertex> geometricVertices = std::map<int, GeometricVertex>();
    std::map<int, TextureVertex> textureVertices = std::map<int, TextureVertex>();
    std::map<int, NormalVertex> normalVertices = std::map<int, NormalVertex>();

    std::vector<ObjFace> faces = std::vector<ObjFace>();

    void print() {
        for(std::pair<int, GeometricVertex> vertex : geometricVertices) {
            std::cout << "v #" << vertex.first << ": " << vertex.second[0] << " "
            << vertex.second[1] << " " << vertex.second[2] << " "
            << vertex.second[3] << ", " << std::endl;
        }

        for(std::pair<int, TextureVertex> vertex : textureVertices) {
            std::cout << "vt #" << vertex.first << ": " << vertex.second[0] << " "
            << vertex.second[1] << " " << vertex.second[2] << ", " << std::endl;
        }

        for(std::pair<int, NormalVertex> vertex : normalVertices) {
            std::cout << "vn #" << vertex.first << ": " << vertex.second[0] << " "
            << vertex.second[1] << " " << vertex.second[2] << ", " << std::endl;
        }

        int facenumber = 1;
        for(ObjFace face : faces) {
            std::cout << "f #" << facenumber << ": ";
            for(int i = 0; i < face.indicesVG.size(); ++i) {
                bool hasVN = false;
                bool hasVT = false;
                if(face.indicesVT.size() != 0) {
                    hasVT = true;
                }
                if(face.indicesVN.size() != 0) {
                    hasVN = true;
                }
                std::cout << face.indicesVG.at(i);
                if(hasVT) {
                    std::cout << " " << face.indicesVT.at(i);
                }
                if(hasVN) {
                    std::cout << " " << face.indicesVN.at(i);
                }
                if(i + 1 != face.indicesVG.size()) {
                    std::cout << ", ";
                }
            }
            std::cout << std::endl;
            ++facenumber;
        }
    }
};

struct ObjMesh {
    std::vector<ObjMeshPrimitive> submeshes = std::vector<ObjMeshPrimitive>();
};

#endif