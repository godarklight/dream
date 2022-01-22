/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Original Author(s):
 *	Volker Fischer
 *
 * Qwt 5-6 conversion Author(s):
 *  David Flamand
 *
 * Description:
 *	Custom settings of the qwt-plot, Support Qwt version 5.0.0 to 6.1.0(+)
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#ifndef __DRMPLOT_QWT5_H
#define __DRMPLOT_QWT5_H

/* Qt includes */
#include <QTimer>
#include <QObject>
#include <QPainter>
#include <QDialog>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QHideEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QIcon>

/* Qwt includes */
#include <qwt_global.h>
#if QWT_VERSION < 0x050000
# error QWT_VERSION < 0x050000
#endif
#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>
#include <qwt_scale_draw.h>
#include <qwt_symbol.h>
#if QWT_VERSION >= 0x060000
# include <qwt_picker_machine.h>
#endif

/* Other includes */
#include "../resample/cspectrumresample.h"
#include "../util/Vector.h"
#include "../Parameter.h"
#include "../main-Qt/crx.h"


/* Definitions ****************************************************************/
#define GUI_CONTROL_UPDATE_WATERFALL			100	/* Milliseconds */


/* Define the plot color profiles */
/* BLUEWHITE */
#define BLUEWHITE_MAIN_PEN_COLOR_PLOT			Qt::blue
#define BLUEWHITE_MAIN_PEN_COLOR_CONSTELLATION	Qt::blue
#define BLUEWHITE_BCKGRD_COLOR_PLOT				Qt::white
#define BLUEWHITE_MAIN_GRID_COLOR_PLOT			Qt::gray
#define BLUEWHITE_SPEC_LINE1_COLOR_PLOT			Qt::red
#define BLUEWHITE_SPEC_LINE2_COLOR_PLOT			Qt::black
#define BLUEWHITE_PASS_BAND_COLOR_PLOT			QColor(192, 192, 255)

/* GREENBLACK */
#define GREENBLACK_MAIN_PEN_COLOR_PLOT			Qt::green
#define GREENBLACK_MAIN_PEN_COLOR_CONSTELLATION	Qt::red
#define GREENBLACK_BCKGRD_COLOR_PLOT			Qt::black
#define GREENBLACK_MAIN_GRID_COLOR_PLOT			QColor(128, 0, 0)
#define GREENBLACK_SPEC_LINE1_COLOR_PLOT		Qt::yellow
#define GREENBLACK_SPEC_LINE2_COLOR_PLOT		Qt::blue
#define GREENBLACK_PASS_BAND_COLOR_PLOT			QColor(0, 96, 0)

/* BLACKGREY */
#define BLACKGREY_MAIN_PEN_COLOR_PLOT			Qt::black
#define BLACKGREY_MAIN_PEN_COLOR_CONSTELLATION	Qt::green
#define BLACKGREY_BCKGRD_COLOR_PLOT				Qt::gray
#define BLACKGREY_MAIN_GRID_COLOR_PLOT			Qt::white
#define BLACKGREY_SPEC_LINE1_COLOR_PLOT			Qt::blue
#define BLACKGREY_SPEC_LINE2_COLOR_PLOT			Qt::yellow
#define BLACKGREY_PASS_BAND_COLOR_PLOT			QColor(128, 128, 128)


/* Maximum and minimum values of x-axis of input spectrum plots */
#define MIN_VAL_INP_SPEC_Y_AXIS_DB				((double) -120.0)
#define MAX_VAL_INP_SPEC_Y_AXIS_DB				((double) 0.0)

/* Maximum and minimum values of x-axis of input PSD (shifted) */
#define MIN_VAL_SHIF_PSD_Y_AXIS_DB				((double) -85.0)
#define MAX_VAL_SHIF_PSD_Y_AXIS_DB				((double) -35.0)

/* Maximum and minimum values of x-axis of SNR spectrum */
#define MIN_VAL_SNR_SPEC_Y_AXIS_DB				((double) 0.0)
#define MAX_VAL_SNR_SPEC_Y_AXIS_DB				((double) 35.0)


/* Define the FAC/SDC/MSC Symbol size */
#if QWT_VERSION < 0x060000
# define FAC_SYMBOL_SIZE 5
# define SDC_SYMBOL_SIZE 5
# define MSC_SYMBOL_SIZE 2
# define ALL_FAC_SYMBOL_SIZE 5
# define ALL_SDC_SYMBOL_SIZE 4
# define ALL_MSC_SYMBOL_SIZE 2
#else
# define FAC_SYMBOL_SIZE 4
# define SDC_SYMBOL_SIZE 4
# define MSC_SYMBOL_SIZE 2
# define ALL_FAC_SYMBOL_SIZE 4
# define ALL_SDC_SYMBOL_SIZE 4
# define ALL_MSC_SYMBOL_SIZE 2
#endif


/* Define the style of marker */
#define MARKER_STYLE Qt::DashLine


