#include "gradient.h"
#include "../../camera.h"

#include <opencv2/core/mat.hpp>

using namespace cv;

namespace _Gpu
{

Gradient::Gradient()
    : Program(NumUniforms)
{
    const int ksize = 3;
    const float scale = 1;

    Mat kx, ky;
    cvglSobel(&kx, &ky, ksize);

    const float *kxp = kx.ptr<float>();
    const float *kyp = ky.ptr<float>();

    _fragmentShader = std::string("") +
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"uniform sampler2D u_textureIn;                                              \n"
"uniform int       u_direction;                                              \n"
"                                                                            \n"
"                                                                            \n"
"const float width  = float(" + str<int>(Camera::width() ) + ");             \n"
"const float height = float(" + str<int>(Camera::height()) + ");             \n"
"                                                                            \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    const int ksize = " + str<int>(ksize) + ";                              \n"
"    float kernels[2 * ksize * ksize];                                       \n"
"                                                                            \n"
"    const int X = 0;                                                        \n"
"    const int Y = 1;                                                        \n"
        ;

    for(int y = 0; y < ksize; y++) {
        for(int x = 0; x < ksize; x++) {
            _fragmentShader +=
"    kernels[X * ksize * ksize + " + str<int>(y * ksize + x) + "] = "
                + str<float>(kyp[y] * kxp[x] * scale) + ";                   \n"
"    kernels[Y * ksize * ksize + " + str<int>(y * ksize + x) + "] = "
                + str<float>(kxp[y] * kyp[x] * scale) + ";                   \n"
                ;
        }
    }

    _fragmentShader +=
"    vec3 color = vec3(0.0);                                                 \n"
"                                                                            \n"
"    for(int y = 0; y < ksize; y++) {                                        \n"
"        for(int x = 0; x < ksize; x++)                                      \n"
"            color += kernels[u_direction * ksize * ksize + y * ksize + x] * \n"
"                texture2D(u_textureIn, v_position + vec2(float(x - ksize /  \n"
"                2) / width, float(y - ksize / 2) / height)).rgb;            \n"
"    }                                                                       \n"
"                                                                            \n"
"    for(int i = 0; i < 3; i++) {                                            \n"
"        if(color[i] < 0.0)                                                  \n"
"            color[i] = -color[i];                                           \n"
"    }                                                                       \n"
"                                                                            \n"
"    gl_FragColor = vec4(color, 1.0);                                        \n"
"}                                                                           \n"
        ;

    create("gradient");

    createUniform(TextureIn, "u_textureIn");
    createUniform(Direction , "u_direction");
}

void Gradient::execute(const Surface *in, Surface *out, Dir direction)
{
    Program::use(out);

    setTexture(TextureIn, in);
    setUniform1i(Direction, direction);

    Program::execute();
}

void Gradient::cvglSobel(Mat *kx, Mat *ky, int ksize)
{
    int ksizeX = ksize;
    int ksizeY = ksize;

    if(ksizeX == 1)
        ksizeX = 3;

    kx->create(Size(ksizeX, 1), CV_32F);
    ky->create(Size(ksizeY, 1), CV_32F);

    std::vector<int> kerI(std::max(ksizeX, ksizeY) + 1);

    for(int k = 0; k < 2; k++) {
        Mat *kernel = k == 0 ? kx : ky;
        int order = k == 0 ? 1 : 0;
        ksize = k == 0 ? ksizeX : ksizeY;

        if(ksize == 1)
            kerI[0] = 1;
        else if(ksize == 3) {
            if(order == 0)
                kerI[0] =  1, kerI[1] =  2, kerI[2] = 1;
            else if(order == 1)
                kerI[0] = -1, kerI[1] =  0, kerI[2] = 1;
            else
                kerI[0] =  1, kerI[1] = -2, kerI[2] = 1;
        }
        else {
            int oldval, newval;
            kerI[0] = 1;
            for(int i = 0; i < ksize; i++)
                kerI[i+1] = 0;

            for(int i = 0; i < ksize - order - 1; i++) {
                oldval = kerI[0];
                for(int j = 1; j <= ksize; j++) {
                    newval = kerI[j]+kerI[j-1];
                    kerI[j-1] = oldval;
                    oldval = newval;
                }
            }

            for(int i = 0; i < order; i++) {
                oldval = -kerI[0];
                for(int j = 1; j <= ksize; j++) {
                    newval = kerI[j-1] - kerI[j];
                    kerI[j-1] = oldval;
                    oldval = newval;
                }
            }
        }

        Mat temp(kernel->rows, kernel->cols, CV_32S, &kerI[0]);
        temp.convertTo(*kernel, CV_32F, 1);
    }
}

}
