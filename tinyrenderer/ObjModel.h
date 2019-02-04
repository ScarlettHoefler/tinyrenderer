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

struct ModelFace {
    int a,b,c;
    
    int operator[](int i) {
        assert(i >= 0 && i <= 2);
        
        // TODO: There is almost certainly a more elegant way to do this... a union, perhaps?
        switch(i) {
            case 0:
                return a;
            case 1:
                return b;
            case 2:
                return c;
            default:
                return a; // Error
        }
    }
};

class ObjModel {
public:
    ObjModel() = default;
    
    void loadFromFile(std::string filePath);
    
    size_t numFaces() const { return m_faces.size(); }
    Vector3f vertexAtIndex(int index) const;
    ModelFace faceAtIndex(int index) const;
    
private:
    std::vector<Vector3f> m_vertices;
    std::vector<ModelFace> m_faces;
};

#endif /* ObjModel_hpp */