/* Define the plot font size */
#define PLOT_FONT_SIZE 8


/* Sometime between Qwt version some name may change, we fix this */
#if QWT_VERSION < 0x050200
# define LOWERBOUND lBound
# define UPPERBOUND hBound
#else
# define LOWERBOUND lowerBound
# define UPPERBOUND upperBound
#endif
#if QWT_VERSION < 0x060000
# define SETDATA setData
#else
# define SETDATA setSamples
#endif
#if QWT_VERSION < 0x060100
# define SETMAJORPEN setMajPen
# define SETMINORPEN setMinPen
#else
# define SETMAJORPEN setMajorPen
# define SETMINORPEN setMinorPen
#endif

/* Set workaround flag for Qwt version 5.0.0 and 5.0.1 */
/* QwtPlotCurve::Xfy seems broken on these versions */
#if QWT_VERSION < 0x050002
# define QWT_WORKAROUND_XFY
#endif


/* Window margin for chart */
#define WINDOW_MARGIN 10

/* Window size for standalone chart */
#define WINDOW_CHART_WIDTH 256
#define WINDOW_CHART_HEIGHT 256

/* Window border for standalone chart */
#define WINDOW_BORDER 1


/* Classes ********************************************************************/


/* This class is needed to handle events for standalone window chart */
class QwtPlotDialog : public QDialog
{
	Q_OBJECT

public:
	QwtPlotDialog(QWidget* parent) : QDialog(parent)
	{
		setWindowFlags(Qt::Window);
		resize(WINDOW_CHART_WIDTH, WINDOW_CHART_HEIGHT);
		Frame = new QFrame(this);
		Frame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
		Frame->setLineWidth(WINDOW_BORDER);
		Plot = new QwtPlot(Frame);
		/*fprintf(stderr, "QwtPlotDialog()\n");*/
	}
	~QwtPlotDialog() { /*fprintf(stderr, "~QwtPlotDialog()\n");*/ }
	QwtPlot *GetPlot() { return Plot; }
	void show() { QDialog::show(); emit activate(); }
	void hide() { emit deactivate(); QDialog::hide(); }

protected:
	QFrame *Frame;
	QwtPlot *Plot;
	void reject() { emit deactivate(); QDialog::reject(); }
	void resizeEvent(QResizeEvent *e)
	{
		QRect rf(0, 0, e->size().width(), e->size().height());
		Frame->setGeometry(rf);
		QRect rp(rf.x()+WINDOW_BORDER, rf.y()+WINDOW_BORDER,
			rf.width()-WINDOW_BORDER*2,
			rf.height()-WINDOW_BORDER*2);
		Plot->setGeometry(rp);
	}

signals:
	void activate();
	void deactivate();
};


class CWidget : public QWidget
{
	Q_OBJECT
public:
	CWidget(QWidget* parent, const QPixmap& Pixmap)
		: QWidget(parent), Pixmap(Pixmap)
	{ setAttribute(Qt::WA_OpaquePaintEvent, true);
	  setAttribute(Qt::WA_TransparentForMouseEvents, true);
	  setCursor(Qt::CrossCursor); show(); }
	virtual ~CWidget() {}
protected:
	QPainter Painter;
	const QPixmap& Pixmap;
	void paintEvent(QPaintEvent *)
	{
		if (Painter.begin(this))
		{
			Painter.drawPixmap(0, 0, Pixmap);
			Painter.end();
		}
	}
};


class CDRMPlot : public QObject
{
	Q_OBJECT

public:
	enum ECharType
	{
		INPUT_SIG_PSD = 0, /* default */
		TRANSFERFUNCTION = 1,
		FAC_CONSTELLATION = 2,
		SDC_CONSTELLATION = 3,
		MSC_CONSTELLATION = 4,
		POWER_SPEC_DENSITY = 5,
		INPUTSPECTRUM_NO_AV = 6,
		AUDIO_SPECTRUM = 7,
		FREQ_SAM_OFFS_HIST = 8,
		DOPPLER_DELAY_HIST = 9,
		ALL_CONSTELLATION = 10,
		SNR_AUDIO_HIST = 11,
		AVERAGED_IR = 12,
		SNR_SPECTRUM = 13,
		INPUT_SIG_PSD_ANALOG = 14,
		INP_SPEC_WATERF = 15,
		NONE_OLD = 16 /* None must always be the last element! (see settings) */
	};

	CDRMPlot(QWidget*, QwtPlot*);
	~CDRMPlot();

	QwtPlot         *plot;

	/* This function has to be called before chart can be used! */
    void SetRecObj(CRx* pNDRMRec) {pDRMRec = pNDRMRec;}

	void SetupChart(const ECharType eNewType);
	ECharType GetChartType() const { return CurCharType; }
	void UpdateAnalogBWMarker();
	void SetPlotStyle(const int iNewStyleID);

