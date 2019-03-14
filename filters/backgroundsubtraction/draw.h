#ifndef DRAW_H
#define DRAW_H

#include "../../gpu/program.h"

namespace _Gpu
{

class BackgroundSubtraction;
class Surface;

class Draw : public Program
{
public:
    Draw(const BackgroundSubtraction *bs);
    void execute(const Surface *in, Surface *out);

private:
    enum
    {
        GmmAndMean,
        Data,
        TextureIn,

        Prune,


        NumUniforms
    };


    const BackgroundSubtraction *_bs;
};

}

#endif // DRAW_H
