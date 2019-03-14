#ifndef SOBEL_OPERATOR_H
#define SOBEL_OPERATOR_H

#include "../filter.h"

namespace cv
{
    class Mat;
}

class SobelOperator : public Filter
{
public:
    static const char *name;

protected:
    SobelOperator();
};

namespace _Gpu
{


class Surface;

class Gradient;
class Blend;


class SobelOperator : public ::SobelOperator
{
public:
    SobelOperator();
    ~SobelOperator();

    void draw(const ::Surface *in, ::Surface *out);

private:
    Surface *_gradX;
    Surface *_gradY;

    Gradient *_gradient;
    Blend *_blend;
};

}

namespace _Cpu
{

class SobelOperator : public ::SobelOperator
{
public:
    SobelOperator();
    ~SobelOperator();

    void draw(const ::Surface *in, ::Surface *out);

private:
    cv::Mat *_tmp;

    cv::Mat *_gradX;
    cv::Mat *_gradY;
};

}

#endif // SOBEL_OPERATOR_H
