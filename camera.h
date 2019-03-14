#ifndef CAMERA_H
#define CAMERA_H

namespace cv
{
    class VideoCapture;
    class Mat;
}

class Camera
{
public:
    static void init();
    static void clear();

    static void readFrame();
    static cv::Mat *image();

    static int width();
    static int height();

private:
    static cv::VideoCapture *_capture;
    static cv::Mat *_image;

    static int _width;
    static int _height;
};

#endif // CAMERA_H
