#ifndef PREPARE_H
#define PREPARE_H

#include "../../gpu/program.h"

namespace _Gpu
{

class BackgroundSubtraction;
class Surface;

class Prepare : public Program
{
public:
    Prepare(BackgroundSubtraction *bs);
    void execute(const Surface *in, Surface *out);

private:
    enum
    {
        GmmAndMean,
        GmmAndMeanWidth,

        TextureIn,
        TextureInHeight,

        OutSize,

        Alpha1,
        Prune,


        NumUniforms
    };


    BackgroundSubtraction *_bs;
    int _frames;
};

}

#endif // PREPARE_H
