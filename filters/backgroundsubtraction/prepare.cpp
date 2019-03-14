#include "prepare.h"
#include "../backgroundsubtraction.h"
#include "../../surface.h"

namespace _Gpu
{

Prepare::Prepare(BackgroundSubtraction *bs)
    : Program(NumUniforms)

    , _bs(bs)
    , _frames(0)
{
    _fragmentShader =
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"                                                                            \n"
"uniform sampler2D u_gmmAndMean;                                             \n"
"uniform float     u_gmmAndMeanWidth;                                        \n"
"                                                                            \n"
"uniform sampler2D u_textureIn;                                              \n"
"uniform float     u_textureInHeight;                                        \n"
"                                                                            \n"
"uniform vec2      u_outSize;                                                \n"
"                                                                            \n"
"uniform float     u_alpha1;                                                 \n"
"uniform float     u_prune;                                                  \n"
"                                                                            \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    float weightX = (v_position.x - 0.5 / u_outSize.x) / 2.0 + 0.5          \n"
"        / u_gmmAndMeanWidth;                                                \n"
"                                                                            \n"
"    float weight = texture2D(u_gmmAndMean, vec2(weightX, v_position.y)).r;  \n"
"    vec3 mean = texture2D(u_gmmAndMean, vec2(weightX + 0.5,                 \n"
"        v_position.y)).rgb;                                                 \n"
"                                                                            \n"
"    float imageY = v_position.y * u_outSize.y / u_textureInHeight;          \n"
"    vec3 color = texture2D(u_textureIn, vec2(v_position.x, imageY           \n"
"        - float(int(imageY)))).bgr;                                         \n"
"                                                                            \n"
"                                                                            \n"
"    float outWeight = u_alpha1 * weight + u_prune;                          \n"
"    vec3 outDData = (mean - color + vec3(1.0)) / 2.0;                       \n"
"                                                                            \n"
"    gl_FragColor = vec4(outDData, outWeight);                               \n"
"}                                                                           \n"
        ;
    create("prepare");


    createUniform(GmmAndMean, "u_gmmAndMean");
    createUniform(GmmAndMeanWidth, "u_gmmAndMeanWidth");

    createUniform(TextureIn, "u_textureIn");
    createUniform(TextureInHeight, "u_textureInHeight");

    createUniform(OutSize, "u_outSize");

    createUniform(Alpha1, "u_alpha1");
    createUniform(Prune, "u_prune");
}

void Prepare::execute(const Surface *in, Surface *out)
{
    Program::use(out);


    setTexture(GmmAndMean, _bs->gmmAndMean());
    setUniform1f(GmmAndMeanWidth, _bs->gmmAndMean()->width());
    setTexture(TextureIn, in);
    setUniform1f(TextureInHeight, in->height());
    setUniform2f(OutSize, out->width(), out->height());

    float learningRate;
    if(_bs->initialLearningRate >= 0) {
        if(_frames > 0) {
            learningRate = _bs->initialLearningRate;
        }
        else {
            _frames++;
            const int frames2 = 2 * _frames;
            const bool beginning = frames2 < _bs->history;
            learningRate = 1.0 / (beginning ? frames2 : _bs->history);
        }
    }
    else {
        const int frames2 = 2 * (_frames + 1);
        const bool beginning = frames2 < _bs->history;

        if(beginning)
            _frames++;

        learningRate = 1.0 / (beginning ? frames2 : _bs->history);
    }
    const float alphaT = learningRate;
    _bs->setAlphaT(alphaT);
    const float alpha1 = 1 - alphaT;
    _bs->setAlpha1(alpha1);
    setUniform1f(Alpha1, alpha1);

    const float prune = -learningRate * _bs->CT;
    _bs->setPrune(prune);
    setUniform1f(Prune, prune);


    Program::execute();
}

}
