#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    int width = std::abs(x0-x1);
    int height = std::abs(y0-y1);
    
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    for (int x = x0; x <= x1; ++x) {
        float t = (float)(x-x0) / width;
        
        int targetY = y0 + (t * height);
        std::cout << "x = " << x << ", y = " << targetY << "; t = " << t << std::endl;
        
        image.set(x, targetY, color);
    }
}



int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
    
    line(13, 20, 80, 40, image, white);
    line(20, 13, 40, 80, image, red);
    line(80, 40, 13, 20, image, red);
    
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

