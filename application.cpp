#include "application.h"
#include "camera.h"
#include "device.h"
#include "pipeline.h"

#include "filters/backgroundsubtraction.h"
#include "filters/gaussianblur.h"
#include "filters/sobeloperator.h"


#include <iostream>


using namespace std;

const char *Application::gpu("gpu");
const char *Application::cpu("cpu");

Device *Application::_device;

void Application::init(int argc, char **argv)
{
    if(argc < 3)
        usage();

    Camera::init();

    const string deviceName = argv[1];
    if(deviceName == gpu)
        _device = new _Gpu::Device;
    else if(deviceName == cpu)
        _device = new _Cpu::Device;
    else
        usage();

    Pipeline::init(argc - 2, &argv[2]);
}

Device *Application::device()
{
    return _device;
}

void Application::clear()
{
    Pipeline::clear();
    delete _device;
    Camera::clear();
}

void Application::usage()
{
    string str = string("") +
        "Usage: ./filters {" + gpu + " | " + cpu + "} {b | g | s} {b | g | s} ...\n"
        "    b - " + BackgroundSubtraction::name + "\n"
        "    g - " + GaussianBlur::name + "\n"
        "    s - " + SobelOperator::name + "\n"
        "\n"
        "Examples: ./filters gpu s\n"
        "          ./filters cpu b\n"
        "          ./filters gpu b g s\n"
        "          ./filters cpu g g g g g\n";
    cout << str;
    exit(0);
}
