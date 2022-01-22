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

#include "DRMPlot.h"
#include "../PlotManager.h"
#include <cmath>
#include <algorithm>
#include "../matlib/MatlibSigProToolbox.h"

/* Implementation *************************************************************/
CDRMPlot::CDRMPlot(QWidget* parent, QwtPlot* SuppliedPlot) :
	SuppliedPlot(SuppliedPlot), DialogPlot(nullptr), bActive(false),
	CurCharType(NONE_OLD), InitCharType(NONE_OLD),
	eLastSDCCodingScheme((ECodScheme)-1), eLastMSCCodingScheme((ECodScheme)-1),
	bLastAudioDecoder(false), pDRMRec(nullptr),
	WaterfallWidget(nullptr), iAudSampleRate(0), iSigSampleRate(0),
	iLastXoredSampleRate(0), iLastChanMode(-1)
{
	/* Create new plot if none is supplied */
	if (SuppliedPlot == nullptr)
	{
		DialogPlot = new QwtPlotDialog(parent);
		plot = DialogPlot->GetPlot();
	}
	else
		plot = SuppliedPlot;

	/* Setup plot */
	plot->setAutoDelete(false);
	plot->setAutoReplot(false);

	/* Base font */
	QFont basefont;
	basefont.setPointSize(PLOT_FONT_SIZE);
	basefont.setStyleHint(QFont::SansSerif, QFont::PreferOutline);

	/* Legend creation */
	legend = new QwtLegend();
	QFont legendfont(basefont);
	legend->setFont(legendfont);
	plot->insertLegend(legend, QwtPlot::RightLegend);

	/* Curve defaults (other curves are set by PlotDefaults) */
	curve4.setItemAttribute(QwtPlotItem::Legend, false);
	curve5.setItemAttribute(QwtPlotItem::Legend, false);
	vcurvegrid.setItemAttribute(QwtPlotItem::Legend, false);
	hcurvegrid.setItemAttribute(QwtPlotItem::Legend, false);
#ifndef QWT_WORKAROUND_XFY
	vcurvegrid.setStyle(QwtPlotCurve::Sticks);
	hcurvegrid.setStyle(QwtPlotCurve::Sticks);
#else
	vcurvegrid.setStyle(QwtPlotCurve::Steps);
	hcurvegrid.setStyle(QwtPlotCurve::Steps);
#endif
#if QWT_VERSION < 0x060000
# ifndef QWT_WORKAROUND_XFY
	vcurvegrid.setCurveType(QwtPlotCurve::Yfx);
	hcurvegrid.setCurveType(QwtPlotCurve::Xfy);
# endif
#else
	vcurvegrid.setOrientation(Qt::Vertical);
	hcurvegrid.setOrientation(Qt::Horizontal);
#endif

	/* Grid */
	grid.enableXMin(false);
	grid.enableYMin(false);
	grid.attach(plot);

	/* Axis and title fonts */
	QFont axisfont(basefont);
	QFont titlefont(basefont);
	titlefont.setWeight(QFont::Bold);
	plot->setAxisFont(QwtPlot::xBottom, axisfont);
	plot->setAxisFont(QwtPlot::yLeft, axisfont);
	plot->setAxisFont(QwtPlot::yRight, axisfont);
	QwtText title;
	title.setFont(titlefont);
	plot->setTitle(title);

	/* Axis titles */
	bottomTitle.setFont(axisfont);
	plot->setAxisTitle(QwtPlot::xBottom, bottomTitle);
	leftTitle.setFont(axisfont);
	plot->setAxisTitle(QwtPlot::yLeft, leftTitle);
	rightTitle.setFont(axisfont);
	plot->setAxisTitle(QwtPlot::yRight, rightTitle);

	/* Global frame */
	plot->setFrameStyle(QFrame::Plain|QFrame::NoFrame);
	plot->setLineWidth(0);
	plot->setContentsMargins(
		WINDOW_MARGIN, WINDOW_MARGIN,
		WINDOW_MARGIN, WINDOW_MARGIN);

	/* Canvas */
#if QWT_VERSION < 0x060100
	plot->setCanvasLineWidth(0);
#else
	((QFrame*)plot->canvas())->setLineWidth(0);
#endif
	plot->canvas()->setBackgroundRole(QPalette::Window);

	/* Picker */
	picker = new QwtPlotPicker(plot->canvas());
#if QWT_VERSION < 0x060000
	picker->setSelectionFlags(QwtPicker::PointSelection | QwtPicker::ClickSelection);
#else
	picker->initMousePattern(1);
	QwtPickerClickPointMachine *machine = new QwtPickerClickPointMachine();
	picker->setStateMachine(machine);
#endif

	/* Set default style */
	SetPlotStyle(0);

	/* Connections */
	connect(&TimerChart, SIGNAL(timeout()), this, SLOT(OnTimerChart()));
#if QWT_VERSION < 0x060000
	connect(picker, SIGNAL(selected(const QwtDoublePoint &)),
		this, SLOT(OnSelected(const QwtDoublePoint &)));
#else
	connect(picker, SIGNAL(selected(const QPointF &)),
		this, SLOT(OnSelected(const QPointF &)));
#endif
	if (DialogPlot != nullptr)
	{
		connect(DialogPlot, SIGNAL(activate()), this, SLOT(activate()));
		connect(DialogPlot, SIGNAL(deactivate()), this, SLOT(deactivate()));
	}
}

CDRMPlot::~CDRMPlot()
{
	deactivate();
	/* Delete DialogPlot if exist */
	if (DialogPlot != nullptr)
		delete DialogPlot;
}

void CDRMPlot::UpdateAnalogBWMarker()
{
	if (CurCharType == INPUT_SIG_PSD_ANALOG)
	{
		_REAL rCenterFreq, rBandwidth;
		/* Get data and parameters from modules */
        pDRMRec->GetAMBWParameters(rCenterFreq, rBandwidth);
		/* Prepare graph and set data */
        SetDCCarrier(pDRMRec->GetAMMixerFrequencyOffset());
		SetBWMarker(rCenterFreq, rBandwidth);
		plot->replot();
	}
}