	void setCaption(const QString& s) { if (DialogPlot) DialogPlot->setWindowTitle(s); }
	void setIcon(const QIcon& s) { if (DialogPlot) DialogPlot->setWindowIcon(s); }
	void setGeometry(const QRect& g) { if (DialogPlot) DialogPlot->setGeometry(g); }
	bool isVisible() { if (DialogPlot) return DialogPlot->isVisible(); else return false; }
	const QRect geometry() { if (DialogPlot) return DialogPlot->geometry(); else return QRect(); }
	void close() { if (DialogPlot) delete this; }
	void hide() { if (DialogPlot) DialogPlot->hide(); }
	void show() { if (DialogPlot) DialogPlot->show(); }


protected:
	void SetVerticalBounds(
		const _REAL rStartGuard, const _REAL rEndGuard,
		const _REAL rBeginIR, const _REAL rEndIR);
	void SetHorizontalBounds( _REAL rScaleMin, _REAL rScaleMax, _REAL rLowerB, _REAL rHigherB);
	void SetInpSpecWaterf(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
	void SetDCCarrier(const _REAL rDCFreq);
	void SetBWMarker(const _REAL rBWCenter, const _REAL rBWWidth);
	void AutoScale(CVector<_REAL>& vecrData, CVector<_REAL>& vecrData2,
		CVector<_REAL>& vecrScale);
	void AutoScale2(CVector<_REAL>& vecrData,
		CVector<_REAL>& vecrData2,
		CVector<_REAL>& vecrScale);
	void AutoScale3(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
	void SetData(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
	void SetData(CVector<_REAL>& vecrData1, CVector<_REAL>& vecrData2,
		CVector<_REAL>& vecrScale);
	void SetData(QwtPlotCurve& curve, CVector<_COMPLEX>& veccData);
	void SetData(CVector<_COMPLEX>& veccData);
	void SetData(CVector<_COMPLEX>& veccMSCConst,
		CVector<_COMPLEX>& veccSDCConst,
		CVector<_COMPLEX>& veccFACConst);
	void SetGrid(double div, int step, int substep);
	void SetQAMGrid(const ECodScheme eCoSc);

	void PlotDefaults();
	void PlotForceUpdate();
	void PlotSetLegendFont();

	void SetupAvIR();
	void SetupTranFct();
	void SetupAudioSpec(bool bAudioDecoder);
	void SetupFreqSamOffsHist();
	void SetupDopplerDelayHist();
	void SetupSNRAudHist();
	void SetupPSD();
	void SetupSNRSpectrum();
	void SetupInpSpec();
	void SetupFACConst();
	void SetupSDCConst(const ECodScheme eNewCoSc);
	void SetupMSCConst(const ECodScheme eNewCoSc);
	void SetupAllConst();
	void SetupInpPSD(bool bAnalog = false);
	void SetupInpSpecWaterf();

	void AddWhatsThisHelpChar(const ECharType NCharType);

	QwtPlot         *SuppliedPlot;
	QwtPlotDialog   *DialogPlot;

	bool            bActive;

    /* Colors */
	QColor			MainPenColorPlot;
	QColor			MainPenColorConst;
	QColor			MainGridColorPlot;
	QColor			SpecLine1ColorPlot;
	QColor			SpecLine2ColorPlot;
	QColor			PassBandColorPlot;
	QColor			BckgrdColorPlot;

	ECharType		CurCharType;
	ECharType		InitCharType;
	ECodScheme		eLastSDCCodingScheme;
	ECodScheme		eLastMSCCodingScheme;
	bool		bLastAudioDecoder;

	QwtText			leftTitle, rightTitle, bottomTitle;

	QwtPlotCurve	main1curve, main2curve;
	QwtPlotCurve	curve1, curve2, curve3, curve4, curve5;
	QwtPlotCurve	hcurvegrid, vcurvegrid;
	QwtPlotGrid		grid;
#if QWT_VERSION < 0x060000
	QwtSymbol		MarkerSym1, MarkerSym2, MarkerSym3;
#endif
	QwtPlotPicker	*picker;
	QwtLegend		*legend;

	QTimer			TimerChart;
    CRx             *pDRMRec;

	/* Waterfall spectrum stuff */
	QPixmap			Canvas;
	QImage			Image;
	QColor			BackgroundColor;
	CSpectrumResample	Resample;
	CWidget*		WaterfallWidget;
	QRect			LastPlotCanvRect;
	int				scaleWidth;
	QRgb*			imageData;
	int				iAudSampleRate;
	int				iSigSampleRate;
	int				iLastXoredSampleRate;
	int				iLastChanMode;

public slots:
#if QWT_VERSION < 0x060000
	void OnSelected(const QwtDoublePoint &pos);
#else
	void OnSelected(const QPointF &pos);
#endif
	void OnTimerChart();
	void activate();
	void deactivate();

signals:
	void xAxisValSet(double);
};


#endif
