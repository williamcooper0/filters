#include "surface.h"

#include <opencv2/core.hpp>

#include <iostream>

using namespace cv;


Surface::~Surface()
{

}

Surface::Surface()
{

}


namespace _Gpu
{

Surface::Surface(int width, int height, bool screen)
    : _width(width)
    , _height(height)
{
    createTexture();
    createFramebuffer(screen);
}

void Surface::write(GLint format, const unsigned char *data) const
{
    bindTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
}

Surface::~Surface()
{
    glDeleteFramebuffers(1, &_framebuffer);
    glDeleteTextures(1, &_texture);
}

int Surface::width() const
{
    return _width;
}

int Surface::height() const
{
    return _height;
}

GLuint Surface::texture() const
{
    return _texture;
}

void Surface::bind() const
{
    glViewport(0, 0, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
}

void Surface::createTexture()
{
    glGenTextures(1, &_texture);
    bindTexture();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    write(GL_RGBA, nullptr);
}

void Surface::createFramebuffer(bool screen)
{
    if(screen) {
        _framebuffer = 0;
        return;
    }

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);


    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Failed to create framebuffer: " << status << '\n';
        exit(0);
    }
}

void Surface::bindTexture() const
{
    glBindTexture(GL_TEXTURE_2D, _texture);
}

}


namespace _Cpu
{

Surface::Surface()
    : _mat(new Mat)
    , _wrap(false)
{

}

Surface::Surface(Mat *mat)
    : _mat(mat)
    , _wrap(true)
{

}

Surface::~Surface()
{
    if(!_wrap)
        delete _mat;
}

Mat *Surface::mat() const
{
    return _mat;
}

}