void CDRMPlot::OnTimerChart() // TODO make sure timer stopped when receiver not running
{
	CParameter& Parameters = *pDRMRec->GetParameters();

	/* CHART ******************************************************************/
	CVector<_REAL>	vecrData;
	CVector<_REAL>	vecrData2;
	CVector<_COMPLEX>	veccData1, veccData2, veccData3;
	CVector<_REAL>	vecrScale;
	_REAL		rLowerBound, rHigherBound;
	_REAL		rStartGuard, rEndGuard;
	_REAL		rPDSBegin, rPDSEnd;
	_REAL		rFreqAcquVal;
	_REAL		rCenterFreq, rBandwidth;

	Parameters.Lock();
	_REAL rDCFrequency = Parameters.GetDCFrequency();
	ECodScheme eSDCCodingScheme = Parameters.eSDCCodingScheme;
	ECodScheme eMSCCodingScheme = Parameters.eMSCCodingScheme;
	bool bAudioDecoder = !Parameters.audiodecoder.empty();
	iAudSampleRate = Parameters.GetAudSampleRate();
	iSigSampleRate = Parameters.GetSigSampleRate();
    int iChanMode = pDRMRec->GetInChanSel();
	Parameters.Unlock();

	/* Needed to detect sample rate change */
	const int iXoredSampleRate = iAudSampleRate ^ iSigSampleRate;

	CPlotManager& PlotManager = *pDRMRec->GetPlotManager();

	/* First check if plot must be set up */
	bool change = false;
	if (InitCharType != CurCharType ||
		iLastXoredSampleRate != iXoredSampleRate ||
		iLastChanMode != iChanMode)
	{
		InitCharType = CurCharType;
		iLastXoredSampleRate = iXoredSampleRate;
		iLastChanMode = iChanMode;
		change = true;
		PlotDefaults();
	}
	switch (CurCharType)
	{
	case AVERAGED_IR:
		/* Get data from module */
		PlotManager.GetAvPoDeSp(vecrData, vecrScale, rLowerBound, rHigherBound,
			rStartGuard, rEndGuard, rPDSBegin, rPDSEnd);

		if (vecrScale.Size() != 0)
		{
			if (change) SetupAvIR();
			SetVerticalBounds(rStartGuard, rEndGuard, rPDSBegin, rPDSEnd);
			SetHorizontalBounds(vecrScale[0], vecrScale[vecrScale.Size() - 1], rLowerBound, rHigherBound);
			SetData(vecrData, vecrScale);
        }
		else
		{
			/* No input data, clear plot (by resetting it) */
			SetupAvIR();
		}
		break;

	case TRANSFERFUNCTION:
		/* Get data from module */
		PlotManager.GetTransferFunction(vecrData, vecrData2, vecrScale);

		if (change) SetupTranFct();
		/* Prepare graph and set data */
		plot->setAxisScale(QwtPlot::xBottom, (double) 0.0, (double) vecrScale.Size());
		SetData(vecrData, vecrData2, vecrScale);
		break;

    case POWER_SPEC_DENSITY: // "Shifted PSD" - works for file input, displaced on sound card input
		/* Get data from module */
        pDRMRec->GetPowDenSpec(vecrData, vecrScale);

		if (change) SetupPSD();
		/* Set data */
		SetData(vecrData, vecrScale);
		break;

    case SNR_SPECTRUM: // "SNR Spectrum"
		/* Get data from module */
		PlotManager.GetSNRProfile(vecrData, vecrScale);

		if (change) SetupSNRSpectrum();
		/* Prepare graph and set data */
		AutoScale3(vecrData, vecrScale);
		SetData(vecrData, vecrScale);
		break;

    case INPUTSPECTRUM_NO_AV: // "Input Spectrum" - broken
		/* Get data from module */
        pDRMRec->GetInputSpec(vecrData, vecrScale);

		if (change) SetupInpSpec();
		/* Prepare graph and set data */
		SetDCCarrier(rDCFrequency);
		SetData(vecrData, vecrScale);
		break;

    case INP_SPEC_WATERF: // "Waterfall"
		/* Get data from module */
        pDRMRec->GetInputSpec(vecrData, vecrScale);

		if (change) SetupInpSpecWaterf();
		/* Set data */
		SetInpSpecWaterf(vecrData, vecrScale);
		break;

    case INPUT_SIG_PSD: // "Input PSD" - broken
		/* Get data from module */
		PlotManager.GetInputPSD(vecrData, vecrScale);

		if (change) SetupInpPSD();
		/* Prepare graph and set data */
		SetDCCarrier(rDCFrequency);
		SetData(vecrData, vecrScale);
		break;

	case INPUT_SIG_PSD_ANALOG:
		/* Get data and parameters from modules */
        pDRMRec->GetInputPSD(vecrData, vecrScale);
        pDRMRec->GetAMBWParameters(rCenterFreq, rBandwidth);

		if (change) SetupInpPSD(true);
		/* Prepare graph and set data */
        SetDCCarrier(pDRMRec->GetAMMixerFrequencyOffset());
		SetBWMarker(rCenterFreq, rBandwidth);
		SetData(vecrData, vecrScale);
		break;

	case AUDIO_SPECTRUM:
		/* Get data from module */
        pDRMRec->GetAudioSpec(vecrData, vecrScale);

		if (change || bLastAudioDecoder != bAudioDecoder)
		{
			bLastAudioDecoder = bAudioDecoder;
			SetupAudioSpec(bAudioDecoder);
		}
		/* Set data */
		if (bAudioDecoder)
			SetData(vecrData, vecrScale);
		break;

	case FREQ_SAM_OFFS_HIST:
		/* Get data from module */
		PlotManager.GetFreqSamOffsHist(vecrData, vecrData2, vecrScale, rFreqAcquVal);

		if (change) SetupFreqSamOffsHist();
		/* Prepare graph and set data */
		plot->setAxisTitle(QwtPlot::yLeft, tr("Freq. Offset [Hz] rel. to ")
            + QString().setNum(pDRMRec->ConvertFrequency(rFreqAcquVal))
			+ " Hz");
		AutoScale(vecrData, vecrData2, vecrScale);
		SetData(vecrData, vecrData2, vecrScale);
		break;

	case DOPPLER_DELAY_HIST:
		/* Get data from module */
		PlotManager.GetDopplerDelHist(vecrData, vecrData2, vecrScale);

		if (change) SetupDopplerDelayHist();
		/* Prepare graph and set data */
		plot->setAxisScale(QwtPlot::xBottom, (double) vecrScale[0], (double) 0.0);
		SetData(vecrData, vecrData2, vecrScale);
		break;

	case SNR_AUDIO_HIST:
		/* Get data from module */
		PlotManager.GetSNRHist(vecrData, vecrData2, vecrScale);

		if (change) SetupSNRAudHist();
		/* Prepare graph and set data */
		AutoScale2(vecrData, vecrData2, vecrScale);
		SetData(vecrData, vecrData2, vecrScale);
		break;

	case FAC_CONSTELLATION:
		/* Get data vector */
        pDRMRec->GetFACMLCVectorSpace(veccData1);

		if (change) SetupFACConst();
		/* Set data */
		SetData(veccData1);
		break;

	case SDC_CONSTELLATION:
		/* Get data vector */
        pDRMRec->GetSDCMLCVectorSpace(veccData1);

		if (change || eLastSDCCodingScheme != eSDCCodingScheme)
		{
			eLastSDCCodingScheme = eSDCCodingScheme;
			SetupSDCConst(eSDCCodingScheme);
		}
		/* Set data */
		SetData(veccData1);
		break;

	case MSC_CONSTELLATION:
		/* Get data vector */
        pDRMRec->GetMSCMLCVectorSpace(veccData1);

		if (change || eLastMSCCodingScheme != eMSCCodingScheme)
		{
			eLastMSCCodingScheme = eMSCCodingScheme;
			SetupMSCConst(eMSCCodingScheme);
		}
		/* Set data */
		SetData(veccData1);
		break;

	case ALL_CONSTELLATION:
		/* Get data vectors */
        pDRMRec->GetMSCMLCVectorSpace(veccData1);
        pDRMRec->GetSDCMLCVectorSpace(veccData2);
        pDRMRec->GetFACMLCVectorSpace(veccData3);

		if (change) SetupAllConst();
		/* Set data */
		SetData(veccData1, veccData2, veccData3);
		break;

	case NONE_OLD:
		break;
	}

	plot->replot();
}

void CDRMPlot::SetupChart(const ECharType eNewType)
{
	if (eNewType != NONE_OLD)
	{
		/* Set internal variable */
		CurCharType = eNewType;

		/* Update help text connected with the plot widget */
		AddWhatsThisHelpChar(eNewType);

		/* Update chart */
		if (bActive)
			OnTimerChart();

		/* Set up timer */
		switch (eNewType)
		{
		case INP_SPEC_WATERF:
			/* Very fast update */
			TimerChart.setInterval(GUI_CONTROL_UPDATE_WATERFALL);
			break;

		case AVERAGED_IR:
		case TRANSFERFUNCTION:
		case POWER_SPEC_DENSITY:
		case INPUT_SIG_PSD:
		case INPUT_SIG_PSD_ANALOG:
		case SNR_SPECTRUM:
			/* Fast update */
			TimerChart.setInterval(GUI_CONTROL_UPDATE_TIME_FAST);
			break;

		case FAC_CONSTELLATION:
		case SDC_CONSTELLATION:
		case MSC_CONSTELLATION:
		case ALL_CONSTELLATION:
		case INPUTSPECTRUM_NO_AV:
		case AUDIO_SPECTRUM:
		case FREQ_SAM_OFFS_HIST:
		case DOPPLER_DELAY_HIST:
		case SNR_AUDIO_HIST:
			/* Slow update of plot */
			TimerChart.setInterval(GUI_CONTROL_UPDATE_TIME);
			break;

		case NONE_OLD:
			break;
		}
	}
}

