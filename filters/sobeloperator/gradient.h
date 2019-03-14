#ifndef GRADIENT_H
#define GRADIENT_H

#include "../../gpu/program.h"

namespace cv
{
    class Mat;
}

namespace _Gpu
{

class Surface;

class Gradient : public Program
{
public:
    enum Dir
    {
        X = 0,
        Y = 1
    };


    Gradient();
    void execute(const Surface *in, Surface *out, Dir direction);

private:
    enum
    {
        TextureIn,
        Direction,

        NumUniforms
    };


    void cvglSobel(cv::Mat *kx, cv::Mat *ky, int ksize);
};

}

#endif // GRADIENT_H
