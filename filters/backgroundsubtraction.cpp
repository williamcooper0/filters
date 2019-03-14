#include "backgroundsubtraction.h"

#include "backgroundsubtraction/prepare.h"
#include "backgroundsubtraction/draw.h"
#include "backgroundsubtraction/update.h"

#include "../camera.h"
#include "../surface.h"

using namespace cv;


const char *BackgroundSubtraction::name("background subtraction");

BackgroundSubtraction::BackgroundSubtraction()
    : Filter(name)
{

}


namespace _Gpu
{

BackgroundSubtraction::BackgroundSubtraction(int cHistory, float cInitialLearningRate, float cVarThreshold, bool cShadowDetection)
    : mixtures(5)
    , channels(3)
    , varianceInitial(15.0 / 255.0)
    , history(cHistory)
    , initialLearningRate(cInitialLearningRate)
    , CT(0.047)
    , varThreshold(cVarThreshold)
    , varThresholdGen(3.0 * 3.0 / 255.0)
    , backgroundRatio(0.9)
    , varianceMin(4.0 / 255.0)
    , varianceMax(5.0 * varianceInitial)
    , shadowDetection(cShadowDetection)
    , shadow(0.5)
    , tau(0.5)

    , _gmmAndMean(new Surface(Camera::width() * 2, Camera::height() * mixtures))
    , _data(new Surface(Camera::width(), Camera::height() * mixtures))

    , _prepare(new Prepare(this))
    , _update(new Update(this))
    , _draw(new Draw(this))
{
    unsigned char *data = new unsigned char[_gmmAndMean->height() * _gmmAndMean->width() * 4];

    initWeight(data);
    initVarianceAndMean(data);

    _gmmAndMean->write(GL_RGBA, data);
    delete[] data;
}

BackgroundSubtraction::~BackgroundSubtraction()
{
    delete _draw;
    delete _update;
    delete _prepare;

    delete _data;
    delete _gmmAndMean;
}

void BackgroundSubtraction::draw(const ::Surface *in, ::Surface *out)
{
    const Surface *gIn = static_cast<const Surface*>(in);
    Surface *gOut = static_cast<Surface*>(out);

    _prepare->execute(gIn, _data);
    _update->execute(gIn, _gmmAndMean);
    _draw->execute(gIn, gOut);
}

const Surface *BackgroundSubtraction::gmmAndMean() const
{
    return _gmmAndMean;
}

const Surface *BackgroundSubtraction::data() const
{
    return _data;
}

float BackgroundSubtraction::prune() const
{
    return _prune;
}

float BackgroundSubtraction::alphaT() const
{
    return _alphaT;
}

float BackgroundSubtraction::alpha1() const
{
    return _alpha1;
}

void BackgroundSubtraction::setPrune(float prune)
{
    _prune = prune;
}

void BackgroundSubtraction::setAlphaT(float alphaT)
{
    _alphaT = alphaT;
}

void BackgroundSubtraction::setAlpha1(float alpha1)
{
    _alpha1 = alpha1;
}


#define ROW(MODE, Y) (&(data[(((MODE) * Camera::height() + (Y)) * _gmmAndMean->width()) * 4]))
#define GMM_PIXEL(MODE, X, Y) (&(ROW((MODE), (Y))[(X) * 4]))


#define WEIGHT(MODE, X, Y)   GMM_PIXEL((MODE), (X), (Y))[0]
#define VARIANCE(MODE, X, Y) GMM_PIXEL((MODE), (X), (Y))[1]

#define MEAN(MODE, X, Y) (&(ROW((MODE), (Y))[(Camera::width() + (X)) * 4]))


void BackgroundSubtraction::initWeight(unsigned char *data) const
{
    const int width = Camera::width();
    const int height = Camera::height();

    int m = 0;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++)
            WEIGHT(m, x, y) = 1 * 255.0;
    }

    for(m = 1; m < mixtures; m++) {
        const float alphaT = 1.0 / std::min( 2 * (m + 1), history);
        const float alpha1 = 1 - alphaT;

        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                for(int mm = 0; mm < m; mm++)
                    WEIGHT(mm, x, y) *= alpha1;

                WEIGHT(m, x, y) = alphaT * 255.0;
            }
        }
    }
}

void BackgroundSubtraction::initVarianceAndMean(unsigned char *data) const
{
    for(int m = 0; m < mixtures; m++) {
        for(int y = 0; y < Camera::height(); y++) {
            for(int x = 0; x < Camera::width(); x++) {
                VARIANCE(m, x, y) = varianceInitial * 255.0;

                for(int c = 0; c < channels; c++) {
                    const cv::Mat *image = Camera::image();
                    MEAN(m, x, y)[c] = image->data[y * image->step[0] + x * channels + channels - 1 - c];
                }
            }
        }
    }
}

}


namespace _Cpu
{

BackgroundSubtraction::BackgroundSubtraction()
    : _subtractor(createBackgroundSubtractorMOG2())
{
    Mat out;

    for(int i = 0; i < 5; i++)
        _subtractor->apply(*Camera::image(), out);
}

void BackgroundSubtraction::draw(const ::Surface *in, ::Surface *out)
{
    const Mat &cIn = *static_cast<const Surface*>(in)->mat();
    Mat &cOut = *static_cast<Surface*>(out)->mat();

    _subtractor->apply(cIn, cOut);
}

}
