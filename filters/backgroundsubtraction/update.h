#ifndef UPDATE_H
#define UPDATE_H

#include "../../gpu/program.h"

namespace _Gpu
{

class BackgroundSubtraction;
class Surface;

class Update : public Program
{
public:
    Update(const BackgroundSubtraction *bs);
    void execute(const Surface *in, Surface *out);

private:
    enum
    {
        GmmAndMean,
        Data,
        TextureIn,

        AlphaT,
        Prune,
        Alpha1,


        NumUniforms
    };


    const BackgroundSubtraction *_bs;
};

}

#endif // UPDATE_H
