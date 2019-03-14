#ifndef DEVICE_H
#define DEVICE_H

class Surface;
class Filter;

class Device
{
public:
    virtual ~Device();

    virtual void loop() const = 0;
    virtual Surface *surface() const = 0;

    virtual Filter *gaussianBlur() const = 0;
    virtual Filter *backgroundSubtraction() const = 0;
    virtual Filter *sobelOperator() const = 0;

protected:
    Device();

    void read() const;
    void draw(const Surface *in, Surface *out) const;
};

namespace _Gpu
{

class Surface;
class Convert;

class Device : public ::Device
{
public:
    Device();
    ~Device();

    void loop() const;
    ::Surface *surface() const;

    Filter *gaussianBlur() const;
    Filter *backgroundSubtraction() const;
    Filter *sobelOperator() const;

private:
    static Device *_instance;


    static void eglutIdle();
    static void eglutDisplay();


    Surface *_in;
    Surface *_converted;
    Surface *_out;

    Convert *_convert;


    void update();
    Surface *_surface() const;
};

}

namespace _Cpu
{

class Surface;

class Device : public ::Device
{
public:
    Device();
    ~Device();

    void loop() const;
    ::Surface *surface() const;

    Filter *gaussianBlur() const;
    Filter *backgroundSubtraction() const;
    Filter *sobelOperator() const;

private:
    const char *_windowName;
    Surface *_out;


    bool process() const;
    Surface *_surface() const;
};

}

#endif // DEVICE_H
