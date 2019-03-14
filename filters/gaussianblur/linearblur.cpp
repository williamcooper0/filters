#include "linearblur.h"
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

namespace _Gpu
{

LinearBlur::LinearBlur()
    : Program(NumUniforms)
{
    const double sigma = 3;
    Mat kernel;
    cvglGaussianBlur(sigma, &kernel);

    _fragmentShader = string("") +
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"uniform sampler2D u_textureIn;                                              \n"
"uniform float     u_resolution;                                             \n"
"uniform vec2      u_direction;                                              \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    const int kernelSize = " + str<int>(kernel.rows) + ";                   \n"
"    float kernel[kernelSize];                                               \n"
        ;

    for(int i = 0; i < kernel.rows; i++) {
        _fragmentShader +=
"    kernel[" + str<int>(i) + "] = "
              + str<float>(kernel.ptr<float>()[i]) + ";                      \n"
            ;
    }

    _fragmentShader +=
"    const int halfSize = kernelSize / 2;                                    \n"
"                                                                            \n"
"                                                                            \n"
"    vec4 sum = vec4(0.0);                                                   \n"
"                                                                            \n"
"    for(int i = 0; i < kernelSize; i++)                                     \n"
"        sum += texture2D(u_textureIn, v_position + u_direction * float(i -  \n"
"            halfSize) / u_resolution) * kernel[i];                          \n"
"                                                                            \n"
"    gl_FragColor = vec4(sum.rgb, 1.0);                                      \n"
"}                                                                           \n"
        ;

    create("linear blur");


    createUniform(TextureIn, "u_textureIn");
    createUniform(Resolution, "u_resolution");
    createUniform(Direction, "u_direction");
}

void LinearBlur::execute(const Surface *in, Surface *out, GLuint resolution, float directionX, float directionY)
{
    Program::use(out);


    setTexture(TextureIn, in);
    setUniform1f(Resolution, resolution);
    setUniform2f(Direction, directionX, directionY);


    Program::execute();
}

void LinearBlur::cvglGaussianBlur(double sigma, Mat *kernel)
{
    const int n = cvRound(sigma * 4 * 2 + 1) | 1;
    kernel->create(n, 1, CV_32F);
    float* cf = kernel->ptr<float>();

    double scale2X = -0.5/(sigma*sigma);
    double sum = 0;

    int i;
    for( i = 0; i < n; i++ )
    {
        double x = i - (n-1)*0.5;
        double t = exp(scale2X*x*x);

        cf[i] = (float)t;
        sum += cf[i];
    }

    sum = 1./sum;
    for( i = 0; i < n; i++ )
        cf[i] = (float)(cf[i]*sum);
}

}
