#ifndef BLEND_H
#define BLEND_H

#include "../../gpu/program.h"

namespace _Gpu
{

class Surface;

class Blend : public Program
{
public:
    Blend();
    void execute(const Surface *gradientXIn, Surface *gradientYIn, Surface *out);

private:
    enum
    {
        GradientX,
        GradientY,

        NumUniforms
    };
};

}

#endif // BLEND_H
