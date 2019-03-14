#ifndef SURFACE_H
#define SURFACE_H

#include <GLES2/gl2.h>

namespace cv
{
    class Mat;
}

class Surface
{
public:
    virtual ~Surface();

protected:
    Surface();
};

namespace _Gpu
{

class Surface : public ::Surface
{
public:
    Surface(int width, int height, bool screen = false);
    void write(GLint format, const unsigned char *data) const;

    ~Surface();


    int width() const;
    int height() const;

    GLuint texture() const;
    void bind() const;

private:
    int _width;
    int _height;

    GLuint _texture;
    GLuint _framebuffer;


    void createTexture();
    void createFramebuffer(bool screen);

    void bindTexture() const;
};

}

namespace _Cpu
{

class Surface : public ::Surface
{
public:
    Surface();
    Surface(cv::Mat *mat);

    ~Surface();


    cv::Mat *mat() const;

private:
    cv::Mat *_mat;
    bool _wrap;
};

}

#endif // SURFACE_H
