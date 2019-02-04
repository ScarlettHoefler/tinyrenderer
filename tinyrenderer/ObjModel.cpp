//
//  ObjModel.cpp
//  tinyrenderer
//
//  Created by Scarlett Hoefler on 1/26/19.
//  Copyright © 2019 Scarlett Hoefler. All rights reserved.
//

#include "ObjModel.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

void ObjModel::loadFromFile(std::string filePath) {
    std::ifstream inputStream(filePath);
    if (!inputStream.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return;
    }
    
    m_vertices.clear();
    m_textureCoordinates.clear();
    m_faces.clear();
    
    std::string nextLine;
    while (!inputStream.eof()) {
        std::getline(inputStream, nextLine);
        
        std::istringstream lineStream(nextLine);
        char charDiscard;
        std::string stringDiscard;

        if (nextLine.compare(0, 2, "v ") == 0) {
            // Vector
            // e.g. "v -0.000581696 -0.734665 -0.623267"
            Vector3f vertexData;
            lineStream >> stringDiscard; // get rid of the "v "
            lineStream >> vertexData.x >> vertexData.y >> vertexData.z;
            m_vertices.push_back(vertexData);
        } else if (nextLine.compare(0, 3, "vt ") == 0) {
            // Texture coordinates
            // e.g. "vt 0.500 1"
            Vector2f texCoord;
            lineStream >> stringDiscard; // get rid of "vt "
            lineStream >> texCoord.x >> texCoord.y;
            m_textureCoordinates.push_back(texCoord);
        } else if (nextLine.compare(0, 2, "f ") == 0) {
            // Face
            // e.g. "f 1258/1339/1258 1208/1256/1208 1206/1252/1206"
            // Each entry is "position/texture/normal"
            lineStream >> stringDiscard; // get rid of the "f "
            int positions[3];
            int texCoords[3];
            lineStream >> positions[0] >> charDiscard >> texCoords[0] >> stringDiscard
                       >> positions[1] >> charDiscard >> texCoords[1] >> stringDiscard
                       >> positions[2] >> charDiscard >> texCoords[2] >> stringDiscard;
            
            ModelFace modelFace;
            for (int i = 0; i < 3; ++i) {
                // OBJ indices are 1-based, so we subtract one to get it to map to our 0-based array
                modelFace.vertices[i].positionIndex = positions[i] - 1;
                modelFace.vertices[i].texCoordIndex = texCoords[i] - 1;
            }
            
            m_faces.push_back(modelFace);
        }
    }
}

Vector3f ObjModel::vertexAtIndex(int index) const {
    assert(index >= 0 && index < m_vertices.size());
    return m_vertices[index];
}

Vector2f ObjModel::texCoordAtIndex(int index) const {
    assert(index >= 0 && index < m_textureCoordinates.size());
    return m_textureCoordinates[index];
}

ModelFace ObjModel::faceAtIndex(int index) const {
    assert(index >= 0 && index < m_faces.size());
    return m_faces[index];
}
