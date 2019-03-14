#ifndef PIPELINE_H
#define PIPELINE_H

#include <vector>

class Filter;
class Surface;

class Pipeline
{
public:
    static void init(int argc, char **argv);
    static void clear();
    
    static void draw(const Surface *in, Surface *out);
    static const std::vector<Filter*> &filters();

private:
    static Surface *_tmp;
    static std::vector<Filter*> _filters;
};

#endif // PIPELINE_H
