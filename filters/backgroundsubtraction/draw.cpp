#include "draw.h"
#include "../backgroundsubtraction.h"

namespace _Gpu
{

Draw::Draw(const BackgroundSubtraction *bs)
    : Program(NumUniforms)
    , _bs(bs)
{
    _fragmentShader = std::string("") +
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"                                                                            \n"
"uniform sampler2D u_gmmAndMean;                                             \n"
"uniform sampler2D u_data;                                                   \n"
"uniform sampler2D u_textureIn;                                              \n"
"                                                                            \n"
"uniform float     u_prune;                                                  \n"
"                                                                            \n"
"                                                                            \n"
"const int   mixtures        = " + str<int>  (_bs->mixtures       ) + ";     \n"
"const float varThreshold    = " + str<float>(_bs->varThreshold   ) + ";     \n"
"const float varThresholdGen = " + str<float>(_bs->varThresholdGen) + ";     \n"
"const float backgroundRatio = " + str<float>(_bs->backgroundRatio) + ";     \n"
"const int   shadowDetection = " + str<bool> (_bs->shadowDetection) + ";     \n"
"const float shadow          = " + str<float>(_bs->shadow         ) + ";     \n"
"const float tau             = " + str<float>(_bs->tau            ) + ";     \n"
"                                                                            \n"
"                                                                            \n"
"vec2 getGmmCoord(int mode)                                                  \n"
"{                                                                           \n"
"    return vec2(v_position.x / 2.0, (v_position.y + float(mode)) /          \n"
"        float(mixtures));                                                   \n"
"}                                                                           \n"
"                                                                            \n"
"vec3 getGmmAndMean(vec2 coord)                                              \n"
"{                                                                           \n"
"    return texture2D(u_gmmAndMean, coord).rgb;                              \n"
"}                                                                           \n"
"                                                                            \n"
"bool detectShadowGMM()                                                      \n"
"{                                                                           \n"
"    float tWeight = 0.0;                                                    \n"
"    vec3 data = texture2D(u_textureIn, v_position).bgr;                     \n"
"                                                                            \n"
"    for(int m = 0; m < mixtures; m++) {                                     \n"
"        vec2 gmmCoord = getGmmCoord(m);                                     \n"
"        vec2 gmm = getGmmAndMean(gmmCoord).rg;                              \n"
"                                                                            \n"
"        vec3 mean = getGmmAndMean(vec2(0.5 + gmmCoord.x, gmmCoord.y));      \n"
"                                                                            \n"
"        float numerator = dot(data, mean);                                  \n"
"        float denominator = dot(mean, mean);                                \n"
"                                                                            \n"
"        if(denominator == 0.0)                                              \n"
"            return false;                                                   \n"
"                                                                            \n"
"        if(numerator <= denominator && numerator >= tau * denominator) {    \n"
"            float a = numerator / denominator;                              \n"
"            vec3 dD = a * mean - data;                                      \n"
"            float dist2a = dot(dD, dD);                                     \n"
"                                                                            \n"
"            if(dist2a < varThreshold * gmm.g * a * a)                       \n"
"                return true;                                                \n"
"        }                                                                   \n"
"                                                                            \n"
"        tWeight += gmm.r;                                                   \n"
"        if(tWeight > backgroundRatio)                                       \n"
"            return false;                                                   \n"
"    }                                                                       \n"
"                                                                            \n"
"    return false;                                                           \n"
"}                                                                           \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    bool background = false;                                                \n"
"                                                                            \n"
"    int modes = mixtures;                                                   \n"
"    float totalWeight = 0.0;                                                \n"
"                                                                            \n"
"    for(int m = 0; m < modes; m++) {                                        \n"
"        vec2 gmmCoord = getGmmCoord(m);                                     \n"
"        vec2 gmm = getGmmAndMean(gmmCoord).rg;                              \n"
"                                                                            \n"
"        float var = gmm.g;                                                  \n"
"                                                                            \n"
"        vec4 data = texture2D(u_data, vec2(v_position.x, gmmCoord.y));      \n"
"                                                                            \n"
"        vec3 dData = data.rgb * 2.0 - vec3(1.0);                            \n"
"        float dist2 = dot(dData, dData);                                    \n"
"                                                                            \n"
"        if(dist2 < varThreshold * var) {                                    \n"
"            background = true;                                              \n"
"            break;                                                          \n"
"        }                                                                   \n"
"                                                                            \n"
"        if(dist2 < varThresholdGen * var)                                   \n"
"            break;                                                          \n"
"                                                                            \n"
"        if(data.a < -u_prune)                                               \n"
"            modes--;                                                        \n"
"        else {                                                              \n"
"            totalWeight += data.a;                                          \n"
"                                                                            \n"
"            if(totalWeight >= backgroundRatio)                              \n"
"                break;                                                      \n"
"        }                                                                   \n"
"    }                                                                       \n"
"                                                                            \n"
"    gl_FragColor = vec4(vec3(background ? 0.0 : bool(shadowDetection) &&    \n"
"        detectShadowGMM() ? shadow : 1.0), 1.0);                            \n"
"}                                                                           \n"
        ;
    create("draw");


    createUniform(GmmAndMean, "u_gmmAndMean");
    createUniform(Data, "u_data");
    createUniform(TextureIn, "u_textureIn");

    createUniform(Prune, "u_prune");
}

void Draw::execute(const Surface *in, Surface *out)
{
    Program::use(out);


    setTexture(GmmAndMean, _bs->gmmAndMean());
    setTexture(Data, _bs->data());
    setTexture(TextureIn, in);

    setUniform1f(Prune, _bs->prune());


    Program::execute();
}

}
