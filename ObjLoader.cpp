#include "ObjLoader.h"

namespace {
    std::vector<std::string> extractStringFromInputFileStream(std::ifstream& ifs) {
        std::vector<std::string> lines = std::vector<std::string>();
        std::string buffer;
        while(std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        return lines;
    }

    void concatConsecutiveVectorLines(std::vector<std::string>& lines, int start) {
        std::string firstLine = lines.at(start);
        firstLine.erase(firstLine.size() - 1);
        lines.erase(start + lines.begin());
        lines.at(start) = firstLine + lines.at(start);
    }

    void removeObjLineContinuesFromStringVector(std::vector<std::string>& lines) {
        for(int i = 0; i < lines.size(); ++i) {
            std::string& line = lines.at(i);
            if(line.size() != 0) {
                if(line.at(line.size() - 1) == '\\') {
                    concatConsecutiveVectorLines(lines, i);
                    removeObjLineContinuesFromStringVector(lines);
                    break;
                }
            }
        }
    }

    void processVertexLine(ObjMeshPrimitive& primitiveMesh, std::string& line, int vParseState) {
        std::stringstream s;
        std::string buffer;

        float x, y, z, w;

        int i = 0;

        GeometricVertex gv;
        TextureVertex tv;
        NormalVertex nv;

        //determine what type of vertex we are parsing
        switch(vParseState) {
            case 0:
                s = std::stringstream(line.substr(2));

                w = 1;

                while(std::getline(s, buffer, ' ')) {
                    try {
                        switch(i) {
                            case 0:
                                x = std::stof(buffer);
                                break;
                            case 1:
                                y = std::stof(buffer);
                                break;
                            case 2:
                                z = std::stof(buffer);
                                break;
                            case 3:
                                w = std::stof(buffer);
                                break;
                            default:
                                return;
                        }
                    }catch(std::exception e) {
                        std::cout << "malformed obj file (reading gv)" << std::endl;
                        std::cout << "line: " << line << std::endl;
                        abort();
                    }
                    ++i;
                }
                if(i != 4 && i != 3) {
                    std::cout << "wrong number of floats in geometric vertex def" << std::endl;
                    abort();
                }
                gv = GeometricVertex();
                gv.geometricVertex = glm::vec4(x, y, z, w);
                primitiveMesh.geometricVertices[primitiveMesh.geometricVertices.size()] = gv;
                return;
            case 1:
                s = std::stringstream(line.substr(3));
                
                y = 0;
                z = 0;

                while(std::getline(s, buffer, ' ')) {
                    try {
                        switch(i) {
                            case 0:
                                x = std::stof(buffer);
                                break;
                            case 1:
                                y = std::stof(buffer);
                                break;
                            case 2:
                                z = std::stof(buffer);
                                break;
                            default:
                                return;
                        }
                    }catch(std::exception e) {
                        std::cout << "malformed obj file (reading tv)" << std::endl;
                        std::cout << "line: " << line << std::endl;
                        abort();
                    }
                    ++i;
                }
                if(i != 3 && i != 2 && i != 1) {
                    std::cout << "wrong number of floats in texture vertex def" << std::endl;
                    abort();
                }
                tv = TextureVertex();
                tv.textureVertex = glm::vec3(x, y, z);
                primitiveMesh.textureVertices[primitiveMesh.textureVertices.size()] = tv;
                return;
            case 2:
                s = std::stringstream(line.substr(3));

                while(std::getline(s, buffer, ' ')) {
                    try {
                        switch(i) {
                            case 0:
                                x = std::stof(buffer);
                                break;
                            case 1:
                                y = std::stof(buffer);
                                break;
                            case 2:
                                z = std::stof(buffer);
                                break;
                            default:
                                return;
                        }
                    }catch(std::exception e) {
                        std::cout << "malformed obj file (reading nv)" << std::endl;
                        std::cout << "line: " << line << std::endl;
                        abort();
                    }
                    ++i;
                }
                if(i != 3) {
                    std::cout << "wrong number of floats in normal vertex def" << std::endl;
                    abort();
                }
                nv = NormalVertex();
                nv.normalVertex = glm::vec3(x, y, z);
                primitiveMesh.normalVertices[primitiveMesh.normalVertices.size()] = nv;                
                return;
            default:
                std::cout << "how did we get here?" << std::endl;
                abort();
        }
    }

    void processFaceLine(std::vector<ObjFace>& faces, std::string& line, int numberOfVertices, int verticesNumOffset) {
        std::stringstream s(line.substr(2));
        std::string buffer;
        ObjFace newFace = ObjFace();

        int i = 0;
        while(std::getline(s, buffer, ' ')) {
            if(buffer.size() > 0) {
                if(buffer.at(buffer.size() - 1) != ' ') {
                    buffer = buffer + ' ';
                }
            }
            std::stringstream s1(buffer);
            std::vector<int> indices = std::vector<int>();
            try {
                while(std::getline(s1, buffer, '/')) {
                    buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
                    buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
                    buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

                    if(buffer.length() > 0) {
                        int index = std::stoi(buffer);
                        if(index < 1) {
                            index = verticesNumOffset + numberOfVertices - index + 1;
                        }
                        indices.push_back(index);
                    }
                }
            }catch(std::exception e) {
                std::cout << "malformed face def (error 1) " << "line: " << line << std::endl;
                std::cout << "at buffer: ";
                std::cout << buffer;
                std::cout << std::endl;
                abort();
            }

            if(indices.size() != 0) {
                if(indices.size() < 1 || indices.size() > 3) {
                    std::cout << "malformed face def (error 2) " << "line: " << line << std::endl;
                    abort();
                }

                int index = (faces.size() > 0) ? (faces.size() - 1) : 0;
                int vgs, vts, vns;

                if(index != 0) {
                    vgs = faces.at(index).indicesVG.size();
                    vts = faces.at(index).indicesVT.size();
                    vns = faces.at(index).indicesVN.size();
                }else {
                    vgs = 0;
                    vts = 0;
                    vns = 0;
                }
                if(indices.size() == 1) {
                    if((vts != 0 || vns != 0) && index != verticesNumOffset) {
                        std::cout << "inconsistent face def (1)" << "at line: " << line << std::endl;
                        abort();
                    }
                    
                    newFace.indicesVG.push_back(indices[0]);
                }else if(indices.size() == 2) {
                    if((vts != 0 && vns != 0 || vgs < vts || vgs < vns) && index != verticesNumOffset) {
                        std::cout << "inconsistent face def (2)" << "at line: " << line << std::endl;
                        abort();
                    }

                    if(vts != 0) {
                        newFace.indicesVG.push_back(indices[0]);
                        newFace.indicesVT.push_back(indices[1]);
                    }else {
                        newFace.indicesVG.push_back(indices[0]);
                        newFace.indicesVN.push_back(indices[1]);
                    }
                }else if(indices.size() == 3) {
                    if((vgs != vts || vgs != vns) && index != verticesNumOffset) {
                        std::cout << "inconsistent face def (3)" << "at line: " << line << std::endl;
                        abort();
                    }

                    newFace.indicesVG.push_back(indices[0]);
                    newFace.indicesVT.push_back(indices[1]);
                    newFace.indicesVN.push_back(indices[2]);
                }
            }
        }

        faces.push_back(newFace);
    }
};

ObjMeshPrimitive ObjLoader::loadMeshFromFile(std::string pathToFile) {
    ObjMeshPrimitive mesh = ObjMeshPrimitive();

    std::ifstream meshStream(pathToFile);

    std::vector<std::string> lines = extractStringFromInputFileStream(meshStream);

    removeObjLineContinuesFromStringVector(lines);
    
    std::vector<ObjFace>& faces = mesh.faces;

    int numberOfVertices = 0;
    int verticesNumOffset = 0;
    bool onFacesDef = false;

    for(int i = 0; i < lines.size(); ++i) {
        std::string& line = lines.at(i);
        if(line.size() == 0) {

        }else if(line.at(0) == '#') {

        }else {
            if(line.at(0) == 'v') {
                if(onFacesDef) {
                    onFacesDef = false;
                    numberOfVertices = 0;
                }

                if(line.substr(0, 2) == "v ") {
                    processVertexLine(mesh, line, 0);
                }else if(line.substr(0, 3) == "vt ") {
                    processVertexLine(mesh, line, 1);
                }else if(line.substr(0, 3) == "vn ") {
                    processVertexLine(mesh, line, 2);
                }else {
                    std::cout << "malformed vertex command (probably?) at line " << (i+1) << std::endl;
                    abort(); 
                }
                ++numberOfVertices;
            }else if(line.at(0) == 'f') {
                if(!onFacesDef) {
                    verticesNumOffset = verticesNumOffset + numberOfVertices;
                }
                onFacesDef = true;
                processFaceLine(faces, line, numberOfVertices, verticesNumOffset);
            }
        }
    }

    #ifdef PRINT_MESH_OUTPUT
        mesh.print();
    #endif

    return mesh;
}

std::unique_ptr<ObjMesh> ObjLoader::combinePrimitives(std::vector<ObjMeshPrimitive>& objmeshprimitives) {
    std::unique_ptr<ObjMesh> masterMesh = std::make_unique<ObjMesh>();
    masterMesh->submeshes = objmeshprimitives;
    return std::move(masterMesh);
}