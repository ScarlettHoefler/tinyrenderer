//
//  ObjModel.h
//  tinyrenderer
//
//  Created by Scarlett Hoefler on 1/26/19.
//  Copyright © 2019 Scarlett Hoefler. All rights reserved.
//

#ifndef ObjModel_hpp
#define ObjModel_hpp

#include <vector>
#include <string>

#include "Vector.hpp"

struct ModelVertex {
    int positionIndex;
    int texCoordIndex;
};

struct ModelFace {
    ModelVertex vertices[3];
};

class ObjModel {
public:
    ObjModel() = default;
    
    void loadFromFile(std::string filePath);
    
    size_t numFaces() const { return m_faces.size(); }
    Vector3f vertexAtIndex(int index) const;
    Vector2f texCoordAtIndex(int index) const;
    ModelFace faceAtIndex(int index) const;
    
private:
    std::vector<Vector3f> m_vertices;
    std::vector<Vector2f> m_textureCoordinates;
    std::vector<ModelFace> m_faces;
};

#endif /* ObjModel_hpp */
