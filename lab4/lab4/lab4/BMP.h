#ifndef BITMAP_H
#define BITMAP_H

#include <string>
#include <vector>

class Pixel
{
public:
    int red, green, blue;

    Pixel() : red(0), green(0), blue(0) {}

    Pixel(int r, int g, int b) : red(r), green(g), blue(b) {}

    void set(int r, int g, int b)
    {
        red = r;
        green = g;
        blue = b;
    }
};

typedef std::vector < std::vector <Pixel> > PixelMatrix;


class Bitmap
{
private:
    PixelMatrix pixels;

public:
    void open(std::string);

    void save(std::string);

    bool isImage();

    PixelMatrix toPixelMatrix();

    void fromPixelMatrix(const PixelMatrix&);

};

#endif