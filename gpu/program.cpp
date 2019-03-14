#include "program.h"
#include "../surface.h"

#include <cstring>
#include <iostream>

using namespace std;

namespace _Gpu
{

GLfloat *Program::_vertices;
string Program::_vertexShader;
int Program::_textureIndex;

void Program::init()
{
    _vertices = new GLfloat[8] {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1
    };
    _vertexShader =
"attribute vec2 a_position;                                                  \n"
"                                                                            \n"
"varying vec2 v_position;                                                    \n"
"                                                                            \n"
"void main()                                                                 \n"
"{                                                                           \n"
"    gl_Position = vec4(a_position, 0.0, 1.0);                               \n"
"    v_position = (a_position + vec2(1.0)) / 2.0;                            \n"
"}                                                                           \n"
        ;
}

void Program::clear()
{
    delete[] _vertices;
}

Program::~Program()
{
    glDeleteShader(_fragment);
    glDeleteShader(_vertex);

    glUseProgram(0);
    glDeleteProgram(_program);

    delete[] _uniforms;
}

Program::Program(int numUniforms)
    : _program(0)
    , _uniforms(new GLint[numUniforms])
{

}

void Program::create(const string &name)
{
    cout << "    " << name << '\n';

    _vertex = createShader(GL_VERTEX_SHADER, _vertexShader);
    _fragment = createShader(GL_FRAGMENT_SHADER, _fragmentShader);

    _program = glCreateProgram();

    glAttachShader(_program, _vertex);
    glAttachShader(_program, _fragment);

    glBindAttribLocation(_program, 0, "a_position");
    glLinkProgram(_program);

    char msg[512];
    msg[0] = '\0';
    glGetProgramInfoLog(_program, sizeof(msg), 0, msg);
    cout << "\n        program info: " << msg << '\n';
}

void Program::createUniform(int index, const char *name)
{
    _uniforms[index] = glGetUniformLocation(_program, name);
}

void Program::use(Surface *out)
{
    out->bind();
    glUseProgram(_program);
    _textureIndex = 0;
}

void Program::execute() const
{
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, _vertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFinish();
}

void Program::setUniform1i(int index, int value) const
{
    glUniform1i(_uniforms[index], value);
}

void Program::setUniform1f(int index, float value) const
{
    glUniform1f(_uniforms[index], value);
}

void Program::setUniform2f(int index, float value1, float value2) const
{
    glUniform2f(_uniforms[index], value1, value2);
}

void Program::setTexture(int index, const Surface *surface)
{
    glActiveTexture(GL_TEXTURE0 + _textureIndex);
    glBindTexture(GL_TEXTURE_2D, surface->texture());
    setUniform1i(index, _textureIndex);
    _textureIndex++;
}

GLuint Program::createShader(GLenum type, const string &source)
{
    GLuint shader = glCreateShader(type);

    char *cStr = new char[source.size() + 1];
    strncpy(cStr, source.data(), source.size());
    cStr[source.size()] = 0;
    glShaderSource(shader, 1, &cStr, 0);
    delete[] cStr;

    glCompileShader(shader);

    char log[512];
    log[0] = '\0';
    glGetShaderInfoLog(shader, sizeof(log), 0, log);
    cout << "        " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader info: " << log << '\n';

    return shader;
}

}