void CDRMPlot::activate()
{
	/* Set active flag */
	bActive = true;

	/* Force re-initialization */
	InitCharType = NONE_OLD;
	SetupChart(CurCharType);

	/* Activate real-time timers when window is shown */
	TimerChart.start();
}

void CDRMPlot::deactivate()
{
	/* Deactivate real-time timers when window is hide to save CPU power */
	TimerChart.stop();

	/* Free some resources */
	PlotDefaults();

	/* Clear active flag */
	bActive = false;
}

void CDRMPlot::SetPlotStyle(const int iNewStyleID)
{
	switch (iNewStyleID)
	{
	case 1:
		MainPenColorPlot = GREENBLACK_MAIN_PEN_COLOR_PLOT;
		MainPenColorConst = GREENBLACK_MAIN_PEN_COLOR_CONSTELLATION;
		BckgrdColorPlot = GREENBLACK_BCKGRD_COLOR_PLOT;
		MainGridColorPlot = GREENBLACK_MAIN_GRID_COLOR_PLOT;
		SpecLine1ColorPlot = GREENBLACK_SPEC_LINE1_COLOR_PLOT;
		SpecLine2ColorPlot = GREENBLACK_SPEC_LINE2_COLOR_PLOT;
		PassBandColorPlot = GREENBLACK_PASS_BAND_COLOR_PLOT;
		break;

	case 2:
		MainPenColorPlot = BLACKGREY_MAIN_PEN_COLOR_PLOT;
		MainPenColorConst = BLACKGREY_MAIN_PEN_COLOR_CONSTELLATION;
		BckgrdColorPlot = BLACKGREY_BCKGRD_COLOR_PLOT;
		MainGridColorPlot = BLACKGREY_MAIN_GRID_COLOR_PLOT;
		SpecLine1ColorPlot = BLACKGREY_SPEC_LINE1_COLOR_PLOT;
		SpecLine2ColorPlot = BLACKGREY_SPEC_LINE2_COLOR_PLOT;
		PassBandColorPlot = BLACKGREY_PASS_BAND_COLOR_PLOT;
		break;

	case 0: /* 0 is default */
	default:
		MainPenColorPlot = BLUEWHITE_MAIN_PEN_COLOR_PLOT;
		MainPenColorConst = BLUEWHITE_MAIN_PEN_COLOR_CONSTELLATION;
		BckgrdColorPlot = BLUEWHITE_BCKGRD_COLOR_PLOT;
		MainGridColorPlot = BLUEWHITE_MAIN_GRID_COLOR_PLOT;
		SpecLine1ColorPlot = BLUEWHITE_SPEC_LINE1_COLOR_PLOT;
		SpecLine2ColorPlot = BLUEWHITE_SPEC_LINE2_COLOR_PLOT;
		PassBandColorPlot = BLUEWHITE_PASS_BAND_COLOR_PLOT;
		break;
	}

	/* Apply colors */
	grid.SETMAJORPEN(QPen(MainGridColorPlot, 0, Qt::DotLine));
	grid.SETMINORPEN(QPen(MainGridColorPlot, 0, Qt::DotLine));
	vcurvegrid.setPen(QPen(MainGridColorPlot, 1, Qt::DotLine));
	hcurvegrid.setPen(QPen(MainGridColorPlot, 1, Qt::DotLine));
	plot->setCanvasBackground(QColor(BckgrdColorPlot));

	/* Make sure that plot are being initialized again */
	InitCharType = NONE_OLD;
}

void CDRMPlot::SetData(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
	const int size = vecrData.Size();
	main1curve.SETDATA(size ? &vecrScale[0] : nullptr, size ? &vecrData[0] : nullptr, size);
}

void CDRMPlot::SetData(CVector<_REAL>& vecrData1, CVector<_REAL>& vecrData2,
                       CVector<_REAL>& vecrScale)
{
	const int size1 = vecrData1.Size();
	main1curve.SETDATA(size1 ? &vecrScale[0] : nullptr, size1 ? &vecrData1[0] : nullptr, size1);
	const int size2 = vecrData2.Size();
	main2curve.SETDATA(size2 ? &vecrScale[0] : nullptr, size2 ? &vecrData2[0] : nullptr, size2);
}

void CDRMPlot::SetData(QwtPlotCurve& curve, CVector<_COMPLEX>& veccData)
{
	const int size = veccData.Size();
	if (size)
	{
		vector<double> r(size), im(size);
		for(int i=0; i<size; i++)
		{
			r[i] = veccData[i].real();
			im[i] = veccData[i].imag();
		}
		curve.SETDATA(&r[0], &im[0], size);
	}
	else
		curve.SETDATA(nullptr, nullptr, 0);
}

void CDRMPlot::SetData(CVector<_COMPLEX>& veccData)
{
	SetData(curve1, veccData);
}

void CDRMPlot::SetData(CVector<_COMPLEX>& veccMSCConst,
                       CVector<_COMPLEX>& veccSDCConst,
                       CVector<_COMPLEX>& veccFACConst)
{
	SetData(curve1, veccMSCConst);
	SetData(curve2, veccSDCConst);
	SetData(curve3, veccFACConst);
}

void CDRMPlot::PlotDefaults()
{
	/* Set default value of plot items */
	Canvas = QPixmap();
	Image = QImage();
	curve1.detach();
	curve2.detach();
	curve3.detach();
	curve4.detach();
	curve5.detach();
	hcurvegrid.detach();
	vcurvegrid.detach();
	main1curve.detach();
	main2curve.detach();
	curve1.SETDATA(nullptr, nullptr, 0);
	curve2.SETDATA(nullptr, nullptr, 0);
	curve3.SETDATA(nullptr, nullptr, 0);
	curve4.SETDATA(nullptr, nullptr, 0);
	curve5.SETDATA(nullptr, nullptr, 0);
	hcurvegrid.SETDATA(nullptr, nullptr, 0);
	vcurvegrid.SETDATA(nullptr, nullptr, 0);
	main1curve.SETDATA(nullptr, nullptr, 0);
	main2curve.SETDATA(nullptr, nullptr, 0);
#if QWT_VERSION < 0x060000
	curve1.setSymbol(QwtSymbol());
    curve2.setSymbol(QwtSymbol());
	curve3.setSymbol(QwtSymbol());
#else
	curve1.setSymbol(nullptr);
	curve2.setSymbol(nullptr);
	curve3.setSymbol(nullptr);
	curve1.setLegendAttribute(QwtPlotCurve::LegendShowSymbol, false);
	curve2.setLegendAttribute(QwtPlotCurve::LegendShowSymbol, false);
	curve3.setLegendAttribute(QwtPlotCurve::LegendShowSymbol, false);
#endif
	grid.enableX(true);
	grid.enableY(true);
	grid.SETMAJORPEN(QPen(MainGridColorPlot, 0, Qt::DotLine));
	grid.SETMINORPEN(QPen(MainGridColorPlot, 0, Qt::DotLine));
	curve1.setItemAttribute(QwtPlotItem::Legend, false);
	curve2.setItemAttribute(QwtPlotItem::Legend, false);
	curve3.setItemAttribute(QwtPlotItem::Legend, false);
	main1curve.setItemAttribute(QwtPlotItem::Legend, false);
	main2curve.setItemAttribute(QwtPlotItem::Legend, false);
	plot->setCanvasBackground(QColor(BckgrdColorPlot));
	if (WaterfallWidget != nullptr)
	{
		delete WaterfallWidget;
		WaterfallWidget = nullptr;
	}
}

void CDRMPlot::PlotSetLegendFont()
{
#if QWT_VERSION < 0x060100
	foreach(QWidget* item, legend->legendItems())
		item->setFont(legend->font());
#endif
}

