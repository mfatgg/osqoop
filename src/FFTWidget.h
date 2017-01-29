#ifndef _FFT_WIDGET_H
#define _FFT_WIDGET_H 1

#include <qwt_plot.h>
#include "IntegerRealValuedFFT.h"

const int PLOT_SIZE = 256;      // 0 to 511


class FFTWidget : public QwtPlot
{
public:
    FFTWidget();
    void processData(const std::valarray<signed short> &dataLinear, unsigned samplingRate, unsigned unitPerVoltageCount);
private:
    void alignScales();
    double d_x[PLOT_SIZE];
    double d_y[PLOT_SIZE];
    IntegerRealValuedFFT<512> fft;
};

#endif
