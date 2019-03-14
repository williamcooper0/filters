#ifndef PROGRAM_H
#define PROGRAM_H

#include <GLES2/gl2.h>
#include <iomanip>

namespace _Gpu
{

class Surface;

class Program
{
public:
    static void init();
    static void clear();


    virtual ~Program();

protected:
    std::string _fragmentShader;


    Program(int numUniforms);
    void create(const std::string &name);

    void createUniform(int index, const char *name);

    void use(Surface *out);
    void execute() const;


    void setUniform1i(int index, int value) const;
    void setUniform1f(int index, float value) const;
    void setUniform2f(int index, float value1, float value2) const;

    void setTexture(int index, const Surface *surface);


    template<typename T> std::string str(T t) const;

private:
    static GLfloat *_vertices;
    static std::string _vertexShader;
    static int _textureIndex;


    GLuint _program;
    GLint *_uniforms;

    GLuint _vertex;
    GLuint _fragment;


    GLuint createShader(GLenum type, const std::string &source);
};

template<typename T>
std::string Program::str(T t) const
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << t;
    return ss.str();
}

}

#endif // PROGRAM_H
