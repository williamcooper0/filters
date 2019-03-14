#ifndef BACKGROUND_SUBTRACTION_H
#define BACKGROUND_SUBTRACTION_H

#include "../filter.h"
#include <opencv2/opencv.hpp>

class BackgroundSubtraction : public Filter
{
public:
    static const char *name;

protected:
    BackgroundSubtraction();
};

namespace _Gpu
{


class Surface;

class Prepare;
class Draw;
class Update;


class BackgroundSubtraction : public ::BackgroundSubtraction
{
public:
    const int mixtures;
    const int channels;
    const float varianceInitial;
    const int history;
    const float initialLearningRate;
    const float CT;
    const float varThreshold;
    const float varThresholdGen;
    const float backgroundRatio;
    const float varianceMin;
    const float varianceMax;
    const bool shadowDetection;
    const float shadow;
    const float tau;


    BackgroundSubtraction(int cHistory = 250, float cInitialLearningRate = -1, float cVarThreshold = 16.0 / 255.0, bool cShadowDetection = true);
    ~BackgroundSubtraction();

    void draw(const ::Surface *in, ::Surface *out);

    const Surface *gmmAndMean() const;
    const Surface *data() const;

    float prune() const;
    float alphaT() const;
    float alpha1() const;

    void setPrune(float prune);
    void setAlphaT(float alphaT);
    void setAlpha1(float alpha1);

private:
    Surface *_gmmAndMean;
    Surface *_data;

    Prepare *_prepare;
    Update *_update;
    Draw *_draw;

    float _prune;
    float _alphaT;
    float _alpha1;


    void initWeight(unsigned char *data) const;
    void initVarianceAndMean(unsigned char *data) const;
};

}

namespace _Cpu
{

class BackgroundSubtraction : public ::BackgroundSubtraction
{
public:
    BackgroundSubtraction();
    void draw(const ::Surface *in, ::Surface *out);

private:
    cv::Ptr<cv::BackgroundSubtractor> _subtractor;
};

}

#endif // BACKGROUND_SUBTRACTION_H