void CDRMPlot::PlotForceUpdate()
{
	/* Force the plot to update */
#if QWT_VERSION >= 0x050200
	plot->updateAxes();
	plot->updateLayout();
#else
	plot->replot();
#endif

	/* Get window background color */
	const QPalette& palette(plot->palette());
	BackgroundColor = palette.color(QPalette::Window);

	/* Get/Set various size */
	scaleWidth = plot->axisScaleDraw(QwtPlot::xBottom)->length();
#if QWT_VERSION >= 0x060000
	QRect rect(plot->plotLayout()->canvasRect().toRect());
#else
	QRect rect(plot->plotLayout()->canvasRect());
#endif
	QSize CanvSize(scaleWidth, rect.height());
	LastPlotCanvRect = rect;

	/* Update waterfall widget geometry */
	WaterfallWidget->setGeometry(rect.x() + (rect.width()-scaleWidth)/2, rect.y(), CanvSize.width(), CanvSize.height());

	/* Allocate the new canvas */
	Canvas = QPixmap(CanvSize);
	Canvas.fill(BackgroundColor);
	Image = QImage(scaleWidth, 1, QImage::Format_RGB32);
	imageData = (QRgb*)Image.bits();
}

void CDRMPlot::SetupAvIR()
{
	/* Init chart for averaged impulse response */
	plot->setTitle(tr("Channel Impulse Response"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Time [ms]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("IR [dB]"));

	/* Curves color */
	curve1.setPen(QPen(SpecLine1ColorPlot, 1, MARKER_STYLE));
	curve2.setPen(QPen(SpecLine1ColorPlot, 1, MARKER_STYLE));
	curve3.setPen(QPen(SpecLine2ColorPlot, 1, MARKER_STYLE));
	curve4.setPen(QPen(SpecLine2ColorPlot, 1, MARKER_STYLE));
	curve5.setPen(QPen(SpecLine1ColorPlot, 1, MARKER_STYLE));
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	/* Curves title */
	main1curve.setTitle(tr("Channel Impulse Response"));

	/* Insert curves */
	curve1.attach(plot);
	curve2.attach(plot);
	curve3.attach(plot);
	curve4.attach(plot);
	curve5.attach(plot);
	main1curve.attach(plot);
}


void CDRMPlot::SetVerticalBounds(
	const _REAL rStartGuard, const _REAL rEndGuard,
	const _REAL rBeginIR, const _REAL rEndIR)
{
	/* Fixed scale */
	const double cdAxMinLeft = (double) -20.0;
	const double cdAxMaxLeft = (double) 40.0;
	plot->setAxisScale(QwtPlot::yLeft, cdAxMinLeft, cdAxMaxLeft);

	/* Vertical bounds -------------------------------------------------- */
	double dX[2], dY[2];

	/* These bounds show the beginning and end of the guard-interval */
	dY[0] = cdAxMinLeft;
	dY[1] = cdAxMaxLeft;

	/* Left bound */
	dX[0] = dX[1] = rStartGuard;
	curve1.SETDATA(dX, dY, 2);

	/* Right bound */
	dX[0] = dX[1] = rEndGuard;
	curve2.SETDATA(dX, dY, 2);

	/* Estimated begin of impulse response */
	dX[0] = dX[1] = rBeginIR;
	curve3.SETDATA(dX, dY, 2);

	/* Estimated end of impulse response */
	dX[0] = dX[1] = rEndIR;
	curve4.SETDATA(dX, dY, 2);
}

void CDRMPlot::SetHorizontalBounds( _REAL rScaleMin, _REAL rScaleMax, _REAL rLowerB, _REAL rHigherB)
{
	/* Adjust scale for x-axis */
	plot->setAxisScale(QwtPlot::xBottom, (double) rScaleMin, (double) rScaleMax);

	double dX[2], dY[2];
	/* These bounds show the peak detection bound from timing tracking */
	dX[0] = rScaleMin;
	dX[1] = rScaleMax;

	/* Only include highest bound */
	dY[0] = dY[1] = Max(rHigherB, rLowerB);
	curve5.SETDATA(dX, dY, 2);
}

void CDRMPlot::SetupTranFct()
{
	/* Init chart for transfer function */
	plot->setTitle(tr("Channel Transfer Function / Group Delay"));
	plot->enableAxis(QwtPlot::yRight);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Carrier Index"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("TF [dB]"));

	plot->setAxisTitle(QwtPlot::yRight, tr("Group Delay [ms]"));
	plot->setAxisScale(QwtPlot::yRight, (double) -50.0, (double) 50.0);

	/* Fixed scale */
	plot->setAxisScale(QwtPlot::yLeft, (double) -85.0, (double) -35.0);

	/* Curve colors */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	main2curve.setPen(QPen(SpecLine2ColorPlot, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	/* Set legends */
	main1curve.setItemAttribute(QwtPlotItem::Legend, true);
	main2curve.setItemAttribute(QwtPlotItem::Legend, true);

	/* Add main curves */
	main1curve.setTitle(tr("Transf. Fct."));
	main1curve.attach(plot);
	main2curve.setTitle(tr("Group Del."));
	main2curve.setYAxis(QwtPlot::yRight);
	main2curve.attach(plot);
	PlotSetLegendFont();
}

void CDRMPlot::SetupAudioSpec(bool bAudioDecoder)
{
	/* Init chart for audio spectrum */
	if (bAudioDecoder)
		plot->setTitle(tr("Audio Spectrum"));
	else
	{
		plot->setTitle(tr("No audio decoding possible"));
		main1curve.SETDATA(nullptr, nullptr, 0);
	}
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency [kHz]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, "AS [dB]");

	/* Fixed scale */
	plot->setAxisScale(QwtPlot::yLeft, (double) -100.0, (double) -20.0);
	int iMaxAudioFrequency = MAX_SPEC_AUDIO_FREQUENCY;
	if (iMaxAudioFrequency > iAudSampleRate/2)
		iMaxAudioFrequency = iAudSampleRate/2;
	const double dBandwidth = double(iMaxAudioFrequency) / 1000;
	plot->setAxisScale(QwtPlot::xBottom, (double) 0.0, dBandwidth);

	/* Curve color */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	/* Add main curve */
	main1curve.setTitle(tr("Audio Spectrum"));
	main1curve.attach(plot);
}

void CDRMPlot::SetupFreqSamOffsHist()
{
	/* Init chart for transfer function. Enable right axis, too */
	plot->setTitle(tr("Rel. Frequency Offset / Sample Rate Offset History"));
	plot->enableAxis(QwtPlot::yRight);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Time [s]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yRight, tr("Sample Rate Offset [Hz]"));

	/* Curve colors */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));
	main2curve.setPen(QPen(SpecLine2ColorPlot, 1, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));

	/* Set legends */
	main1curve.setItemAttribute(QwtPlotItem::Legend, true);
	main2curve.setItemAttribute(QwtPlotItem::Legend, true);

	/* Add main curves */
	main1curve.setTitle(tr("Freq."));
	main1curve.attach(plot);
	main2curve.setTitle(tr("Samp."));
	main2curve.setYAxis(QwtPlot::yRight);
	main2curve.attach(plot);
	PlotSetLegendFont();
}

void CDRMPlot::AutoScale(CVector<_REAL>& vecrData,
                         CVector<_REAL>& vecrData2,
                         CVector<_REAL>& vecrScale)
{
	/* Customized auto-scaling. We adjust the y scale so that it is not larger
	   than rMinScaleRange"  */
	const _REAL rMinScaleRange = (_REAL) 1.0; /* Hz */

	/* Get maximum and minimum values */
	_REAL MaxFreq = -_MAXREAL;
	_REAL MinFreq = _MAXREAL;
	_REAL MaxSam = -_MAXREAL;
	_REAL MinSam = _MAXREAL;

	const int iSize = vecrScale.Size();
	for (int i = 0; i < iSize; i++)
	{
		if (vecrData[i] > MaxFreq)
			MaxFreq = vecrData[i];

		if (vecrData[i] < MinFreq)
			MinFreq = vecrData[i];

		if (vecrData2[i] > MaxSam)
			MaxSam = vecrData2[i];

		if (vecrData2[i] < MinSam)
			MinSam = vecrData2[i];
	}

	/* Apply scale to plot */
	plot->setAxisScale(QwtPlot::yLeft, (double) Floor(MinFreq / rMinScaleRange),
		(double) Ceil(MaxFreq / rMinScaleRange));
	plot->setAxisScale(QwtPlot::yRight, (double) Floor(MinSam / rMinScaleRange),
		(double) Ceil(MaxSam / rMinScaleRange));
	plot->setAxisScale(QwtPlot::xBottom, (double) vecrScale[0], (double) 0.0);
}

