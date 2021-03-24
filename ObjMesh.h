#ifndef OBJMESH_H
#define OBJMESH_H

#include <map>
#include <vector>

#include "glm/glm/glm.hpp"
#include "glm/glm/ext.hpp"

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
    std::vector<GeometricVertex> geometricVertices = std::vector<GeometricVertex>();
    std::vector<TextureVertex> textureVertices = std::vector<TextureVertex>();
    std::vector<NormalVertex> normalVertices = std::vector<NormalVertex>();

    std::vector<ObjFace> faces = std::vector<ObjFace>();

    void print() {
        int i = 1;
        for(GeometricVertex vertex : geometricVertices) {
            std::cout << "v #" << i << ": " << vertex[0] << " "
            << vertex[1] << " " << vertex[2] << " "
            << vertex[3] << ", " << std::endl;
            ++i;
        }

        i = 1;
        for(TextureVertex vertex : textureVertices) {
            std::cout << "vt #" << i << ": " << vertex[0] << " "
            << vertex[1] << " " << vertex[2] << ", " << std::endl;
            ++i;
        }

        i = 1;
        for(NormalVertex vertex : normalVertices) {
            std::cout << "vn #" << i << ": " << vertex[0] << " "
            << vertex[1] << " " << vertex[2] << ", " << std::endl;
            ++i;
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