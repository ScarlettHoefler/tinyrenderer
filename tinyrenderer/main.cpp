#include "tgaimage.h"
#include "ObjModel.h"
#include <iostream>
#include <cassert>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

const int ImageWidth = 800;
const int ImageHeight = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    const bool tallerThanWide = std::abs(y0-y1) > std::abs(x0-x1);
    
    bool axesTransposed = false;
    if (tallerThanWide) {
        // We want to iterate over the Y coordinate instead of the X coordinate to make sure we fill out the entire line; otherwise we'll have gaps
        // To avoid needing two nearly-identical code paths, instead we'll just swap our axes and then swap them back before drawing
        std::swap(x0,y0);
        std::swap(x1,y1);
        axesTransposed = true;
    }
    
    if (x0 > x1) {
        // To simplify our looping logic, ensure we're drawing left-to-right by switching start and end coordinates
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    for (int x = x0; x <= x1; ++x) {
        float t = (float)(x-x0) / (x1-x0);
        
        int targetX = x;
        int targetY = y0 + (t * (y1-y0));
        
        if (axesTransposed) {
            std::swap(targetX, targetY); // Flip the coords back to the correct axes before drawing
        }
        
        image.set(targetX, targetY, color);
    }
}

void drawModel(const ObjModel& model, TGAImage &image) {
    for (int faceIndex = 0; faceIndex < model.numFaces(); ++faceIndex) {
        ModelFace face = model.faceAtIndex(faceIndex);
        const int VerticesPerFace = 3;
        for (int i = 0; i < VerticesPerFace; ++i) {
            Vector3 v0 = model.vertexAtIndex(face[i]);
            Vector3 v1 = model.vertexAtIndex(face[(i+1) % VerticesPerFace]);
            int x0 = (v0.x + 1.f) * ImageWidth / 2.f;
            int y0 = (v0.y + 1.f) * ImageHeight / 2.f;
            int x1 = (v1.x + 1.f) * ImageWidth / 2.f;
            int y1 = (v1.y + 1.f) * ImageHeight / 2.f;
            line(x0, y0, x1, y1, image, white);
        }
    }
}


int main(int argc, char** argv) {
	TGAImage image(ImageWidth, ImageHeight, TGAImage::RGB);
    
    ObjModel model;
    model.loadFromFile("obj/head.obj");
    drawModel(model, image);
    
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

