#ifndef APPLICATION_H
#define APPLICATION_H

class Device;

class Application
{
public:
    static void init(int argc, char **argv);
    static Device *device();
    static void clear();
    static void usage();

private:
    static const char *gpu;
    static const char *cpu;

    static Device *_device;
};

#endif // APPLICATION_H
