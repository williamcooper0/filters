#include "device.h"
#include "camera.h"
#include "pipeline.h"
#include "surface.h"

#include "gpu/convert.h"

#include "filters/gaussianblur.h"
#include "filters/backgroundsubtraction.h"
#include "filters/sobeloperator.h"


#include <opencv2/highgui.hpp>


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


    _in = _surface();
    _converted = _surface();
    _out = new Surface(width, height, true);

    Program::init();
    _convert = new Convert;
}

Device::~Device()
{
    delete _convert;
    Program::clear();

    delete _out;
    delete _converted;
    delete _in;
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
    _in->write(GL_RGB, Camera::image()->ptr());

    if(Pipeline::filters().empty())
        _convert->execute(_in, _out);
    else {
        _convert->execute(_in, _converted);
        draw(_converted, _out);
    }
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
    , _out(_surface())
{
    namedWindow(_windowName);
}

Device::~Device()
{
    delete _out;
}

void Device::loop() const
{
    for(;;) {
        read();
	const Mat *image = Camera::image();

	const Mat *result;

	if(Pipeline::filters().empty())
	    result = image;
        else {
            const Surface in(Camera::image());
            draw(&in, _out);
	    result = _out->mat();
        }
        
        imshow(_windowName, *result);

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
