#include <stdlib.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_plot_picker.h>
#include <valarray>
#include "FFTWidget.h"
#include "IntegerRealValuedFFT.h"


//
//  Initialize main window
//
FFTWidget::FFTWidget()
{
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//sizePolicy.setHorizontalStretch(20);
	//sizePolicy.setVerticalStretch(20);
    //sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
	this->setSizePolicy(sizePolicy);


    // Disable polygon clipping
    QwtPainter::setDeviceClipping(false);

    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

#if QT_VERSION >= 0x040000
#ifdef Q_WS_X11
    /*
       Qt::WA_PaintOnScreen is only supported for X11, but leads
       to substantial bugs with Qt 4.2.x/Windows
     */
    canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif
#endif

    //alignScales();

    //  Initialize data
    for (int i = 0; i< PLOT_SIZE; i++)
    {
        d_x[i] = i;     // time axis
        d_y[i] = 0; //double(rand()%256)/256;
    }

    // Assign a title
    //setTitle("512 FFT");
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

    // Insert new curves
    QwtPlotCurve *cRight = new QwtPlotCurve("FFT Spectrum");
    cRight->attach(this);

    // Set curve styles
    cRight->setPen(QPen(Qt::red));

    // Attach (don't copy) data. Both curves use the same x array.
    cRight->setRawData(d_x, d_y, PLOT_SIZE);

    // Axis
    //setAxisTitle(QwtPlot::xBottom, "Frequency");
    //setAxisScale(QwtPlot::xBottom, 0, 1000);
    setAxisAutoScale(QwtPlot::xBottom);

    //setAxisTitle(QwtPlot::yLeft, "Values");
    //setAxisScale(QwtPlot::yLeft, 0, 256000);
    setAxisAutoScale(QwtPlot::yLeft);

	resize(600,100);
    //replot();

    QwtPlotPicker *picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPicker::PointSelection,
        QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());
    picker->setRubberBandPen(QColor(Qt::black));
    picker->setTrackerPen(QColor(Qt::black));
}

void FFTWidget::processData(const std::valarray<signed short> &dataLinear, unsigned samplingRate, unsigned unitPerVoltageCount)
{
    short fftbuf[2*512];

    for (int i=0; i<512; i++)
    {
        fftbuf[2*i] = (0.53836 - 0.46164 * cos(2.0*M_PI*(double)i/511.0)) * ((double)dataLinear[i]*2000.0/(double)unitPerVoltageCount);
        //fftbuf[2*i] = dataLinear[i];
        fftbuf[2*i+1] = 0;
    }
    fft.fft(fftbuf);

    for (int i = 0; i< PLOT_SIZE; i++)
    {
        //d_x[i] = ((double)i-(PLOT_SIZE/2)) * ((sampleRate/2)/(PLOT_SIZE/2));     // time axis
        d_x[i] = (double)i*((double)samplingRate/2.0/(double)PLOT_SIZE);
        d_y[i] = fft.module2(fftbuf, i);
        //d_y[i] = (0.53836 - 0.46164 * cos(2.0*3.141592*(double)i/255.0));
    }

    //setAxisScale(QwtPlot::yLeft, 0, 70);
    //printf("%u\n", sampleRate);
    replot();
}

//
//  Set a plain canvas frame and align the scales to it
//
void FFTWidget::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}
