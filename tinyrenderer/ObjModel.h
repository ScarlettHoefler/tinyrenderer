//
//  ObjModel.h
//  tinyrenderer
//
//  Created by Scarlett Hoefler on 1/26/19.
//  Copyright © 2019 Scarlett Hoefler. All rights reserved.
//

#ifndef ObjModel_hpp
#define ObjModel_hpp

#include <string>

class ObjModel {
public:
    ObjModel() = default;
    
    void LoadFromFile(std::string filePath);
};

#endif /* ObjModel_hpp */
