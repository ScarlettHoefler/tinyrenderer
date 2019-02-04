#include "tgaimage.h"
#include "ObjModel.h"
#include <iostream>
#include <cassert>
#include <cmath>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);

const int ImageWidth = 800;
const int ImageHeight = 800;

template<typename T>
struct Vector2 {
    union {
        struct { T u, v; };
        struct { T x, y; };
        T raw[2];
    };
    
    Vector2(T x, T y)
    : x(x), y(y)
    {
    }
};

typedef Vector2<int> Vector2i;

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

void line(Vector2i v0, Vector2i v1, TGAImage &image, TGAColor color) {
    line(v0.x, v0.y, v1.x, v1.y, image, color);
}

void triangle(Vector2i v0, Vector2i v1, Vector2i v2, TGAImage &image, TGAColor color) {
    // Sort the 3 vectors from lowest to highest
    // 123 -> xx -> xx -> 123
    // 132 -> xx -> 123 -> 123
    // 213 -> 123 -> xx -> 123
    // 231 -> xx -> 213 -> 123
    // 312 -> 132 -> 123 -> 123
    // 321 -> 231 -> 213 -> 123
    if (v0.y > v1.y) {
        std::swap(v0, v1);
    }
    if (v1.y > v2.y) {
        std::swap(v1, v2);
    }
    if (v0.y > v1.y) {
        std::swap(v0, v1);
    }
    
    // Draw bottom half of triangle
    for (int curY = v0.y; curY <= v1.y; ++curY) {
        // Two sides: t0 to t1, and t0 to t2
        int yDistV0toV1 = v1.y - v0.y;
        int startY = v0.y;
        float tToV1 = (float)(curY - startY) / yDistV0toV1;
        std::cout << "t: " << tToV1 << std::endl;
        float x1 = v0.x + (v1.x - v0.x) * tToV1;
        
        int yDistV0toV2 = v2.y - v0.y;
        float tToV2 = (float)(curY - startY) / yDistV0toV2;
        float x2 = v0.x + (v2.x - v0.x) * tToV2;
        
        if (x1 > x2) { std::swap(x1, x2); }
        
        int leftX = (int)floor(x1);
        int rightX = (int)ceil(x2);
        line(leftX, curY, rightX, curY, image, color);
    }
    
    // Draw top half of triangle
    for (int curY = v1.y; curY <= v2.y; ++curY) {
        int yDistV0toV2 = v2.y - v0.y;
        int yDistV1toV2 = v2.y - v1.y;
        
        float tV0toV2 = (float)(curY - v0.y) / yDistV0toV2;
        float tV1toV2 = (float)(curY - v1.y) / yDistV1toV2;
        float x1 = v0.x + (v2.x - v0.x) * tV0toV2;
        float x2 = v1.x + (v2.x - v1.x) * tV1toV2;
        
        if (x1 > x2) { std::swap(x1, x2); }
        
        int leftX = (int)floor(x1);
        int rightX = (int)ceil(x2);
        line(leftX, curY, rightX, curY, image, color);
    }
    
    //* // Debug draw over
    line(v0, v1, image, blue);
    line(v1, v2, image, blue);
    line(v2, v0, image, blue);
    //*/
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

void drawHead(TGAImage &image) {
    ObjModel model;
    model.loadFromFile("obj/head.obj");
    drawModel(model, image);
}


int main(int argc, char** argv) {
	TGAImage image(ImageWidth, ImageHeight, TGAImage::RGB);
    
    Vector2i t0[3] = {Vector2i(10, 70),   Vector2i(50, 160),  Vector2i(70, 80)};
    Vector2i t1[3] = {Vector2i(180, 50),  Vector2i(150, 1),   Vector2i(70, 180)};
    Vector2i t2[3] = {Vector2i(180, 150), Vector2i(120, 160), Vector2i(130, 180)};
    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);
    
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

