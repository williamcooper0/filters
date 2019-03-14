#include "camera.h"

#include <opencv2/videoio.hpp>

#include <iostream>

using namespace cv;
using namespace std;

VideoCapture *Camera::_capture;
Mat *Camera::_image;

int Camera::_width;
int Camera::_height;

void Camera::init()
{
    _capture = new VideoCapture(0);

    if(!_capture->isOpened()) {
        cerr << "Failed to open capture\n";
        exit(0);
    }

    _image = new Mat;

    readFrame();

    _width = _image->cols;
    _height = _image->rows;
}

void Camera::clear()
{
    delete _image;

    _capture->release();
    delete _capture;
}

void Camera::readFrame()
{
    if(!_capture->read(*_image)) {
        cerr << "Failed to read frame\n";
        exit(0);
    }
}

Mat *Camera::image()
{
    return _image;
}

int Camera::width()
{
    return _width;
}

int Camera::height()
{
    return _height;
}