void CDRMPlot::SetupDopplerDelayHist()
{
	/* Init chart for transfer function. Enable right axis, too */
	plot->setTitle(tr("Delay / Doppler History"));
	plot->enableAxis(QwtPlot::yRight);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Time [min]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Delay [ms]"));
	plot->setAxisTitle(QwtPlot::yRight, tr("Doppler [Hz]"));

	/* Fixed scale */
	plot->setAxisScale(QwtPlot::yLeft, (double) 0.0, (double) 10.0);
	plot->setAxisScale(QwtPlot::yRight, (double) 0.0, (double) 4.0);

	/* Curve colors */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));
	main2curve.setPen(QPen(SpecLine2ColorPlot, 1, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));

	/* Set legends */
	main1curve.setItemAttribute(QwtPlotItem::Legend, true);
	main2curve.setItemAttribute(QwtPlotItem::Legend, true);

	/* Add main curves */
	main1curve.setTitle(tr("Delay"));
	main1curve.attach(plot);
	main2curve.setTitle(tr("Doppler"));
	main2curve.setYAxis(QwtPlot::yRight);
	main2curve.attach(plot);
	PlotSetLegendFont();
}

void CDRMPlot::SetupSNRAudHist()
{
	/* Init chart for transfer function. Enable right axis, too */
	plot->setTitle(tr("SNR / Correctly Decoded Audio History"));
	plot->enableAxis(QwtPlot::yRight);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Time [min]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("SNR [dB]"));
	plot->setAxisTitle(QwtPlot::yRight, tr("Corr. Dec. Audio / DRM-Frame"));

	/* Curve colors */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));
	main2curve.setPen(QPen(SpecLine2ColorPlot, 1, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));

	/* Set legends */
	main1curve.setItemAttribute(QwtPlotItem::Legend, true);
	main2curve.setItemAttribute(QwtPlotItem::Legend, true);

	/* Add main curves */
	main1curve.setTitle(tr("SNR"));
	main1curve.attach(plot);
	main2curve.setTitle(tr("Audio"));
	main2curve.setYAxis(QwtPlot::yRight);
	main2curve.attach(plot);
	PlotSetLegendFont();
}

void CDRMPlot::AutoScale2(CVector<_REAL>& vecrData,
                          CVector<_REAL>& vecrData2,
                          CVector<_REAL>& vecrScale)
{
	(void)vecrData2;

	/* Customized auto-scaling. We adjust the y scale maximum so that it
	   is not more than "rMaxDisToMax" to the curve */
	const int iMaxDisToMax = 5; /* dB */
	const int iMinValueSNRYScale = 15; /* dB */

	/* Get maximum value */
	_REAL MaxSNR = -_MAXREAL;

	const int iSize = vecrScale.Size();
	for (int i = 0; i < iSize; i++)
	{
		if (vecrData[i] > MaxSNR)
			MaxSNR = vecrData[i];
	}

	/* Quantize scale to a multiple of "iMaxDisToMax" */
	double dMaxYScaleSNR =
		(double) (Ceil(MaxSNR / iMaxDisToMax) * iMaxDisToMax);

	/* Bound at the minimum allowed value */
	if (dMaxYScaleSNR < (double) iMinValueSNRYScale)
		dMaxYScaleSNR = (double) iMinValueSNRYScale;

	/* Ratio between the maximum values for audio and SNR. The ratio should be
	   chosen so that the audio curve is not in the same range as the SNR curve
	   under "normal" conditions to increase readability of curves.
	   Since at very low SNRs, no audio can received anyway so we do not have to
	   check whether the audio y-scale is in range of the curve */
	const _REAL rRatioAudSNR = (double) 1.5;
	const double dMaxYScaleAudio = dMaxYScaleSNR * (double) rRatioAudSNR;

	/* Apply scale to plot */
	plot->setAxisScale(QwtPlot::yLeft, (double) 0.0, dMaxYScaleSNR);
	plot->setAxisScale(QwtPlot::yRight, (double) 0.0, dMaxYScaleAudio);
	plot->setAxisScale(QwtPlot::xBottom, (double) vecrScale[0], (double) 0.0);
}

void CDRMPlot::AutoScale3(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
	const int iSize = vecrScale.Size();

	/* Fixed scale for x-axis */
	plot->setAxisScale(QwtPlot::xBottom, (double) 0.0, (double) iSize);

	/* Fixed / variable scale (if SNR is in range, use fixed scale otherwise enlarge scale) */
	/* Get maximum value */
	_REAL rMaxSNR = -_MAXREAL;
	for (int i = 0; i < iSize; i++)
	{
		if (vecrData[i] > rMaxSNR)
			rMaxSNR = vecrData[i];
	}

	double dMaxScaleYAxis = MAX_VAL_SNR_SPEC_Y_AXIS_DB;

	if (rMaxSNR > dMaxScaleYAxis)
	{
		const double rEnlareStep = (double) 10.0; /* dB */
		dMaxScaleYAxis = ceil(rMaxSNR / rEnlareStep) * rEnlareStep;
	}

	/* Set scale */
	plot->setAxisScale(QwtPlot::yLeft, MIN_VAL_SNR_SPEC_Y_AXIS_DB, dMaxScaleYAxis);
}

