#include "device.h"
#include "camera.h"
#include "pipeline.h"
#include "surface.h"

#include "gpu/convert.h"

#include "filters/gaussianblur.h"
#include "filters/backgroundsubtraction.h"
#include "filters/sobeloperator.h"


#include <opencv2/opencv.hpp>


extern "C" {
#include "eglut/eglut.h"
}


using namespace cv;


Device::~Device()
{

}

Device::Device()
{

}

void Device::read() const
{
    Camera::readFrame();
}

void Device::draw(const Surface *in, Surface *out) const
{
    Pipeline::draw(in, out);
}


namespace _Gpu
{

Device *Device::_instance;

Device::Device()
{
    _instance = this;

    const int width = Camera::width();
    const int height = Camera::height();

    eglutInitWindowSize(width, height);
    eglutInitAPIMask(EGLUT_OPENGL_ES2_BIT);
    eglutInit(0, 0);
    eglutCreateWindow("gpu");

    eglutIdleFunc(eglutIdle);
    eglutDisplayFunc(eglutDisplay);


    _textureIn = _surface();
    _converted = _surface();
    _fboOut = new Surface(width, height, true);

    Program::init();
    _convert = new Convert;
}

Device::~Device()
{
    delete _convert;
    Program::clear();

    delete _fboOut;
    delete _converted;
    delete _textureIn;
}

void Device::loop() const
{
    eglutMainLoop();
}

::Surface *Device::surface() const
{
    return _surface();
}

Filter *Device::gaussianBlur() const
{
    return new GaussianBlur;
}

Filter *Device::backgroundSubtraction() const
{
    return new BackgroundSubtraction;
}

Filter *Device::sobelOperator() const
{
    return new SobelOperator;
}

void Device::eglutIdle()
{
    eglutPostRedisplay();
}

void Device::eglutDisplay()
{
    _instance->update();
}

void Device::update()
{
    read();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _textureIn->write(GL_RGB, Camera::image()->ptr());
    _convert->execute(_textureIn, _converted);

    draw(_converted, _fboOut);
}

Surface *Device::_surface() const
{
    return new Surface(Camera::width(), Camera::height());
}

}


namespace _Cpu
{

Device::Device()
    : _windowName("cpu")
    , _result(_surface())
{
    namedWindow(_windowName);
}

Device::~Device()
{
    delete _result;
}

void Device::loop() const
{
    for(;;) {
        read();

        const Surface in(Camera::image());
        draw(&in, _result);

        imshow(_windowName, *_result->mat());

        if(!process())
            return;
    }
}

::Surface *Device::surface() const
{
    return _surface();
}

Filter *Device::gaussianBlur() const
{
    return new GaussianBlur;
}

Filter *Device::backgroundSubtraction() const
{
    return new BackgroundSubtraction;
}

Filter *Device::sobelOperator() const
{
    return new SobelOperator;
}

bool Device::process() const
{
    return waitKey(1) != 27;
}

Surface *Device::_surface() const
{
    return new _Cpu::Surface;
}

}
