#include "sobeloperator.h"

#include "sobeloperator/gradient.h"
#include "sobeloperator/blend.h"

#include "../camera.h"
#include "../surface.h"


#include <opencv2/video.hpp>


using namespace cv;


const char *SobelOperator::name("sobel operator");

SobelOperator::SobelOperator()
    : Filter(name)
{

}


namespace _Gpu
{

SobelOperator::SobelOperator()
    : _gradX(new Surface(Camera::width(), Camera::height()))
    , _gradY(new Surface(Camera::width(), Camera::height()))

    , _gradient(new Gradient)
    , _blend(new Blend)
{

}

SobelOperator::~SobelOperator()
{
    delete _blend;
    delete _gradient;

    delete _gradY;
    delete _gradX;
}

void SobelOperator::draw(const ::Surface *in, ::Surface *out)
{
    const Surface *gIn = static_cast<const Surface*>(in);
    Surface *gOut = static_cast<Surface*>(out);


    _gradient->execute(gIn, _gradX, Gradient::X);
    _gradient->execute(gIn, _gradY, Gradient::Y);

    _blend->execute(_gradX, _gradY, gOut);
}

}


namespace _Cpu
{

SobelOperator::SobelOperator()
    : _tmp(new Mat)

    , _gradX(new Mat)
    , _gradY(new Mat)
{

}

SobelOperator::~SobelOperator()
{
    delete _gradY;
    delete _gradX;

    delete _tmp;
}

void SobelOperator::draw(const ::Surface *in, ::Surface *out)
{
    const Mat &cIn = *static_cast<const Surface*>(in)->mat();
    Mat &cOut = *static_cast<Surface*>(out)->mat();

    const int depth = CV_32F;
    const int size = 3;
    const double scale = 1;
    const double delta = 0;
    const int border = BORDER_DEFAULT;

    Sobel(cIn, *_tmp, depth, 1, 0, size, scale, delta, border);
    convertScaleAbs(*_tmp, *_gradX);

    Sobel(cIn, *_tmp, depth, 0, 1, size, scale, delta, border);
    convertScaleAbs(*_tmp, *_gradY);

    addWeighted(*_gradX, 0.5, *_gradY, 0.5, 0, cOut);
}

}
