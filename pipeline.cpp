#include "pipeline.h"
#include "device.h"
#include "application.h"
#include "timer.h"
#include "filter.h"
#include "surface.h"

#include <iostream>

using namespace std;

vector<Filter*> Pipeline::_filters;
Surface *Pipeline::_tmp;

void Pipeline::init(int argc, char **argv)
{
    const Device *device = Application::device();
    _tmp = device->surface();

    for(int i = 0; i < argc; i++) {
        const string filterName = argv[i];
        Filter *filter;

        if(filterName == "b")
            filter = device->backgroundSubtraction();
        else if(filterName == "g")
            filter = device->gaussianBlur();
        else if(filterName == "s")
            filter = device->sobelOperator();
        else {
            Application::clear();
            Application::usage();
        }

        _filters.push_back(filter);
    }
}

void Pipeline::draw(const Surface *in, Surface *out)
{
    const int last = _filters.size() - 1;

    for(int i = 0; i <= last; i++) {
        Timer::start();
        _filters[i]->draw(i == 0    ? in  : _tmp,
                          i == last ? out : _tmp);
        Timer::end();
    }

    cout << '\n';
}

void Pipeline::clear()
{
    for(int i = 0; i < _filters.size(); i++)
        delete _filters[i];

    delete _tmp;
}
