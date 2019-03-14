#include "filter.h"

#include <iostream>

using namespace std;

Filter::~Filter()
{

}

Filter::Filter(const string &name)
{
    cout << name << '\n';
}


