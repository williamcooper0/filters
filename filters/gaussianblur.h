#ifndef GAUSSIAN_BLUR_H
#define GAUSSIAN_BLUR_H

#include "../filter.h"

class GaussianBlur : public Filter
{
public:
    static const char *name;

protected:
    GaussianBlur();
};

namespace _Gpu
{

class Surface;
class LinearBlur;

class GaussianBlur : public ::GaussianBlur
{
public:
    GaussianBlur();
    ~GaussianBlur();

    void draw(const ::Surface *in, ::Surface *out);

private:
    Surface *_tmp;
    LinearBlur *_linearBlur;
};

}

namespace _Cpu
{

class GaussianBlur : public ::GaussianBlur
{
public:
    void draw(const ::Surface *in, ::Surface *out);
};

}

#endif // GAUSSIAN_BLUR_H
