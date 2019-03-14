#ifndef CONVERT_H
#define CONVERT_H

#include "program.h"

namespace _Gpu
{

class Surface;

class Convert : public Program
{
public:
    Convert();
    void execute(const Surface *in, Surface *out);

private:
    enum
    {
        TextureIn,

        NumUniforms
    };
};

}

#endif // CONVERT_H