void CDRMPlot::SetupPSD()
{
	/* Init chart for power spectram density estimation */
	plot->setTitle(tr("Shifted Power Spectral Density of Input Signal"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency [kHz]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("PSD [dB]"));

	/* Fixed scale */
	plot->setAxisScale(QwtPlot::xBottom,
		(double) 0.0, (double) iSigSampleRate / 2000);

	plot->setAxisScale(QwtPlot::yLeft, MIN_VAL_SHIF_PSD_Y_AXIS_DB,
		MAX_VAL_SHIF_PSD_Y_AXIS_DB);

	double dX[2], dY[2];
    dX[0] = dX[1] = VIRTUAL_INTERMED_FREQ / 1000.0;

	/* Take the min-max values from scale to get vertical line */
	dY[0] = MIN_VAL_SHIF_PSD_Y_AXIS_DB;
	dY[1] = MAX_VAL_SHIF_PSD_Y_AXIS_DB;

	curve1.SETDATA(dX, dY, 2);

	/* Insert line for DC carrier */
	curve1.setPen(QPen(SpecLine1ColorPlot, 1, MARKER_STYLE));
	curve1.attach(plot);

	/* Curve color */
	main1curve.setPen(QPen(MainPenColorPlot, 1, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));

	/* Add main curve */
	main1curve.setTitle(tr("Shifted PSD"));
	main1curve.attach(plot);
}

void CDRMPlot::SetupSNRSpectrum()
{
	/* Init chart for power spectram density estimation */
	plot->setTitle(tr("SNR Spectrum (Weighted MER on MSC Cells)"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Carrier Index"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("WMER [dB]"));

	/* Curve color */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));

	/* Add main curve */
	main1curve.setTitle(tr("SNR Spectrum"));
	main1curve.attach(plot);
}

void CDRMPlot::SetupInpSpec()
{
	/* Init chart for power spectram density estimation */
	plot->setTitle(tr("Input Spectrum"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency [kHz]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Input Spectrum [dB]"));

	/* Fixed scale */
    const double dXScaleMin = pDRMRec->ConvertFrequency((_REAL) 0.0) / 1000;
    const double dXScaleMax = pDRMRec->ConvertFrequency((_REAL) iSigSampleRate / 2) / 1000;
	plot->setAxisScale(QwtPlot::xBottom, dXScaleMin, dXScaleMax);

	plot->setAxisScale(QwtPlot::yLeft, MIN_VAL_INP_SPEC_Y_AXIS_DB,
		MAX_VAL_INP_SPEC_Y_AXIS_DB);

	/* Insert line for DC carrier */
	curve1.setPen(QPen(SpecLine1ColorPlot, 1, MARKER_STYLE));
	curve1.attach(plot);

	/* Curve color */
	main1curve.setPen(QPen(MainPenColorPlot, 1, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));

	/* Add main curve */
	main1curve.setTitle(tr("Input spectrum"));
	main1curve.attach(plot);
}

void CDRMPlot::SetDCCarrier(const _REAL rDCFreq)
{
	/* Insert line for DC carrier */
	double dX[2], dY[2];
	dX[0] = dX[1] =
        pDRMRec->ConvertFrequency(rDCFreq) / 1000;

	/* Take the min-max values from scale to get vertical line */
	dY[0] = MIN_VAL_INP_SPEC_Y_AXIS_DB;
	dY[1] = MAX_VAL_INP_SPEC_Y_AXIS_DB;

	curve1.SETDATA(dX, dY, 2);
}

void CDRMPlot::SetupInpPSD(bool bAnalog)
{
	/* Init chart for power spectram density estimation */
	plot->setTitle(tr("Input PSD"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency [kHz]"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Input PSD [dB]"));

	/* Fixed scale */
    const double dXScaleMin = pDRMRec->ConvertFrequency((_REAL) 0.0) / 1000;
    const double dXScaleMax = pDRMRec->ConvertFrequency((_REAL) iSigSampleRate / 2) / 1000;
	plot->setAxisScale(QwtPlot::xBottom, dXScaleMin, dXScaleMax);

    plot->setAxisScale(QwtPlot::yLeft, MIN_VAL_INP_SPEC_Y_AXIS_DB, MAX_VAL_INP_SPEC_Y_AXIS_DB);

	/* Insert line for bandwidth marker */
	if (bAnalog)
	{
		QColor color = PassBandColorPlot;
		color.setAlpha(125);
		QBrush brush(color);
		brush.setStyle(Qt::SolidPattern);
		curve2.setBrush(QBrush(color));
		curve2.setBaseline(MIN_VAL_INP_SPEC_Y_AXIS_DB);
		curve2.setPen(QPen(Qt::NoPen));
		curve2.attach(plot);
	}

	/* Insert line for DC carrier */
	curve1.setPen(QPen(SpecLine1ColorPlot, 1, MARKER_STYLE));
	curve1.attach(plot);

	/* Curve color */
	main1curve.setPen(QPen(MainPenColorPlot, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	/* Add main curve */
	main1curve.setTitle(tr("Input PSD"));
	main1curve.attach(plot);
}

void CDRMPlot::SetBWMarker(const _REAL rBWCenter, const _REAL rBWWidth)
{
	double	dX[2], dY[2];
	/* Insert marker for filter bandwidth if required */
	if (rBWWidth != (_REAL) 0.0)
	{
        dX[0] = pDRMRec->ConvertFrequency(
			(rBWCenter - rBWWidth / 2) * (double)iSigSampleRate) / 1000.0;
        dX[1] = pDRMRec->ConvertFrequency(
			(rBWCenter + rBWWidth / 2) * (double)iSigSampleRate) / 1000.0;

		/* Take the min-max values from scale to get vertical line */
		dY[0] = MAX_VAL_INP_SPEC_Y_AXIS_DB;//MIN_VAL_INP_SPEC_Y_AXIS_DB;
		dY[1] = MAX_VAL_INP_SPEC_Y_AXIS_DB;//MIN_VAL_INP_SPEC_Y_AXIS_DB;

		curve2.SETDATA(dX, dY, 2);
	}
	else
		curve2.SETDATA(nullptr, nullptr, 0);
}

void CDRMPlot::SetupInpSpecWaterf()
{
	/* Init chart for waterfall input spectrum */
	plot->setTitle(tr("Waterfall Input Spectrum"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency [kHz]"));
	plot->enableAxis(QwtPlot::yLeft, false);
	grid.enableX(false);
	grid.enableY(false);

	/* Create a new waterfall widget if not exists */
	if (WaterfallWidget == nullptr)
		WaterfallWidget = new CWidget(plot, Canvas);

	/* Set plot background color */
	const QPalette& palette(plot->palette());
	plot->setCanvasBackground(palette.color(QPalette::Window));

	/* Fixed scale */
    const double dXScaleMin = pDRMRec->ConvertFrequency((_REAL) 0.0) / 1000;
    const double dXScaleMax = pDRMRec->ConvertFrequency((_REAL) iSigSampleRate / 2) / 1000;
	plot->setAxisScale(QwtPlot::xBottom, dXScaleMin, dXScaleMax);
}

void CDRMPlot::SetInpSpecWaterf(CVector<_REAL>& vecrData, CVector<_REAL>&)
{
	/* No WaterfallWidget so return */
	if (WaterfallWidget == nullptr)
		return;

	/* Check if the canvas size has changed */
	bool bWidthChanged, bHeightChanged;
	QSize CanvSize(Canvas.size());
#if QWT_VERSION >= 0x060000
	QRect PlotCanvRect(plot->plotLayout()->canvasRect().toRect());
#else
	QRect PlotCanvRect(plot->plotLayout()->canvasRect());
#endif
	bWidthChanged = LastPlotCanvRect.width() != PlotCanvRect.width();
	bHeightChanged = LastPlotCanvRect.height() != PlotCanvRect.height();
	if (Canvas.size().isEmpty() || bWidthChanged)
	{
		/* Force plot update */
		PlotForceUpdate();

		/* Update current canvas size */
		CanvSize = Canvas.size();
	}

	/* If only the height has changed, then copy the content
	   to the newly allocated Canvas */
	if (!bWidthChanged && bHeightChanged)
	{
		QPixmap tmpPixmap = QPixmap(Canvas);
		Canvas = QPixmap(CanvSize);

		/* Force plot update */
		PlotForceUpdate();

		/* Update current canvas size */
		CanvSize = Canvas.size();
		if (CanvSize.isEmpty())
			return;

		QPainter p(&Canvas);
		int width = CanvSize.width();
		int height = CanvSize.height();
		p.drawPixmap(0, 0, width, height, tmpPixmap, 0, 0, width, height);
	}

	/* Check if the canvas is valid */
	if (CanvSize.isEmpty())
		return;

	/* Resample input data */
	CVector<_REAL> *pvecrResampledData;
	Resample.Resample(&vecrData, &pvecrResampledData, scaleWidth, true);

	/* The scaling factor */
	const _REAL rScale = _REAL(pvecrResampledData->Size()) / scaleWidth;

	/* Actual waterfall data */
	QColor color;
	for (int i = 0; i < scaleWidth; i++)
	{
		/* Init some constants */
		const int iMaxHue = 359; /* Range of "Hue" is 0-359 */
		const int iMaxSat = 255; /* Range of saturation is 0-255 */

		/* Stretch width to entire canvas width */
		const int iCurIdx =
			(int) Round(_REAL(i) * rScale);

		/* Translate dB-values in colors */
		const int iCurCol =
			(int) Round(((*pvecrResampledData)[iCurIdx] - MIN_VAL_INP_SPEC_Y_AXIS_DB) /
			(MAX_VAL_INP_SPEC_Y_AXIS_DB - MIN_VAL_INP_SPEC_Y_AXIS_DB) *
			iMaxHue);

		/* Reverse colors and add some offset (to make it look a bit nicer) */
		const int iColOffset = 60;
		int iFinalCol = iMaxHue - iColOffset - iCurCol;
		if (iFinalCol < 0) /* Prevent from out-of-range */
			iFinalCol = 0;

		/* Also change saturation to get dark colors when low level */
		int iCurSat = (int) ((1 - (_REAL) iFinalCol / iMaxHue) * iMaxSat);
		if (iCurSat < 0) /* Prevent from out-of-range */
			iCurSat = 0;

		/* Generate pixel */
		color.setHsv(iFinalCol, iCurSat, iCurSat);
		imageData[i] = color.rgb();
	}

	/* Scroll Canvas */
	Canvas.scroll(0, 1, 0, 0, CanvSize.width(), CanvSize.height(), 0);

	/* Paint new line (top line) */
	QPainter Painter;
	if (Painter.begin(&Canvas)) {
		/* Draw pixel line to canvas */
		Painter.drawImage(0, 0, Image);

		Painter.end();

		WaterfallWidget->repaint();
	}
}

void CDRMPlot::SetupFACConst()
{
	/* Init chart for FAC constellation */
	plot->setTitle(tr("FAC Constellation"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Real"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Imaginary"));

	/* Set fixed scale */
	SetQAMGrid(CS_1_SM);

#if QWT_VERSION < 0x060000
	QwtSymbol *MarkerSym1_ptr = &MarkerSym1;
#else
	QwtSymbol *MarkerSym1, *MarkerSym1_ptr;
	MarkerSym1 = MarkerSym1_ptr = new QwtSymbol();
#endif

	/* Set marker symbol */
	MarkerSym1_ptr->setStyle(QwtSymbol::Ellipse);
	MarkerSym1_ptr->setSize(FAC_SYMBOL_SIZE);
	MarkerSym1_ptr->setPen(QPen(MainPenColorConst));
	MarkerSym1_ptr->setBrush(QBrush(MainPenColorConst));

	curve1.setPen(QPen(Qt::NoPen));
	curve1.setSymbol(MarkerSym1);
	curve1.attach(plot);
}

void CDRMPlot::SetupSDCConst(const ECodScheme eNewCoSc)
{
	/* Init chart for SDC constellation */
	plot->setTitle(tr("SDC Constellation"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Real"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Imaginary"));

	/* Set fixed scale */
	SetQAMGrid(eNewCoSc);

#if QWT_VERSION < 0x060000
	QwtSymbol *MarkerSym1_ptr = &MarkerSym1;
#else
	QwtSymbol *MarkerSym1, *MarkerSym1_ptr;
	MarkerSym1 = MarkerSym1_ptr = new QwtSymbol();
#endif

	/* Set marker symbol */
	MarkerSym1_ptr->setStyle(QwtSymbol::Ellipse);
	MarkerSym1_ptr->setSize(SDC_SYMBOL_SIZE);
	MarkerSym1_ptr->setPen(QPen(MainPenColorConst));
	MarkerSym1_ptr->setBrush(QBrush(MainPenColorConst));

	curve1.setPen(QPen(Qt::NoPen));
	curve1.setSymbol(MarkerSym1);
	curve1.attach(plot);
}

void CDRMPlot::SetupMSCConst(const ECodScheme eNewCoSc)
{
	/* Init chart for MSC constellation */
	plot->setTitle(tr("MSC Constellation"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Real"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Imaginary"));

	/* Set fixed scale */
	SetQAMGrid(eNewCoSc);

#if QWT_VERSION < 0x060000
	QwtSymbol *MarkerSym1_ptr = &MarkerSym1;
#else
	QwtSymbol *MarkerSym1, *MarkerSym1_ptr;
	MarkerSym1 = MarkerSym1_ptr = new QwtSymbol();
#endif

	/* Set marker symbol */
	MarkerSym1_ptr->setStyle(QwtSymbol::Ellipse);
	MarkerSym1_ptr->setSize(MSC_SYMBOL_SIZE);
	MarkerSym1_ptr->setPen(QPen(MainPenColorConst));
	MarkerSym1_ptr->setBrush(QBrush(MainPenColorConst));

	curve1.setPen(QPen(Qt::NoPen));
	curve1.setSymbol(MarkerSym1);
	curve1.attach(plot);
}

void CDRMPlot::SetupAllConst()
{
	/* Init chart for constellation */
	plot->setTitle(tr("MSC / SDC / FAC Constellation"));
	plot->enableAxis(QwtPlot::yRight, false);
	plot->setAxisTitle(QwtPlot::xBottom, tr("Real"));
	plot->enableAxis(QwtPlot::yLeft, true);
	plot->setAxisTitle(QwtPlot::yLeft, tr("Imaginary"));

	/* Set fixed scale */
	SetGrid(1.5, 3, 5);

#if QWT_VERSION < 0x060000
	QwtSymbol *MarkerSym1_ptr;
	QwtSymbol *MarkerSym2_ptr;
	QwtSymbol *MarkerSym3_ptr;
	MarkerSym1_ptr = &MarkerSym1;
	MarkerSym2_ptr = &MarkerSym2;
	MarkerSym3_ptr = &MarkerSym3;
#else
	QwtSymbol *MarkerSym1, *MarkerSym1_ptr;
	QwtSymbol *MarkerSym2, *MarkerSym2_ptr;
	QwtSymbol *MarkerSym3, *MarkerSym3_ptr;
	MarkerSym1 = MarkerSym1_ptr = new QwtSymbol();
	MarkerSym2 = MarkerSym2_ptr = new QwtSymbol();
	MarkerSym3 = MarkerSym3_ptr = new QwtSymbol();
	curve1.setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
	curve2.setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
	curve3.setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
#endif

	/* Set marker symbols */
	/* MSC */
	MarkerSym1_ptr->setStyle(QwtSymbol::Rect);
	MarkerSym1_ptr->setSize(ALL_MSC_SYMBOL_SIZE);
	MarkerSym1_ptr->setPen(QPen(MainPenColorConst));
	MarkerSym1_ptr->setBrush(QBrush(MainPenColorConst));
	/* SDC */
	MarkerSym2_ptr->setStyle(QwtSymbol::XCross);
	MarkerSym2_ptr->setSize(ALL_SDC_SYMBOL_SIZE);
	MarkerSym2_ptr->setPen(QPen(SpecLine1ColorPlot));
	MarkerSym2_ptr->setBrush(QBrush(SpecLine1ColorPlot));
	/* FAC */
	MarkerSym3_ptr->setStyle(QwtSymbol::Ellipse);
	MarkerSym3_ptr->setSize(ALL_FAC_SYMBOL_SIZE);
	MarkerSym3_ptr->setPen(QPen(SpecLine2ColorPlot));
	MarkerSym3_ptr->setBrush(QBrush(SpecLine2ColorPlot));

	/* Insert "dummy" curves for legend */
	curve1.setTitle(QString("MSC"));
	curve1.setSymbol(MarkerSym1);
	curve1.setPen(QPen(Qt::NoPen));
	curve1.setItemAttribute(QwtPlotItem::Legend, true);
	curve2.setTitle(QString("SDC"));
	curve2.setSymbol(MarkerSym2);
	curve2.setPen(QPen(Qt::NoPen));
	curve2.setItemAttribute(QwtPlotItem::Legend, true);
	curve3.setTitle(QString("FAC"));
	curve3.setSymbol(MarkerSym3);
	curve3.setPen(QPen(Qt::NoPen));
	curve3.setItemAttribute(QwtPlotItem::Legend, true);

	curve1.attach(plot);
	curve2.attach(plot);
	curve3.attach(plot);
	PlotSetLegendFont();
}

void CDRMPlot::SetGrid(double div, int step, int substep)
{
	int i;
	_REAL pos;
#if QWT_VERSION < 0x060000
	QwtValueList ticks[3];
#else
	QList <double> ticks[3];
#endif

	for (i=0; i<=step*2; i++)
	{
		pos = -div + div / step * i;
		/* Keep 2 digit after the point */
		pos = Round(pos * 100.0) / 100.0;
		ticks[2].push_back((double)pos);
	}

	substep *= step;
	for (i=0; i<=substep*2; i++)
	{
		pos = -div + div / substep * i;
		/* Keep 2 digit after the point */
		pos = Round(pos * 100.0) / 100.0;
		ticks[1].push_back((double)pos);
	}

	/* Keep 2 digit after the point */
	div = (double)Round(div * 100.0) / 100.0;

	/* Set the scale */
	QwtScaleDiv scaleDiv(-div, div, ticks);
	plot->setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);
	plot->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
}

void CDRMPlot::SetQAMGrid(const ECodScheme eCoSc)
{
	switch (eCoSc)
	{
	case CS_1_SM:
		SetGrid(2.0 / sqrt(2.0), 1, 4); /* QAM4 */
		break;

	case CS_2_SM:
		SetGrid(4.0 / sqrt(10.0), 2, 4); /* QAM16 */
		break;

	default:
		SetGrid(8.0 / sqrt(42.0), 4, 4); /* QAM64 */
		break;
	}
}

#if QWT_VERSION < 0x060000
void CDRMPlot::OnSelected(const QwtDoublePoint &pos)
#else
void CDRMPlot::OnSelected(const QPointF &pos)
#endif
{
	/* Send normalized frequency to receiver */
#if QWT_VERSION < 0x060100
	const double dMaxxBottom = plot->axisScaleDiv(QwtPlot::xBottom)->UPPERBOUND();
#else
	const double dMaxxBottom = plot->axisScaleDiv(QwtPlot::xBottom).UPPERBOUND();
#endif

	/* Check if dMaxxBottom is valid */
	if (dMaxxBottom > 0.0)
	{
		/* Get frequency from current cursor position */
		double dFreq = pos.x();

		/* Emit signal containing normalized selected frequency */
		emit xAxisValSet(
            pDRMRec->ConvertFrequency(dFreq * 1000, true) /
            pDRMRec->ConvertFrequency(dMaxxBottom * 1000, true));
	}
}

void CDRMPlot::AddWhatsThisHelpChar(const ECharType NCharType)
{
	QString strCurPlotHelp;

	switch (NCharType)
	{
	case AVERAGED_IR:
		/* Impulse Response */
		strCurPlotHelp =
			tr("<b>Impulse Response:</b> This plot shows "
			   "the estimated Impulse Response (IR) of the channel based on the "
			   "channel estimation. It is the averaged, Hamming Window weighted "
			   "Fourier back transformation of the transfer function. The length "
			   "of PDS estimation and time synchronization tracking is based on "
			   "this function. The two red dashed vertical lines show the "
			   "beginning and the end of the guard-interval. The two black dashed "
			   "vertical lines show the estimated beginning and end of the PDS of "
			   "the channel (derived from the averaged impulse response "
			   "estimation). If the \"First Peak\" timing tracking method is "
			   "chosen, a bound for peak estimation (horizontal dashed red line) "
			   "is shown. Only peaks above this bound are used for timing "
			   "estimation.");
		break;

	case TRANSFERFUNCTION:
		/* Transfer Function */
		strCurPlotHelp =
			tr("<b>Transfer Function / Group Delay:</b> "
			   "This plot shows the squared magnitude and the group delay of "
			   "the estimated channel at each sub-carrier.");
		break;

	case FAC_CONSTELLATION:
	case SDC_CONSTELLATION:
	case MSC_CONSTELLATION:
	case ALL_CONSTELLATION:
		/* Constellations */
		strCurPlotHelp =
			tr("<b>FAC, SDC, MSC:</b> The plots show the "
			   "constellations of the FAC, SDC and MSC logical channel of the DRM "
			   "stream. Depending on the current transmitter settings, the SDC "
			   "and MSC can have 4-QAM, 16-QAM or 64-QAM modulation.");
		break;

	case POWER_SPEC_DENSITY:
		/* Shifted PSD */
		strCurPlotHelp =
			tr("<b>Shifted PSD:</b> This plot shows the "
			   "estimated Power Spectrum Density (PSD) of the input signal. The "
			   "DC frequency (red dashed vertical line) is fixed at 6 kHz. If "
			   "the frequency offset acquisition was successful, the rectangular "
			   "DRM spectrum should show up with a center frequency of 6 kHz. "
			   "This plot represents the frequency synchronized OFDM spectrum. "
			   "If the frequency synchronization was successful, the useful "
			   "signal really shows up only inside the actual DRM bandwidth "
			   "since the side loops have in this case only energy between the "
			   "samples in the frequency domain. On the sample positions outside "
			   "the actual DRM spectrum, the DRM signal has zero crossings "
			   "because of the orthogonality. Therefore this spectrum represents "
			   "NOT the actual spectrum but the \"idealized\" OFDM spectrum.");
			break;

	case SNR_SPECTRUM:
		/* SNR Spectrum (Weighted MER on MSC Cells) */
		strCurPlotHelp =
			tr("<b>SNR Spectrum (Weighted MER on MSC Cells):</b> "
			   "This plot shows the Weighted MER on MSC cells for each carrier "
			   "separately.");
			break;

	case INPUTSPECTRUM_NO_AV:
		/* Input Spectrum */
		strCurPlotHelp =
		tr("<b>Input Spectrum:</b> This plot shows the "
		   "Fast Fourier Transformation (FFT) of the input signal. This plot "
		   "is active in both modes, analog and digital. There is no "
		   "averaging applied. The screen shot of the Evaluation Dialog shows "
		   "the significant shape of a DRM signal (almost rectangular). The "
		   "dashed vertical line shows the estimated DC frequency. This line "
		   "is very important for the analog AM demodulation. Each time a "
		   "new carrier frequency is acquired, the red line shows the "
		   "selected AM spectrum. If more than one AM spectrums are within "
		   "the sound card frequency range, the strongest signal is chosen.");
		break;

    case INPUT_SIG_PSD:
    case INPUT_SIG_PSD_ANALOG:
		/* Input PSD */
		strCurPlotHelp =
			tr("<b>Input PSD:</b> This plot shows the "
			   "estimated power spectral density (PSD) of the input signal. The "
			   "PSD is estimated by averaging some Hamming Window weighted "
			   "Fourier transformed blocks of the input signal samples. The "
			   "dashed vertical line shows the estimated DC frequency.");
		break;

	case AUDIO_SPECTRUM:
		/* Audio Spectrum */
		strCurPlotHelp =
			tr("<b>Audio Spectrum:</b> This plot shows the "
			   "averaged audio spectrum of the currently played audio. With this "
			   "plot the actual audio bandwidth can easily determined. Since a "
			   "linear scale is used for the frequency axis, most of the energy "
			   "of the signal is usually concentrated on the far left side of the "
			   "spectrum.");
		break;

	case FREQ_SAM_OFFS_HIST:
		/* Frequency Offset / Sample Rate Offset History */
		strCurPlotHelp =
			tr("<b>Frequency Offset / Sample Rate Offset History:"
			   "</b> The history "
			   "of the values for frequency offset and sample rate offset "
			   "estimation is shown. If the frequency offset drift is very small, "
			   "this is an indication that the analog front end is of high "
			   "quality.");
		break;

	case DOPPLER_DELAY_HIST:
		/* Doppler / Delay History */
		strCurPlotHelp =
			tr("<b>Doppler / Delay History:</b> "
			   "The history of the values for the "
			   "Doppler and Impulse response length is shown. Large Doppler "
			   "values might be responsable for audio drop-outs.");
		break;

	case SNR_AUDIO_HIST:
		/* SNR History */
		strCurPlotHelp =
			tr("<b>SNR History:</b> "
			   "The history of the values for the "
			   "SNR and correctly decoded audio blocks is shown. The maximum "
			   "achievable number of correctly decoded audio blocks per DRM "
			   "frame is 10 or 5 depending on the audio sample rate (24 kHz "
			   "or 12 kHz AAC core bandwidth).");
		break;

	case INP_SPEC_WATERF:
		/* Waterfall Display of Input Spectrum */
		strCurPlotHelp =
			tr("<b>Waterfall Display of Input Spectrum:</b> "
			   "The input spectrum is displayed as a waterfall type. The "
			   "different colors represent different levels.");
		break;

	case NONE_OLD:
		break;
	}

	/* Main plot */
	plot->setWhatsThis(strCurPlotHelp);
}
