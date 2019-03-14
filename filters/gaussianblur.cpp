#include "gaussianblur.h"
#include "gaussianblur/linearblur.h"

#include "../camera.h"
#include "../surface.h"


#include <opencv2/video.hpp>


using namespace cv;


const char *GaussianBlur::name("gaussian blur");

GaussianBlur::GaussianBlur()
    : Filter(name)
{

}


namespace _Gpu
{

GaussianBlur::GaussianBlur()
    : _tmp(new Surface(Camera::width(), Camera::height()))
    , _linearBlur(new LinearBlur)
{

}

GaussianBlur::~GaussianBlur()
{
    delete _linearBlur;
    delete _tmp;
}

void GaussianBlur::draw(const ::Surface *in, ::Surface *out)
{
    const Surface *gIn = static_cast<const Surface*>(in);
    Surface *gOut = static_cast<Surface*>(out);

    _linearBlur->execute(gIn, _tmp, Camera::width(), 1, 0);
    _linearBlur->execute(_tmp, gOut, Camera::height(), 0, 1);
}

}


namespace _Cpu
{

void GaussianBlur::draw(const ::Surface *in, ::Surface *out)
{
    const Mat &cIn = *static_cast<const Surface*>(in)->mat();
    Mat &cOut = *static_cast<Surface*>(out)->mat();

    const int size = 25;
    cv::GaussianBlur(cIn, cOut, Size(size, size), 3);
}

}
