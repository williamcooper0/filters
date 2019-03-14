#include "update.h"
#include "../backgroundsubtraction.h"

namespace _Gpu
{

Update::Update(const BackgroundSubtraction *bs)
    : Program(NumUniforms)
    , _bs(bs)
{
    _fragmentShader =
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"                                                                            \n"
"uniform sampler2D u_gmmAndMean;                                             \n"
"uniform sampler2D u_data;                                                   \n"
"uniform sampler2D u_textureIn;                                              \n"
"                                                                            \n"
"uniform float     u_alphaT;                                                 \n"
"uniform float     u_prune;                                                  \n"
"uniform float     u_alpha1;                                                 \n"
"                                                                            \n"
"                                                                            \n"
"const int   mixtures        = " + str<int>  (_bs->mixtures       ) + ";     \n"
"const float varThresholdGen = " + str<float>(_bs->varThresholdGen) + ";     \n"
"const float varianceInitial = " + str<float>(_bs->varianceInitial) + ";     \n"
"const float varianceMin     = " + str<float>(_bs->varianceMin    ) + ";     \n"
"const float varianceMax     = " + str<float>(_bs->varianceMax    ) + ";     \n"
"                                                                            \n"
"                                                                            \n"
"int list[mixtures];                                                         \n"
"                                                                            \n"
"void swap(int index)                                                        \n"
"{                                                                           \n"
"    int tmp         = list[index];                                          \n"
"    list[index]     = list[index - 1];                                      \n"
"    list[index - 1] = tmp;                                                  \n"
"}                                                                           \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    float iX = v_position.x * 2.0;                                          \n"
"    iX -= float(int(iX));                                                   \n"
"    float halfIX = iX / 2.0;                                                \n"
"                                                                            \n"
"    float iY = v_position.y * float(mixtures);                              \n"
"    int mode = int(iY);                                                     \n"
"    iY -= float(mode);                                                      \n"
"                                                                            \n"
"    bool updateMean = v_position.x > 0.5;                                   \n"
"                                                                            \n"
"                                                                            \n"
"    float y[mixtures];                                                      \n"
"                                                                            \n"
"    vec2 gmm[mixtures];                                                     \n"
"    vec3 mean[mixtures];                                                    \n"
"                                                                            \n"
"    for(int m = 0; m < mixtures; m++) {                                     \n"
"        list[m] = m;                                                        \n"
"                                                                            \n"
"        y[m] = (iY + float(m)) / float(mixtures);                           \n"
"                                                                            \n"
"        gmm[m] = texture2D(u_gmmAndMean, vec2(halfIX, y[m])).rg;            \n"
"        if(updateMean)                                                      \n"
"            mean[m] = texture2D(u_gmmAndMean, vec2(0.5 + halfIX, y[m])      \n"
"                ).rgb;                                                      \n"
"    }                                                                       \n"
"                                                                            \n"
"                                                                            \n"
"    bool fitsPDF = false;                                                   \n"
"                                                                            \n"
"    int modes = mixtures;                                                   \n"
"    float totalWeight = 0.0;                                                \n"
"                                                                            \n"
"    for(int m = 0; m < modes; m++) {                                        \n"
"        vec4 data = texture2D(u_data, vec2(iX, y[m]));                      \n"
"                                                                            \n"
"        float weight = data.a;                                              \n"
"                                                                            \n"
"        if(!fitsPDF) {                                                      \n"
"            float var = gmm[m].y;                                           \n"
"                                                                            \n"
"            vec3 dData = data.rgb * 2.0 - vec3(1.0);                        \n"
"            float dist2 = dot(dData, dData);                                \n"
"                                                                            \n"
"            if(dist2 < varThresholdGen * var) {                             \n"
"                fitsPDF = true;                                             \n"
"                                                                            \n"
"                weight += u_alphaT;                                         \n"
"                float k = u_alphaT / weight;                                \n"
"                                                                            \n"
"                if(updateMean)                                              \n"
"                    mean[m] -= k * dData;                                   \n"
"                                                                            \n"
"                float varnew = (var + k * (dist2 * 255.0 - var)) * 255.0;   \n"
"                if(varnew < varianceMin)                                    \n"
"                    varnew = varianceMin;                                   \n"
"                if(varnew > varianceMax)                                    \n"
"                    varnew = varianceMax;                                   \n"
"                gmm[m].y = varnew;                                          \n"
"                                                                            \n"
"                for(int mm = m; mm > 0; mm--) {                             \n"
"                    if(weight < gmm[mm - 1].x)                              \n"
"                        break;                                              \n"
"                                                                            \n"
"                    swap(mm);                                               \n"
"                }                                                           \n"
"            }                                                               \n"
"        }                                                                   \n"
"                                                                            \n"
"        if(weight < -u_prune) {                                             \n"
"            weight = 0.0;                                                   \n"
"            modes--;                                                        \n"
"        }                                                                   \n"
"                                                                            \n"
"        gmm[m].x = weight;                                                  \n"
"        totalWeight += weight;                                              \n"
"    }                                                                       \n"
"                                                                            \n"
"    for(int m = 0; m < modes; m++)                                          \n"
"        gmm[m].x /= totalWeight;                                            \n"
"                                                                            \n"
"    modes = mixtures;                                                       \n"
"                                                                            \n"
"    if(!fitsPDF) {                                                          \n"
"        gmm[mixtures - 1].x = u_alphaT;                                     \n"
"        for(int m = 0; m < mixtures - 1; m++)                               \n"
"            gmm[m].x *= u_alpha1;                                           \n"
"                                                                            \n"
"        if(updateMean)                                                      \n"
"            mean[mixtures - 1] = texture2D(u_textureIn, vec2(iX, iY)).bgr;  \n"
"                                                                            \n"
"        gmm[mixtures - 1].y = varianceInitial;                              \n"
"                                                                            \n"
"        for(int m = mixtures - 1; m > 0; m--) {                             \n"
"            if(u_alphaT < gmm[m - 1].x)                                     \n"
"                break;                                                      \n"
"                                                                            \n"
"            swap(m);                                                        \n"
"        }                                                                   \n"
"    }                                                                       \n"
"                                                                            \n"
"                                                                            \n"
"    gl_FragColor = updateMean ? vec4(mean[list[mode]], 1.0) : vec4(gmm[     \n"
"        list[mode]], 0.0, 1.0);                                             \n"
"}                                                                           \n"
        ;
    create("update");


    createUniform(GmmAndMean, "u_gmmAndMean");
    createUniform(Data, "u_data");
    createUniform(TextureIn, "u_textureIn");

    createUniform(AlphaT, "u_alphaT");
    createUniform(Prune, "u_prune");
    createUniform(Alpha1, "u_alpha1");
}

void Update::execute(const Surface *in, Surface *out)
{
    Program::use(out);


    setTexture(GmmAndMean, _bs->gmmAndMean());
    setTexture(Data, _bs->data());
    setTexture(TextureIn, in);

    setUniform1f(AlphaT, _bs->alphaT());
    setUniform1f(Prune, _bs->prune());
    setUniform1f(Alpha1, _bs->alpha1());


    Program::execute();
}

}
