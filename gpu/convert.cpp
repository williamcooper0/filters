#include "convert.h"

namespace _Gpu
{

Convert::Convert()
    : Program(NumUniforms)
{
    _fragmentShader =
"precision highp float;                                                      \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"uniform sampler2D u_textureIn;                                              \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    gl_FragColor = texture2D(u_textureIn, vec2(v_position.x, 1.0 -          \n"
"        v_position.y)).bgra;                                                \n"
"}                                                                           \n"
        ;
    create("convert");
    createUniform(TextureIn, "u_textureIn");
}

void Convert::execute(const Surface *in, Surface *out)
{
    Program::use(out);
    setTexture(TextureIn, in);
    Program::execute();
}

}
