#ifndef LINEAR_BLUR_H
#define LINEAR_BLUR_H

#include "../../gpu/program.h"

namespace cv
{
    class Mat;
}

namespace _Gpu
{

class Surface;

class LinearBlur : public Program
{
public:
    LinearBlur();
    void execute(const Surface *in, Surface *out, GLuint resolution, float directionX, float directionY);

private:
    enum
    {
        TextureIn,
        Resolution,
        Direction,

        NumUniforms
    };


    void cvglGaussianBlur(double sigma, cv::Mat *kernel);
};

}

#endif // LINEAR_BLUR_H
