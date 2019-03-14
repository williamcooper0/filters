#include "blend.h"

namespace _Gpu
{

Blend::Blend()
    : Program(NumUniforms)
{
    _fragmentShader =
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"uniform sampler2D u_gradientX;                                              \n"
"uniform sampler2D u_gradientY;                                              \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    vec3 x = texture2D(u_gradientX, v_position).rgb;                        \n"
"    vec3 y = texture2D(u_gradientY, v_position).rgb;                        \n"
"                                                                            \n"
"    vec3 result;                                                            \n"
"    for(int i = 0; i < 3; i++)                                              \n"
"        result[i] = sqrt(x[i] * x[i] + y[i] * y[i]);                        \n"
"                                                                            \n"
"    gl_FragColor = vec4(result, 1.0);                                       \n"
"}                                                                           \n"
        ;
    create("blend");

    createUniform(GradientX, "u_gradientX");
    createUniform(GradientY, "u_gradientY");
}

void Blend::execute(const Surface *gradientXIn, Surface *gradientYIn, Surface *out)
{
    Program::use(out);

    setTexture(GradientX, gradientXIn);
    setTexture(GradientY, gradientYIn);

    Program::execute();
}

}
