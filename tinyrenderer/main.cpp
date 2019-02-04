#include "tgaimage.h"
#include "ObjModel.h"
#include "Vector.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <random>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);

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

void line(Vector2i v0, Vector2i v1, TGAImage &image, TGAColor color) {
    line(v0.x, v0.y, v1.x, v1.y, image, color);
}

Vector3f getBarycentricCoordinatesForScreenPoint(Vector3f triangleA, Vector3f triangleB, Vector3f triangleC, Vector3f screenPoint) {
    // P = A + u(AB) + v(AC)
    // u(AB) + v(AC) + (PA) = 0
    // then expand out to each coordinate: u(AB)x + v(AC)x + (PA)x = 0; same for Y coords
    // to find where these intersect, we can use a cross product. See: https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling#the-method-i-adopt-for-my-code
    Vector3f vAB = triangleB - triangleA;
    Vector3f vAC = triangleC - triangleA;
    Vector3f vPA = triangleA - screenPoint;
    
    Vector3f xVector(vAB.x, vAC.x, vPA.x);
    Vector3f yVector(vAB.y, vAC.y, vPA.y);
    Vector3f xCrossY = xVector.cross(yVector);
    
    // HACK: avoid divide by zero
    if (std::abs(xCrossY.z) < std::numeric_limits<float>::epsilon()) {
        // Just return something with a negative; we'll discard it later anyway
        return Vector3f(-1.f,1.f,1.f);
    }
    
    float v = xCrossY.y / xCrossY.z;
    float w = xCrossY.x / xCrossY.z;
    float u = 1.f - ((xCrossY.x + xCrossY.y) / xCrossY.z); // (1-v-w) will give us gaps :c Floating point inaccuracy?
    return Vector3f(u,v,w);
}

template<typename T>
T clamp(T val, T min, T max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

void triangle(Vector3f v0, Vector3f v1, Vector3f v2, TGAImage &image, float* zBuffer, TGAColor color) {
    Vector3f points[] = {v0, v1, v2};
    
    // Find the bounding rect of our triangle (in pixel coords) so that we only have to iterate over a small area
    const float floatMin = std::numeric_limits<float>::min();
    const float floatMax = std::numeric_limits<float>::max();
    float minX = floatMax;
    float minY = floatMax;
    float maxX = floatMin;
    float maxY = floatMin;
    for (int pointIndex = 0; pointIndex < 3; ++pointIndex) {
        Vector3f point = points[pointIndex];
        minX = std::min(minX, point.x);
        minY = std::min(minY, point.y);
        maxX = std::max(maxX, point.x);
        maxY = std::max(maxY, point.y);
    }
    
    // Clamp to image bounds
    const float imageCoordMaxX = image.get_width() - 1;
    const float imageCoordMaxY = image.get_height() - 1;
    minX = clamp(minX, 0.f, imageCoordMaxX);
    minY = clamp(minY, 0.f, imageCoordMaxY);
    maxX = clamp(maxX, 0.f, imageCoordMaxX);
    maxY = clamp(maxY, 0.f, imageCoordMaxX);
    
    // For each point in our bounding rect, check if our point is within our triangle. If so, draw!
    int minScreenX = (int)std::floor(minX);
    int maxScreenX = (int)std::ceil(maxX);
    int minScreenY = (int)std::floor(minY);
    int maxScreenY = (int)std::ceil(maxY);
    for (int xPos = minScreenX; xPos <= maxScreenX; ++xPos) {
        for (int yPos = minScreenY; yPos <= maxScreenY; ++yPos) {
            Vector3f screenPoint(xPos, yPos, 0);
            Vector3f barycentricCoords = getBarycentricCoordinatesForScreenPoint(v0, v1, v2, screenPoint);
            bool isPointInsideTriangle =    (barycentricCoords.x >= 0)
                                         && (barycentricCoords.y >= 0)
                                         && (barycentricCoords.z >= 0);
            
            if (isPointInsideTriangle) {
                image.set(xPos, yPos, color);
            }
        }
    }
}

void drawHeadWireframe(TGAImage &image) {
    ObjModel model;
    model.loadFromFile("obj/head.obj");
    
    for (int faceIndex = 0; faceIndex < model.numFaces(); ++faceIndex) {
        ModelFace face = model.faceAtIndex(faceIndex);
        const int VerticesPerFace = 3;
        for (int i = 0; i < VerticesPerFace; ++i) {
            Vector3f v0 = model.vertexAtIndex(face[i]);
            Vector3f v1 = model.vertexAtIndex(face[(i+1) % VerticesPerFace]);
            int x0 = (v0.x + 1.f) * ImageWidth / 2.f;
            int y0 = (v0.y + 1.f) * ImageHeight / 2.f;
            int x1 = (v1.x + 1.f) * ImageWidth / 2.f;
            int y1 = (v1.y + 1.f) * ImageHeight / 2.f;
            line(x0, y0, x1, y1, image, white);
        }
    }
}

void drawHeadShaded(TGAImage &image, float* zBuffer) {
    ObjModel model;
    model.loadFromFile("obj/head.obj");
    
    Vector3f lightDirection(0,0,-1.f);
    
    const size_t numFaces = model.numFaces();
    for (int faceIndex = 0; faceIndex < numFaces; ++faceIndex) {
        ModelFace face = model.faceAtIndex(faceIndex);
        Vector3f faceWorldCoords[3];
        Vector3f faceScreenCoords[3];
        for (int iCoord = 0; iCoord < 3; ++iCoord) {
            Vector3f worldCoords = model.vertexAtIndex(face[iCoord]);
            faceWorldCoords[iCoord] = worldCoords;
            
            int xPos = (worldCoords.x + 1.f) * ImageWidth / 2.f;
            int yPos = (worldCoords.y + 1.f) * ImageHeight / 2.f;
            faceScreenCoords[iCoord] = Vector3f(xPos, yPos, 0);
        }
        
        // Calculate color for triangle
        Vector3f edge1 = faceWorldCoords[2] - faceWorldCoords[0];
        Vector3f edge2 = faceWorldCoords[1] - faceWorldCoords[0];
        Vector3f normal = (edge1.cross(edge2)).normalized();
        float intensity = normal.dot(lightDirection);
        
        if (intensity > 0) { // HACK: This does janky backface culling that isn't entirely correct
            int greyIntensity = static_cast<int>(255 * intensity); // TODO: Gamma correction. (128,128,128) is not half as bright as (255,255,255)
            TGAColor color(greyIntensity, greyIntensity, greyIntensity, 255);
            triangle(faceScreenCoords[0], faceScreenCoords[1], faceScreenCoords[2], image, zBuffer, color);
        }
    }
}


int main(int argc, char** argv) {
    TGAImage image(ImageWidth, ImageHeight, TGAImage::RGB);
    const size_t zBufferSize = ImageWidth * ImageHeight;
    float zBuffer[zBufferSize];
    for (int i = 0; i < zBufferSize; ++i) {
        zBuffer[i] = std::numeric_limits<float>::min();
    }

    drawHeadShaded(image, zBuffer);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
