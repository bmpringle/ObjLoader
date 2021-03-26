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

enum IllumModel {
    COLOR_ON_AMBIENT_OFF = 0, //0: Color on and Ambient off
    COLOR_ON_AMBIENT_ON = 1, //1: Color on and Ambient on
    HIGHLIGHT_ON = 2, //2: Highlight on
    REFLECTION_ON_RAYTRACE_ON = 3, //3: Reflection on and Ray trace on
    TRANSPARENCY_GLASS_ON_AND_REFLECTION_RAYTRACE_ON = 4, //4: Transparency: Glass on & Reflection: Ray trace on
    REFLECTION_FRESNEL_ON_AND_RAYTRACE_ON = 5, //5: Reflection: Fresnel on and Ray trace on
    TRANSPARENCY_REFRACTION_ON_AND_REFLECTION_FRESNEL_OFF_AND_RAYTRACE_ON = 6, //6: Transparency: Refraction on & Reflection: Fresnel off and Ray trace on
    TRANSPARENCY_REFRACTION_ON_AND_REFLECTION_FRESNEL_ON_AND_RAYTRACE_ON = 7, //7: Transparency: Refraction on & Reflection: Fresnel on and Ray trace on
    REFLECTION_ON_AND_RAYTRACE_OFF = 8, //8: Reflection on and Ray trace off
    TRANSPARENCY_GLASS_ON_AND_REFLECTION_RAYTRACE_OFF = 9, //9: Transparency: Glass on & Reflection: Ray trace off
    INVISIBLE_SURFACE_SHADOW_CAST = 10, //10: Casts shadows onto invisible surfaces
};

//class because stuff should be private
class Material {
    Material(std::string _name, IllumModel i) : name(_name), lightingModel(i) {

    };

    Material(std::string _name, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _opacity, float _shininess, IllumModel lmodel, std::string textureMapFilePath) : name(_name), lightingModel(lmodel) {
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        opacity = _opacity;
        transparency = 1 - opacity;
        shininess = _shininess;
        textureMapPath = textureMapFilePath;
    }

    //set methods
    void setName(std::string& _name) {
        name = _name;
    }

    void setAmbient(glm::vec3& _ambient) {
        ambient = _ambient;
    }

    void setDiffuse(glm::vec3& _diffuse) {
        diffuse = _diffuse;
    }

    void setSpecular(glm::vec3& _specular) {
        specular = _specular;
    }

    //also sets transparency to 1-opacity
    void setOpacity(float& _opacity) {
        opacity = _opacity;
        transparency = 1 - opacity;
    }

    //also sets opacity to 1-transparency
    void setTransparency(float& _transparency) {
        transparency = _transparency;
        opacity = 1 - transparency;
    }

    void setShininess(float& _shininess) {
        shininess = _shininess;
    }

    void setLightingModel(IllumModel& lmodel) {
        lightingModel = lmodel;
    }

    void setTextureMapPath(std::string& path) {
        textureMapPath = path;
    }

    //get methods
    std::string getName() {
        return name;
    }

    glm::vec3 getAmbient() {
        return ambient;
    }

    glm::vec3 setDiffuse() {
        return diffuse;
    }

    glm::vec3 setSpecular() {
        return specular;
    }

    float setOpacity() {
        return opacity;
    }

    float setTransparency() {
        return transparency;
    }

    float setShininess() {
        return shininess;
    }

    IllumModel setLightingModel() {
        return lightingModel;
    }

    std::string setTextureMapPath() {
        return textureMapPath;
    }

    private:
        std::string name; //name of the material (referenced in obj file)
        glm::vec3 ambient = glm::vec3(0.2, 0.2, 0.2); //ambient color
        glm::vec3 diffuse = glm::vec3(0.8, 0.8, 0.8); //diffuse color
        glm::vec3 specular = glm::vec3(1.0, 1.0, 1.0); //specular color
        float opacity = 1.0; //how opaque the material is
        float transparency = 0.0; //how transparent the material is
        float shininess = 0.0; //how shiny the material is
        IllumModel lightingModel;
        std::string textureMapPath = ""; //path to the texture map (should be ASCII dump of rgb values)
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
        for(GeometricVertex& vertex : geometricVertices) {
            std::cout << "v #" << i << ": " << vertex[0] << " "
            << vertex[1] << " " << vertex[2] << " "
            << vertex[3] << ", " << std::endl;
            ++i;
        }

        i = 1;
        for(TextureVertex& vertex : textureVertices) {
            std::cout << "vt #" << i << ": " << vertex[0] << " "
            << vertex[1] << " " << vertex[2] << ", " << std::endl;
            ++i;
        }

        i = 1;
        for(NormalVertex& vertex : normalVertices) {
            std::cout << "vn #" << i << ": " << vertex[0] << " "
            << vertex[1] << " " << vertex[2] << ", " << std::endl;
            ++i;
        }

        int facenumber = 1;
        for(ObjFace& face : faces) {
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
    ObjMesh(std::string name);

    ObjMesh();

    std::map<std::string, ObjMesh> submeshes = std::map<std::string, ObjMesh>();
    std::string name = "untitled";

    bool isLeaf();

    void setIsLeaf(bool isLeafBool);

    ObjMeshPrimitive& getPrimitive();

    ObjMesh& getMeshWithName(std::string name);

    void getPrimitives(std::vector<ObjMeshPrimitive>& prims);

    private:
        bool isLeafNode;
        ObjMeshPrimitive mesh = ObjMeshPrimitive();   
};
#endif