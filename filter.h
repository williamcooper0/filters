#ifndef FILTER_H
#define FILTER_H

#include <string>

class Surface;

class Filter
{
public:
    virtual ~Filter();
    virtual void draw(const Surface *in, Surface *out) = 0;

protected:
    Filter(const std::string &name);
};

#endif // FILTER_H
