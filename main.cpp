#include "application.h"
#include "device.h"

int main(int argc, char **argv)
{
    Application::init(argc, argv);
    Application::device()->loop();
    Application::clear();
    return 0;
}
