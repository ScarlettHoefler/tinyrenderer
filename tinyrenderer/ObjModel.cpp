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
    m_faces.clear();
    
    std::string nextLine;
    while (!inputStream.eof()) {
        std::getline(inputStream, nextLine);
        
        std::istringstream lineStream(nextLine);
        std::string stringDiscard;

        if (nextLine.compare(0, 2, "v ") == 0) {
            // Vector
            // e.g. "v -0.000581696 -0.734665 -0.623267"
            Vector3 vertexData;
            std::string prefix;
            lineStream >> stringDiscard; // get rid of the "v "
            lineStream >> vertexData.x >> vertexData.y >> vertexData.z;
            m_vertices.push_back(vertexData);
        } else if (nextLine.compare(0, 2, "f ") == 0) {
            // Face
            // e.g. "f 1258/1339/1258 1208/1256/1208 1206/1252/1206"
            // Get the first index of each group to get the 3 vertices of our wireframe triangle, e.g. 1258 1208 1206
            lineStream >> stringDiscard; // get rid of the "f "
            ModelFace modelFace;
            lineStream >> modelFace.a >> stringDiscard >> modelFace.b >> stringDiscard >> modelFace.c;
            
            // OBJ indices are 1-based, so we'll just subtract one to get it to map to our 0-based array
            modelFace.a -= 1;
            modelFace.b -= 1;
            modelFace.c -= 1;
            
            m_faces.push_back(modelFace);
        }
    }
}

Vector3 ObjModel::vertexAtIndex(int index) const {
    assert(index >= 0 && index < m_vertices.size());
    return m_vertices[index];
}

ModelFace ObjModel::faceAtIndex(int index) const {
    assert(index >= 0 && index < m_faces.size());
    return m_faces[index];
}
