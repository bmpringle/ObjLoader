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
                primitiveMesh.geometricVertices.push_back(gv); 
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
                primitiveMesh.textureVertices.push_back(tv); 
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
                primitiveMesh.normalVertices.push_back(nv);                
                return;
            default:
                std::cout << "how did we get here?" << std::endl;
                abort();
        }
    }

    void processFaceLine(std::vector<ObjFace>& faces, std::string& line, int numberOfVertices, int verticesNumOffset, int currentMaterialID) {
        std::stringstream s(line.substr(2));
        std::string buffer;
        ObjFace newFace = ObjFace();
        newFace.materialIndex = currentMaterialID;

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
                    
                    newFace.indicesVG.push_back(indices[0] - 1);
                }else if(indices.size() == 2) {
                    if((vts != 0 && vns != 0 || vgs < vts || vgs < vns) && index != verticesNumOffset) {
                        std::cout << "inconsistent face def (2)" << "at line: " << line << std::endl;
                        abort();
                    }

                    if(vts != 0) {
                        newFace.indicesVG.push_back(indices[0] - 1);
                        newFace.indicesVT.push_back(indices[1] - 1);
                    }else {
                        newFace.indicesVG.push_back(indices[0] - 1);
                        newFace.indicesVN.push_back(indices[1] - 1);
                    }
                }else if(indices.size() == 3) {
                    if((vgs != vts || vgs != vns) && index != verticesNumOffset) {
                        std::cout << "inconsistent face def (3)" << "at line: " << line << std::endl;
                        abort();
                    }

                    newFace.indicesVG.push_back(indices[0] - 1);
                    newFace.indicesVT.push_back(indices[1] - 1);
                    newFace.indicesVN.push_back(indices[2] - 1);
                }
            }
        }

        faces.push_back(newFace);
    }

    //line2 = x1, y1, z1, x2, y2, z2
    //line2 = x3, y3, z3, x4, y4, z4
    bool doSegmentsIntersect(double line[6], double line2[6]) {
        double xDelta1 = line[0] - line[3];
        double yDelta1 = line[1] - line[4];
        double zDelta1 = line[2] - line[5];

        double yPerX1 = yDelta1 / xDelta1;
        double zPerX1 = zDelta1 / xDelta1;

        double yXConstant1 = line[1] - line[0] * yPerX1;
        double zXConstant1 = line[2] - line[0] * zPerX1;

        double xDelta2 = line2[0] - line2[3];
        double yDelta2 = line2[1] - line2[4];
        double zDelta2 = line2[2] - line2[5];

        double yPerX2 = yDelta2 / xDelta2;
        double zPerX2 = zDelta2 / xDelta2;
        
        double yXConstant2 = line2[1] - line2[0] * yPerX2;
        double zXConstant2 = line2[2] - line2[0] * zPerX2;

        double x = (yXConstant2 - yXConstant1) / (yPerX1 - yPerX2);
        double py1 = yXConstant1 + yPerX1 * x;
        double py2 = yXConstant2 + yPerX2 * x;

        double pz1 = zXConstant1 + zPerX1 * x;
        double pz2 = zXConstant2 + zPerX2 * x;

        if(py1 == py2 && pz1 == pz2) {
            x = x;
            double y = py1;
            double z = py2;
            if(line[0] <= x && line[3] >= x) {
                if(line[1] <= y && line[4] >= y) {
                    if(line[2] <= z && line[5] >= z) {
                        if(line2[0] <= x && line2[3] >= x) {
                            if(line2[1] <= y && line2[4] >= y) {
                                if(line2[2] <= z && line2[5] >= z) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    int isEdgeIntersect(int n, const std::vector<GeometricVertex>& gv) {
        GeometricVertex v = gv[n];
        std::vector<GeometricVertex> a;

        for (size_t i = 0; i < gv.size(); i++) {
            if (i != n) {
                a.push_back(gv[i]);
            }
        }

        int c = 0, cnt = a.size(), prev = (cnt + (n - 1)) % cnt, next = n % cnt;

        if(prev < 0) {
            prev = a.size() - prev;
        }

        GeometricVertex v1 = a[prev], v2 = a[next];

        for (size_t i = 0, j = cnt - 1; i < cnt; j = i++)
        {
            if (prev == i || prev == j || next == i || next == j)
                continue;

            GeometricVertex v4 = a[j], v3 = a[i];

            float denominator = ((v4[1] - v3[1]) * (v2[0] - v1[0])) - ((v4[0] - v3[0]) * (v2[1] - v1[1]));

            if (!denominator)
                continue;

            float ua = (((v4[0] - v3[0]) * (v1[1] - v3[1])) - ((v4[1] - v3[1]) * (v1[0] - v3[0]))) / denominator;
            float ub = (((v2[0] - v1[0]) * (v1[1] - v3[1])) - ((v2[1] - v1[1]) * (v1[0] - v3[0]))) / denominator;

            //float x = v1[0] + (ua * (v2[0] - v1[0])), y = v1[1] + (ua * (v2[1] - v1[1]));

            if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1)
            {
                c = 1;
                break;
            }
        }

        return c;
    }

    int isVertexInsideNewPoly(int n, const std::vector<GeometricVertex>& gv)
    {
        GeometricVertex v = gv[n];
        std::vector<GeometricVertex> a;

        for (size_t i = 0; i < gv.size(); i++) {
            if (i != n) {
                a.push_back(gv[i]);
            }
        }

        int c = 1;

        for (size_t i = 0, j = a.size() - 1; i < a.size(); j = i++) 
        {
            if ((((a[i][1] <= v[1]) && (v[1] < a[j][1])) || ((a[j][1] <= v[1]) && (v[1] < a[i][1]))) && (v[0] > (a[j][0] - a[i][0]) * (v[1] - a[i][1]) / (a[j][1] - a[i][1]) + a[i][0]))
                c = !c;
        }

        return c;
    }

    float dist(GeometricVertex a, GeometricVertex b)
    {
        return sqrt(  ((a[0] - b[0]) * (a[0] - b[0])) + (((a[1] - b[1]) * (a[1] - b[1])))  );
    }

    int isVertexEar(int n, const std::vector<GeometricVertex>& gv) {
        return (isVertexInsideNewPoly(n, gv) && !isEdgeIntersect(n, gv));
    }

    void triangulateFace(std::vector<ObjFace>& faces, int faceIndex, std::vector<GeometricVertex>& gv, std::vector<TextureVertex>& tv, std::vector<NormalVertex>& nv) {
        ObjFace& face = faces.at(faceIndex); 

        glm::vec4 p1 = gv.at(0).geometricVertex;
        glm::vec4 p2 = gv.at(1).geometricVertex;
        glm::vec4 p3 = gv.at(2).geometricVertex;

        glm::vec3 vec1 = glm::vec3((p1 - p2).x, (p1 - p2).y, (p1 - p2).z);
        glm::vec3 vec2 = glm::vec3((p3 - p2).x, (p3 - p2).y, (p3 - p2).z);

        glm::vec3 cross = glm::cross(vec1, vec2);

        double d = cross.x * p1.x + cross.y * p1.y + cross.z * p1.z;
        double a = cross.x;
        double b = cross.y;
        double c = cross.z;
        glm::mat4x4 rotation = glm::mat4x4(1.0f);
        double translate = 0;
        if(c != 0) {
            translate = - d / c;
            double angle = acos(glm::dot(cross, glm::vec3(0, 0, 1)) / (cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2]));

            glm::vec3 axis = glm::cross(cross, glm::vec3(0, 0, 1));
            axis = axis / sqrt((axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]));
            axis = abs(axis);

            rotation = glm::rotate(rotation, (float)angle, axis);
        }

        std::vector<GeometricVertex> vertsFromFace = std::vector<GeometricVertex>();
        std::vector<TextureVertex> tVertsFromFace = std::vector<TextureVertex>();
        std::vector<NormalVertex> nVertsFromFace = std::vector<NormalVertex>();
        std::vector<int> indicesFromFace = std::vector<int>();
        std::vector<int> tIndicesFromFace = std::vector<int>();
        std::vector<int> nIndicesFromFace = std::vector<int>();

        for(int i = 0; i < face.indicesVG.size(); ++i) {
            glm::vec4 v = gv.at(face.indicesVG.at(i)).geometricVertex;
            GeometricVertex geov = GeometricVertex();
            geov.geometricVertex = rotation * (glm::vec4(0, 0, translate, 0) + v);
            vertsFromFace.push_back(geov);
            indicesFromFace.push_back(face.indicesVG.at(i));

            if(face.indicesVT.size() > 0) {
                tVertsFromFace.push_back(tv.at(face.indicesVT.at(i)));
                tIndicesFromFace.push_back(face.indicesVT.at(i));            
            }

            if(face.indicesVN.size() > 0) {
                nVertsFromFace.push_back(nv.at(face.indicesVN.at(i)));
                nIndicesFromFace.push_back(face.indicesVN.at(i));
            }
        }

        bool shouldExit = false;
        while(!shouldExit) {
            bool isDone = true;
            for(int j = 0; j < vertsFromFace.size(); ++j) {
                if(isVertexEar(j, vertsFromFace) && vertsFromFace.size() > 3) {
                    isDone = false;
                    int prevVert = (j > 0) ? j - 1 : vertsFromFace.size() - 1;
                    int nextVert = (j + 1 < vertsFromFace.size()) ? j + 1 : 0;
                    
                    ObjFace newFace = ObjFace();

                    newFace.indicesVG.push_back(indicesFromFace.at(j));
                    newFace.indicesVG.push_back(indicesFromFace.at(prevVert));
                    newFace.indicesVG.push_back(indicesFromFace.at(nextVert));

                    if(tIndicesFromFace.size() > 0) {
                        newFace.indicesVT.push_back(tIndicesFromFace.at(j));
                        newFace.indicesVT.push_back(tIndicesFromFace.at(prevVert));
                        newFace.indicesVT.push_back(tIndicesFromFace.at(nextVert));
                    }

                    if(nIndicesFromFace.size() > 0) {
                        newFace.indicesVN.push_back(nIndicesFromFace.at(j));
                        newFace.indicesVN.push_back(nIndicesFromFace.at(prevVert));
                        newFace.indicesVN.push_back(nIndicesFromFace.at(nextVert));
                    }

                    faces.push_back(newFace);

                    vertsFromFace.erase(vertsFromFace.begin() + j);

                    if(tVertsFromFace.size() > 0) {
                        tVertsFromFace.erase(tVertsFromFace.begin() + j);
                    }

                    if(nVertsFromFace.size() > 0) {
                        nVertsFromFace.erase(nVertsFromFace.begin() + j);
                    }

                    indicesFromFace.erase(indicesFromFace.begin() + j);

                    if(tIndicesFromFace.size() > 0) {
                        tIndicesFromFace.erase(tIndicesFromFace.begin() + j);
                    }

                    if(nIndicesFromFace.size() > 0) {
                        nIndicesFromFace.erase(nIndicesFromFace.begin() + j);
                    }
                    break;
                }else {
                    ObjFace newFace = ObjFace();

                    newFace.indicesVG.push_back(indicesFromFace.at(1));
                    newFace.indicesVG.push_back(indicesFromFace.at(0));
                    newFace.indicesVG.push_back(indicesFromFace.at(2));

                    if(tIndicesFromFace.size() > 0) {
                        newFace.indicesVT.push_back(tIndicesFromFace.at(1));
                        newFace.indicesVT.push_back(tIndicesFromFace.at(0));
                        newFace.indicesVT.push_back(tIndicesFromFace.at(2));
                    }

                    if(nIndicesFromFace.size() > 0) {
                        newFace.indicesVN.push_back(nIndicesFromFace.at(1));
                        newFace.indicesVN.push_back(nIndicesFromFace.at(0));
                        newFace.indicesVN.push_back(nIndicesFromFace.at(2));
                    }
                    faces.push_back(newFace);
                    shouldExit = true;
                    break;
                }
            }
            shouldExit = isDone;
        }
        faces.erase(faces.begin() + faceIndex);
    }

    void readVec3(std::stringstream& ss, glm::vec3& data) {
        std::string buffer = "";

        try {
            std::getline(ss, buffer, ' ');
                
            buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

            data.x = std::stof(buffer);

            std::getline(ss, buffer, ' ');
                    
            buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

            data.y = std::stof(buffer);

            std::getline(ss, buffer, ' ');
                    
            buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

            data.z = std::stof(buffer);
        }catch(std::exception e) {
            std::cout << "error reading stringstream into vec3: " << ss.str() << std::endl;
            abort();
        }
    }

    void readFloat(std::stringstream& ss, float& data) {
        std::string buffer = "";

        try {
            std::getline(ss, buffer, ' ');
                
            buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

            data = std::stof(buffer);
        }catch(std::exception e) {
            std::cout << "error reading stringstream into float: " << ss.str() << std::endl;
            abort();
        }
    }

    void readMaterialsFromFile(std::vector<Material>& globalMeshMaterials, std::string mtllibpath) {
        std::ifstream mtllib = std::ifstream(mtllibpath);

        Material currentMaterial = Material();

        std::string line = "";
        while(std::getline(mtllib, line)) {
            std::string buffer = "";
            std::stringstream linestream(line);

            std::getline(linestream, buffer, ' ');
                
            buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
            buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

            if(buffer == "newmtl") {
                if(currentMaterial.getName() != "") {
                    globalMeshMaterials.push_back(currentMaterial);
                }
                currentMaterial = Material();

                std::getline(linestream, buffer, ' ');
                    
                buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

                currentMaterial.setName(buffer);
            }else if(buffer == "Ka") {
                readVec3(linestream, currentMaterial.getAmbient());
            }else if(buffer == "Kd") {
                readVec3(linestream, currentMaterial.getDiffuse());
            }else if(buffer == "Ks") {
                readVec3(linestream, currentMaterial.getSpecular());
            }else if(buffer == "d") {
                readFloat(linestream, currentMaterial.getOpacity());
            }else if(buffer == "Tr") {
                readFloat(linestream, currentMaterial.getTransparency());
            }else if(buffer == "Ns") {
                readFloat(linestream, currentMaterial.getShininess());
            }else if(buffer == "illum") {
                std::getline(linestream, buffer, ' ');
                    
                buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

                try {
                    IllumModel lmodel = IllumModel(std::stoi(buffer));
                    currentMaterial.setLightingModel(lmodel);
                }catch(std::exception e) {
                    std::cout << "error reading illum def" << std::endl;
                }
            }else if(buffer == "map_Ka") {
                std::getline(linestream, buffer, ' ');
                    
                buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());

                currentMaterial.setTextureMapPath(buffer);
            }
        }

        if(currentMaterial.getName() != "") {
            globalMeshMaterials.push_back(currentMaterial);
        }
    }
};

ObjMesh ObjLoader::loadMeshFromFile(std::string pathToFile) {
    int currentMaterialID = -1;

    std::string mtllibname = "";
    std::vector<Material> globalMeshMaterials = std::vector<Material>();

    ObjMesh mesh = ObjMesh();

    std::ifstream meshStream(pathToFile);

    std::vector<std::string> lines = extractStringFromInputFileStream(meshStream);

    removeObjLineContinuesFromStringVector(lines);
    
    ObjMesh submesh = ObjMesh();
    submesh.setIsLeaf(true);
    std::vector<ObjFace>& faces = submesh.getPrimitive().faces;
    std::vector<Material>& materials = submesh.getPrimitive().materials;

    int numberOfVertices = 0;
    int verticesNumOffset = 0;
    bool onFacesDef = false;

    for(int i = 0; i < lines.size(); ++i) {
        std::string& line = lines.at(i);
        if(line.size() == 0) {

        }else if(line.at(0) == 'o') {
            mesh.name = line.substr(2);
        }else if(line.at(0) == 'g') {
            submesh.getPrimitive().materials = globalMeshMaterials;
            mesh.submeshes[submesh.name] = submesh;
            submesh = ObjMesh();
            submesh.setIsLeaf(true);
            submesh.name = line.substr(2);
            faces = submesh.getPrimitive().faces;
            materials = submesh.getPrimitive().materials;
        }else if(line.at(0) == '#') {

        }else if(line.substr(0, 6) == "mtllib") {
            mtllibname = line.substr(7);
            mtllibname.erase(std::remove(mtllibname.begin(), mtllibname.end(), ' '), mtllibname.end());
            mtllibname.erase(std::remove(mtllibname.begin(), mtllibname.end(), '\n'), mtllibname.end());
            mtllibname.erase(std::remove(mtllibname.begin(), mtllibname.end(), '\r'), mtllibname.end());
            if(mtllibname.substr(mtllibname.size() - 4) == ".mtl") {
                readMaterialsFromFile(globalMeshMaterials, mtllibname);
            }else {
                readMaterialsFromFile(globalMeshMaterials, mtllibname+".mtl");
            }
        }else if(line.substr(0, 6) == "usemtl") {
            std::string matname = line.substr(7);
            matname.erase(std::remove(matname.begin(), matname.end(), ' '), matname.end());
            matname.erase(std::remove(matname.begin(), matname.end(), '\n'), matname.end());
            matname.erase(std::remove(matname.begin(), matname.end(), '\r'), matname.end());
            auto is_equal = [matname](Material& mat){ 
                return mat.getName() == matname; 
            };
            auto iter = std::find_if(globalMeshMaterials.begin(), globalMeshMaterials.end(), is_equal);
            if(iter == globalMeshMaterials.end()) {
                std::cout << "material specified has not been loaded: " << matname << "while loading: " << pathToFile << std::endl;
                abort();
            }else {
                currentMaterialID = iter - globalMeshMaterials.begin();
            }
        }else {
            if(line.at(0) == 'v') {
                if(onFacesDef) {
                    onFacesDef = false;
                    numberOfVertices = 0;
                }

                if(line.substr(0, 2) == "v ") {
                    processVertexLine(submesh.getPrimitive(), line, 0);
                }else if(line.substr(0, 3) == "vt ") {
                    processVertexLine(submesh.getPrimitive(), line, 1);
                }else if(line.substr(0, 3) == "vn ") {
                    processVertexLine(submesh.getPrimitive(), line, 2);
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
                processFaceLine(faces, line, numberOfVertices, verticesNumOffset, currentMaterialID);
            }
        }
    }
    submesh.getPrimitive().materials = globalMeshMaterials;
    mesh.submeshes[submesh.name] = submesh;
    #ifdef PRINT_MESH_OUTPUT
        mesh.print();
    #endif

    return mesh;
}

ObjMesh ObjLoader::combineMeshes(std::vector<ObjMesh> submeshes, std::string name) {
    ObjMesh masterMesh = ObjMesh();
    masterMesh.name = name;

    for(ObjMesh smesh : submeshes) {
        masterMesh.submeshes[smesh.name] = smesh;
    }
    return masterMesh;
}

void ObjLoader::triangulateMesh(ObjMesh& combinedMeshes) {
    for(std::pair<const std::string, ObjMesh>& meshpair : combinedMeshes.submeshes) {
        if(!meshpair.second.isLeaf()) {
            for(std::pair<const std::string, ObjMesh>& submesh : meshpair.second.submeshes) {
                triangulateMesh(submesh.second);
            }
        }else {
            ObjMeshPrimitive& mesh = meshpair.second.getPrimitive();
            std::vector<ObjFace>& faces = mesh.faces;
            std::vector<GeometricVertex>& gv = mesh.geometricVertices;
            std::vector<TextureVertex>& tv = mesh.textureVertices;
            std::vector<NormalVertex>& nv = mesh.normalVertices;

            std::vector<ObjFace> newFaces = std::vector<ObjFace>();

            for(ObjFace& face : faces) {
                newFaces.push_back(face);
                triangulateFace(newFaces, newFaces.size() - 1, gv, tv, nv);
            }
            faces = newFaces;
        }
    }
}