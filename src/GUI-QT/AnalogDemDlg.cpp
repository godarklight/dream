/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2005
 *
 * Author(s):
 *	Volker Fischer, Andrew Murphy
 *
 * Description:
 *
 * 11/21/2005 Andrew Murphy, BBC Research & Development, 2005
 *	- Additional widgets for displaying AMSS information
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

#include "AnalogDemDlg.h"
#include "DRMPlot.h"
#include <QMessageBox>
#include <QMenuBar>
#include <QString>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QToolTip>
#include <QButtonGroup>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QSlider>
#include <QLayout>
#include <QProgressBar>
#include <QComboBox>
#include <QWhatsThis>
#include <QDateTime>
#include <QCloseEvent>
#include <QInputDialog>
#include <qwt_dial.h>
#include <qwt_dial_needle.h>
#include <qwt_plot_layout.h>
#if QWT_VERSION >= 0x060100
# include <qwt_round_scale_draw.h>
#endif

/* Implementation *************************************************************/
AnalogDemDlg::AnalogDemDlg(CRx& nrx, CSettings& Settings,
	CFileMenu* pFileMenu, CSoundCardSelMenu* pSoundCardMenu, QWidget* parent) :
	CWindow(parent, Settings, "AM"),
    rx(nrx),
    AMSSDlg(nrx, Settings, this), MainPlot(nullptr),
	pFileMenu(pFileMenu), pSoundCardMenu(pSoundCardMenu)
{
	setupUi(this);

	/* Set help text for the controls */
	AddWhatsThisHelp();

	/* Add file and sound card menu */
	menuBar()->insertMenu(menu_View->menuAction(), pFileMenu);
	menu_Settings->addMenu(pSoundCardMenu);

	connect(action_Stations_Dialog, SIGNAL(triggered()), this, SIGNAL(ViewStationsDlg()));
	connect(action_Live_Schedule_Dialog, SIGNAL(triggered()), this, SIGNAL(ViewLiveScheduleDlg()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionAM, SIGNAL(triggered()), this, SIGNAL(NewAMAcquisition()));
	connect(actionFM, SIGNAL(triggered()), this, SLOT(OnSwitchToFM()));
	connect(actionDRM, SIGNAL(triggered()), this, SLOT(OnSwitchToDRM()));
    connect(pFileMenu, SIGNAL(soundFileChanged(QString)), this, SLOT(OnSoundFileChanged(QString)));
    connect(pSoundCardMenu, SIGNAL(soundSampleRateChanged(int)), this, SLOT(OnSampleRateChanged(int)));
	connect(actionAbout_Dream, SIGNAL(triggered()), this, SLOT(OnHelpAbout()));
	connect(actionWhats_This, SIGNAL(triggered()), this, SLOT(OnWhatsThis()));
	SliderBandwidth->setTickPosition(QSlider::TicksBothSides);
	MainPlot = new CDRMPlot(nullptr, plot);

	/* Init main plot */
	bool waterfall = getSetting("waterfall", false);
	ButtonWaterfall->setChecked(waterfall);
	if(MainPlot)
	{
        MainPlot->SetRecObj(&rx);
		MainPlot->SetPlotStyle(getSetting("plotstyle", 0, true));
		MainPlot->SetupChart(waterfall?CDRMPlot::INP_SPEC_WATERF:CDRMPlot::INPUT_SIG_PSD_ANALOG);
	}

	/* Add tool tip to show the user the possibility of choosing the AM IF */
	QString ptt = tr("Click on the plot to set the demodulation frequency");
	if(MainPlot)
	{
		MainPlot->plot->setToolTip(ptt);
	}

	/* Init bandwidth slider */
	UpdateSliderBandwidth();
	SliderBandwidth->setTickPosition(QSlider::TicksBothSides);
	SliderBandwidth->setTickInterval(1000); /* Each kHz a tick */
	SliderBandwidth->setPageStep(1000); /* Hz */

	/* Init PLL phase dial control */
	PhaseDial->setMode(QwtDial::RotateNeedle);
	PhaseDial->setWrapping(true);
	PhaseDial->setReadOnly(true);
#if QWT_VERSION < 0x060100
	PhaseDial->setScale(0, 360, 45); /* Degrees */
#else
	PhaseDial->setScale(0, 360); /* Degrees */
	PhaseDial->setScaleStepSize(45);
#endif
	PhaseDial->setOrigin(270);
	PhaseDial->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow));
	PhaseDial->setFrameShadow(QwtDial::Plain);
#if QWT_VERSION < 0x060000
	PhaseDial->setScaleOptions(QwtDial::ScaleTicks);
#elif QWT_VERSION < 0x060100
	PhaseDial->setScaleComponents(QwtAbstractScaleDraw::Ticks);
#else
	PhaseDial->setFrameShadow(QwtDial::Sunken);
	PhaseDial->setScaleMaxMinor(0);
	PhaseDial->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Labels, false);
#endif

#ifdef HAVE_SPEEX
    SpinBoxNoiRedLevel->setValue(rx.GetAMNoiseReductionLevel());
#else
	RadioButtonNoiRedSpeex->hide();
	SpinBoxNoiRedLevel->hide();
#endif

	/* Update controls */
	UpdateControls();


	/* Connect controls ----------------------------------------------------- */
	connect(ButtonDRM, SIGNAL(clicked()),
		this, SLOT(OnSwitchToDRM()));
	connect(ButtonAMSS, SIGNAL(clicked()),
		&AMSSDlg, SLOT(show()));
	connect(ButtonWaterfall, SIGNAL(clicked()),
		this, SLOT(OnButtonWaterfall()));
	connect(MainPlot, SIGNAL(xAxisValSet(double)),
		this, SLOT(OnChartxAxisValSet(double)));

	/* Button groups */
	connect(ButtonGroupDemodulation, SIGNAL(buttonClicked(int)),
		this, SLOT(OnRadioDemodulation(int)));
	connect(ButtonGroupAGC, SIGNAL(buttonClicked(int)),
		this, SLOT(OnRadioAGC(int)));
	connect(ButtonGroupNoiseReduction, SIGNAL(buttonClicked(int)),
		this, SLOT(OnRadioNoiRed(int)));

	/* Slider */
	connect(SliderBandwidth, SIGNAL(valueChanged(int)),
		this, SLOT(OnSliderBWChange(int)));

	/* Check boxes */
	connect(CheckBoxMuteAudio, SIGNAL(clicked()),
		this, SLOT(OnCheckBoxMuteAudio()));
	connect(CheckBoxSaveAudioWave, SIGNAL(clicked()),
		this, SLOT(OnCheckSaveAudioWAV()));
	connect(CheckBoxAutoFreqAcq, SIGNAL(clicked()),
		this, SLOT(OnCheckAutoFreqAcq()));
	connect(CheckBoxPLL, SIGNAL(clicked()),
		this, SLOT(OnCheckPLL()));

	/* Timers */
	connect(&Timer, SIGNAL(timeout()),
		this, SLOT(OnTimer()));
	connect(&TimerPLLPhaseDial, SIGNAL(timeout()),
		this, SLOT(OnTimerPLLPhaseDial()));
	connect(&TimerClose, SIGNAL(timeout()),
		this, SLOT(OnTimerClose()));

	/* Don't activate real-time timers, wait for show event */
}

void AnalogDemDlg::OnWhatsThis()
{
	QWhatsThis::enterWhatsThisMode();
}

void AnalogDemDlg::OnSwitchToDRM()
{
	emit SwitchMode(RM_DRM);
}

void AnalogDemDlg::OnSwitchToFM()
{
	emit SwitchMode(RM_FM);
}

void AnalogDemDlg::eventUpdate()
{
	/* Put (re)initialization code here for the settings that might have
	   be changed by another top level window. Called on mode switch */
    //pFileMenu->UpdateMenu();
    cerr << "AMDialog eventUpdate" << endl;
	UpdateSliderBandwidth();
}

void AnalogDemDlg::eventShow(QShowEvent*)
{
	OnTimer();
	OnTimerPLLPhaseDial();
	/* Set correct schedule */

	/* Activate real-time timers */
	Timer.start(GUI_CONTROL_UPDATE_TIME);
	TimerPLLPhaseDial.start(PLL_PHASE_DIAL_UPDATE_TIME);

	UpdateControls();

    /* Notify the MainPlot of showEvent */
    if(MainPlot) MainPlot->activate();
}

void AnalogDemDlg::eventHide(QHideEvent*)
{
    /* Notify the MainPlot of hideEvent */
    if(MainPlot) MainPlot->deactivate();

	/* stop real-time timers */
	Timer.stop();
	TimerPLLPhaseDial.stop();

	bool waterfall = ButtonWaterfall->isChecked();
	putSetting("waterfall", waterfall);
}

void AnalogDemDlg::eventClose(QCloseEvent* ce)
{
	if (!TimerClose.isActive())
	{
		/* Stop real-time timers */
		Timer.stop();
		TimerPLLPhaseDial.stop();

		/* Tell every other window to close too */
		emit Closed();

		/* Set the timer for polling the working thread state */
		TimerClose.start(50);
	}

	/* Stay open until working thread is done */
    if (true) // TODO rx.GetParameters()->eRunState == CParameter::STOPPED)
	{
        TimerClose.stop();
		ce->accept();
	}
	else
		ce->ignore();
}

void AnalogDemDlg::UpdateControls()
{
	/* Set demodulation type */
    switch (rx.GetAMDemodulationType())
	{
    case DT_AM:
		if (!RadioButtonDemAM->isChecked())
			RadioButtonDemAM->setChecked(true);
		break;

    case DT_LSB:
		if (!RadioButtonDemLSB->isChecked())
			RadioButtonDemLSB->setChecked(true);
		break;

    case DT_USB:
		if (!RadioButtonDemUSB->isChecked())
			RadioButtonDemUSB->setChecked(true);
		break;

    case DT_CW:
		if (!RadioButtonDemCW->isChecked())
			RadioButtonDemCW->setChecked(true);
		break;

    case DT_FM:
		if (!RadioButtonDemFM->isChecked())
			RadioButtonDemFM->setChecked(true);
		break;
	}

	/* Set AGC type */
    switch (rx.GetAMAGCType())
	{
    case AT_NO_AGC:
		if (!RadioButtonAGCOff->isChecked())
			RadioButtonAGCOff->setChecked(true);
		break;

    case AT_SLOW:
		if (!RadioButtonAGCSlow->isChecked())
			RadioButtonAGCSlow->setChecked(true);
		break;

    case AT_MEDIUM:
		if (!RadioButtonAGCMed->isChecked())
			RadioButtonAGCMed->setChecked(true);
		break;

    case AT_FAST:
		if (!RadioButtonAGCFast->isChecked())
			RadioButtonAGCFast->setChecked(true);
		break;
	}

	/* Set noise reduction type */
    switch (rx.GetAMNoiseReductionType())
	{
    case NR_OFF:
		if (!RadioButtonNoiRedOff->isChecked())
			RadioButtonNoiRedOff->setChecked(true);
		break;

    case NR_LOW:
		if (!RadioButtonNoiRedLow->isChecked())
			RadioButtonNoiRedLow->setChecked(true);
		break;

    case NR_MEDIUM:
		if (!RadioButtonNoiRedMed->isChecked())
			RadioButtonNoiRedMed->setChecked(true);
		break;

    case NR_HIGH:
		if (!RadioButtonNoiRedHigh->isChecked())
			RadioButtonNoiRedHigh->setChecked(true);
		break;

    case NR_SPEEX:
		if (!RadioButtonNoiRedSpeex->isChecked())
			RadioButtonNoiRedSpeex->setChecked(true);
		break;
	}

#ifdef HAVE_SPEEX
	/* Set speex spinbox enable state */
	SpinBoxNoiRedLevel->setEnabled(RadioButtonNoiRedSpeex->isChecked());
#endif

	/* Set filter bandwidth */
    SliderBandwidth->setValue(rx.GetAMFilterBW());
    TextLabelBandWidth->setText(QString().setNum(SliderBandwidth->value()) +	tr(" Hz"));

	/* Update check boxes */
    CheckBoxMuteAudio->setChecked(rx.isAudioMuted());
	CheckBoxSaveAudioWave->setChecked(rx.isWriteWaveFile());

    CheckBoxAutoFreqAcq->setChecked(rx.isAMAutoFrequencyAcquisitionEnabled());

    CheckBoxPLL->setChecked(rx.isAMPLLEnabled());
}

void AnalogDemDlg::UpdateSliderBandwidth()
{
    SliderBandwidth->setRange(0, rx.GetParameters()->GetSigSampleRate() / 2);
}

void AnalogDemDlg::UpdatePlotStyle(int iPlotstyle)
{
	/* Update main plot window */
	if(MainPlot)
		MainPlot->SetPlotStyle(iPlotstyle);
}

void AnalogDemDlg::OnSampleRateChanged(int)
{
	UpdateSliderBandwidth();
}

void AnalogDemDlg::OnSoundFileChanged(QString)
{
	UpdateSliderBandwidth();
}

void AnalogDemDlg::OnTimer()
{
    switch(rx.GetReceiverMode())
	{
	case RM_DRM:
		this->hide();
		break;
	case RM_FM:
		this->hide();
		break;
	case RM_AM:
		/* Carrier frequency of AM signal */
        ButtonFreqOffset->setText(QString().setNum(rx.ConvertFrequency(rx.GetAMMixerFrequencyOffset()), 'f', 2) + " Hz");
		break;
	case RM_NONE:
		break;
	}
}

void AnalogDemDlg::OnTimerPLLPhaseDial()
{
	CReal rCurPLLPhase;

    if (rx.GetAMPLLPhase(rCurPLLPhase))
	{
		/* Set current PLL phase (convert radiant in degree) */
		PhaseDial->setValue(rCurPLLPhase * (CReal) 360.0 / (2 * crPi));

		/* Check if control is enabled */
		if (!PhaseDial->isEnabled())
			PhaseDial->setEnabled(true);
	}
	else
	{
		/* Reset dial */
		PhaseDial->setValue((CReal) 0.0);

		/* Check if control is disabled */
		if (PhaseDial->isEnabled())
			PhaseDial->setEnabled(false);
	}
}

void AnalogDemDlg::OnTimerClose() // TODO
{
		close();
}

void AnalogDemDlg::OnRadioDemodulation(int iID)
{
	iID = -iID - 2; // TODO understand why
    /* rx takes care of setting appropriate filter BW */
	switch (iID)
	{
	case 0:
        rx.SetAMDemodulationType(DT_AM);
		break;

	case 1:
        rx.SetAMDemodulationType(DT_LSB);
		break;

	case 2:
        rx.SetAMDemodulationType(DT_USB);
		break;

	case 3:
        rx.SetAMDemodulationType(DT_CW);
		break;

	case 4:
        rx.SetAMDemodulationType(DT_FM);
		break;
	}

	/* Update controls */
	UpdateControls();
}

void AnalogDemDlg::OnRadioAGC(int iID)
{
	iID = -iID - 2; // TODO understand why
	switch (iID)
	{
	case 0:
        rx.SetAMAGCType(AT_NO_AGC);
		break;

	case 1:
        rx.SetAMAGCType(AT_SLOW);
		break;

	case 2:
        rx.SetAMAGCType(AT_MEDIUM);
		break;

	case 3:
        rx.SetAMAGCType(AT_FAST);
		break;
	}
}

void AnalogDemDlg::OnRadioNoiRed(int iID)
{
	iID = -iID - 2; // TODO understand why
	switch (iID)
	{
	case 0:
        rx.SetAMNoiseReductionType(NR_OFF);
		break;

	case 1:
        rx.SetAMNoiseReductionType(NR_LOW);
		break;

	case 2:
        rx.SetAMNoiseReductionType(NR_MEDIUM);
		break;

	case 3:
        rx.SetAMNoiseReductionType(NR_HIGH);
		break;

#ifdef HAVE_SPEEX
	case 4:
        rx.SetAMNoiseReductionType(NR_SPEEX);
		break;
#endif
	}

#ifdef HAVE_SPEEX
	/* Set speex spinbox enable state */
	SpinBoxNoiRedLevel->setEnabled(RadioButtonNoiRedSpeex->isChecked());
#endif
}

void AnalogDemDlg::OnSliderBWChange(int value)
{
	/* Set new filter in processing module */
    rx.SetAMFilterBW(value);
	TextLabelBandWidth->setText(QString().setNum(value) + tr(" Hz"));

	/* Update chart */
	if(MainPlot) MainPlot->UpdateAnalogBWMarker();
}

void AnalogDemDlg::OnCheckAutoFreqAcq()
{
	/* Set parameter in working thread module */
    rx.EnableAutoFrequenctAcquisition(CheckBoxAutoFreqAcq->isChecked());
}

void AnalogDemDlg::OnCheckPLL()
{
	/* Set parameter in working thread module */
    rx.EnableAMPLL(CheckBoxPLL->isChecked());
}

void AnalogDemDlg::OnCheckBoxMuteAudio()
{
    rx.MuteAudio(CheckBoxMuteAudio->isChecked());
}

void AnalogDemDlg::OnCheckSaveAudioWAV()
{
/*
	This code is copied in systemevalDlg.cpp. If you do changes here, you should
	apply the changes in the other file, too
*/
	if (CheckBoxSaveAudioWave->isChecked())
	{
		/* Show "save file" dialog */
		QString strFileName =
			QFileDialog::getSaveFileName(this, tr("Save Audio"), "DreamOut.wav", tr("Wav (*.wav)"));
		/* Check if user not hit the cancel button */
		if (!strFileName.isEmpty())
		{
            rx.StartWriteWaveFile(strFileName.toLocal8Bit().constData());
		}
		else
		{
			/* User hit the cancel button, uncheck the button */
			CheckBoxSaveAudioWave->setChecked(false);
		}
	}
	else
        rx.StopWriteWaveFile();
}

void AnalogDemDlg::OnChartxAxisValSet(double dVal)
{
	/* Perform range check */
	if (dVal < 0.0)
		dVal = 0.0;
	else if (dVal > 1.0)
		dVal = 1.0;

	/* Set new frequency in receiver module */
    rx.SetAMDemodAcq(dVal);

	/* Update chart */
	if(MainPlot) MainPlot->UpdateAnalogBWMarker();
}

void AnalogDemDlg::OnButtonWaterfall()
{
	/* Toggle between normal spectrum plot and waterfall spectrum plot */
	if (MainPlot && ButtonWaterfall->isChecked())
		MainPlot->SetupChart(CDRMPlot::INP_SPEC_WATERF);
	else
		MainPlot->SetupChart(CDRMPlot::INPUT_SIG_PSD_ANALOG);
}

/* Manual carrier frequency input box */
void AnalogDemDlg::on_ButtonFreqOffset_clicked(bool)
{
	bool ok = false;
    const double prev_freq = rx.ConvertFrequency(rx.GetAMMixerFrequencyOffset());
	const double new_freq = QInputDialog::getDouble(this, this->windowTitle(),
		LabelFreqOffset->text(), prev_freq, -1e6, 1e6, 2, &ok);
	if (ok)
	{
        const _REAL conv_freq =rx.ConvertFrequency(new_freq, true);
        const double dVal = conv_freq / (rx.GetParameters()->GetSigSampleRate() / 2);
		OnChartxAxisValSet(dVal);
	}
}

void AnalogDemDlg::on_SpinBoxNoiRedLevel_valueChanged(int value)
{
    rx.SetAMNoiseReductionLevel(value);
}

void AnalogDemDlg::AddWhatsThisHelp()
{
	/* Noise Reduction */
	const QString strNoiseReduction =
		tr("<b>Noise Reduction:</b> The noise suppression is a frequency "
		"domain optimal filter design based algorithm. The noise PSD is "
		"estimated utilizing a minimum statistic. A problem of this type of "
		"algorithm is that it produces the so called \"musical tones\". The "
		"noise becomes coloured and sounds a bit strange. At the same time, "
		"the useful signal (which might be speech or music) is also "
		"distorted by the algorithm. By selecting the level of noise "
		"reduction, a compromise between distortion of the useful signal "
		"and actual noise reduction can be made.");

	/* Automatic Gain Control */
	const QString strAGC =
		tr("<b>AGC (Automatic Gain Control):</b> Input signals can have a "
		"large variation in power due to channel impairments. To compensate "
		"for that, an automatic gain control can be applied. The AGC has "
		"four settings: Off, Slow, Medium and Fast.");

	/* Filter Bandwidth */
	const QString strFilterBW =
		tr("<b>Filter Bandwidth:</b> A band-pass filter is applied before "
		"the actual demodulation process. With this filter, adjacent signals "
		"are attenuated. The bandwidth of this filter can be chosen in steps "
		"of 1 Hz by using the slider bar. Clicking on the right or left side "
		"of the slider leveler will increase/decrease the bandwidth by 1 kHz. "
		"<br>The current filter bandwidth is indicated in the spectrum plot "
		"by a selection bar.");

	/* Demodulation type */
	const QString strDemodType =
		tr("<b>Demodulation Type:</b> The following analog "
		"demodulation types are available:<ul>"
		"<li><b>AM:</b> This analog demodulation type is used in most of "
		"the hardware radios. The envelope of the complex base-band signal "
		"is used followed by a high-pass filter to remove the DC offset. "
		"Additionally, a low pass filter with the same bandwidth as the "
		"pass-band filter is applied to reduce the noise caused by "
		"non-linear distortions.</li>"
		"<li><b>LSB / USB:</b> These are single-side-band (SSB) demodulation "
		"types. Only one side of the spectrum is evaluated, the upper side "
		"band is used in USB and the lower side band with LSB. It is "
		"important for SSB demodulation that the DC frequency of the analog "
		"signal is known to get satisfactory results. The DC frequency is "
		"automatically estimated by starting a new acquisition or by "
		"clicking on the plot.</li>"
		"<li><b>CW:</b> This demodulation type can be used to receive "
		"CW signals. Only a narrow frequency band in a fixed distance "
		"to the mixing frequency is used. By clicking on the spectrum "
		"plot, the center position of the band pass filter can be set.</li>"
		"<li><b>FM:</b> This is a narrow band frequency demodulation.</li>"
		"</ul>");

	/* Mute Audio (same as in systemevaldlg.cpp!) */
	QString strCheckBoxMuteAudio =
		tr("<b>Mute Audio:</b> The audio can be muted by "
		"checking this box. The reaction of checking or unchecking this box "
		"is delayed by approx. 1 second due to the audio buffers.");

	/* Save audio as wave (same as in systemevaldlg.cpp!) */
	QString strCheckBoxSaveAudioWave =
		tr("<b>Save Audio as WAV:</b> Save the audio signal "
		"as stereo, 16-bit, 48 kHz sample rate PCM wave file. Checking this "
		"box will let the user choose a file name for the recording.");

	/* Carrier Frequency */
	QString strTextFreqOffset =
		tr("<b>Carrier Frequency:</b> The (estimated) carrier frequency of the "
		"analog signal is shown. (The estimation of this parameter can be done "
		"by the Autom Frequency Acquisition which uses the estimated PSD of "
		"the input signal and applies a maximum search.)");

	/* Phase lock loop */
	const QString strPLL =
		tr("<b>PLL:</b> The Phase-Lock-Loop (PLL) tracks the carrier of the "
		"modulated received signal. The resulting phase offset between the "
		"reference oscillator and the received carrier is displayed in "
		"a dial control. If the pointer is almost steady, the PLL is locked. "
		"If the pointer of the dial control turns quickly, the PLL is "
		"out of lock. To get the PLL locked, the frequency offset to "
		"the true carrier frequency must not exceed a few Hz.");

	/* Auto frequency acquisition */
	const QString strAutoFreqAcqu =
		tr("<b>Auto Frequency Acquisition:</b> Clicking on the "
		"input spectrum plot changes the mixing frequency for demodulation. "
		"If the Auto Frequency Acquisition is enabled, the largest peak "
		"near the curser is selected.");

    RadioButtonNoiRedOff->setWhatsThis(strNoiseReduction);
    RadioButtonNoiRedLow->setWhatsThis(strNoiseReduction);
    RadioButtonNoiRedMed->setWhatsThis(strNoiseReduction);
    RadioButtonNoiRedHigh->setWhatsThis(strNoiseReduction);
    RadioButtonAGCOff->setWhatsThis(strAGC);
    RadioButtonAGCSlow->setWhatsThis(strAGC);
    RadioButtonAGCMed->setWhatsThis(strAGC);
    RadioButtonAGCFast->setWhatsThis(strAGC);
    TextLabelBandWidth->setWhatsThis(strFilterBW);
    SliderBandwidth->setWhatsThis(strFilterBW);
    RadioButtonDemAM->setWhatsThis(strDemodType);
    RadioButtonDemLSB->setWhatsThis(strDemodType);
    RadioButtonDemUSB->setWhatsThis(strDemodType);
    RadioButtonDemCW->setWhatsThis(strDemodType);
    RadioButtonDemFM->setWhatsThis(strDemodType);
    GroupBoxAutoFreqAcq->setWhatsThis(strAutoFreqAcqu);
    CheckBoxAutoFreqAcq->setWhatsThis(strAutoFreqAcqu);
    CheckBoxMuteAudio->setWhatsThis(strCheckBoxMuteAudio);
    GroupBoxPLL->setWhatsThis(strPLL);
    CheckBoxPLL->setWhatsThis(strPLL);
    PhaseDial->setWhatsThis(strPLL);
    TextLabelPhaseOffset->setWhatsThis(strPLL);
    LabelFreqOffset->setWhatsThis(strTextFreqOffset);
    ButtonFreqOffset->setWhatsThis(strTextFreqOffset);
    CheckBoxSaveAudioWave->setWhatsThis(strCheckBoxSaveAudioWave);
    groupBoxNoiseReduction->setWhatsThis(strNoiseReduction);
    groupBoxAGC->setWhatsThis(strAGC);
    groupBoxDemodulation->setWhatsThis(strDemodType);
    groupBoxBW->setWhatsThis(strFilterBW);
}


/******************************************************************************\
* AMSS controls                                                                *
\******************************************************************************/
/*
	Added by Andrew Murphy, BBC Research & Development, 2005

	Additional widgets have been added to display the AMSS service label,
	language etc. in in a similar style to that used for DRM reception.
	A display has also been added to show the status of the AMSS decoding.
	Everytime an AMSS CRC passes (for block or block 2) the 47 decoded
	bits are displayed. Note this could also include 'false' passes.

	The percentage of the current data entity group or 'SDC' is displayed
	along with which parts of the data entity group have been decoded. A
	'#' indicates that a data entity gruop segment is yet to be received
	whilst a 'c' or 'C' indicates a CRC pass for the block 2 carrying that
	particular segment.

	Added phase offset display for AMSS demodulation loop.
*/
CAMSSDlg::CAMSSDlg(CRx& NDRMR, CSettings& Settings, QWidget* parent) :
	CWindow(parent, Settings, "AMSS"),
    rx(NDRMR)
{
	setupUi(this);

#if QWT_VERSION >= 0x060100
	/* Workaround for PhaseDialAMSS receiving focus 
	even if its FocusPolicy is set to Qt::NoFocus,
	might not be needed in final release of Qwt 6.1.0 */
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
#endif

	/* Set help text for the controls */
	AddWhatsThisHelp();

	/* Init AMSS PLL phase dial control */
	PhaseDialAMSS->setMode(QwtDial::RotateNeedle);
	PhaseDialAMSS->setWrapping(true);
	PhaseDialAMSS->setReadOnly(true);
#if QWT_VERSION < 0x060100
	PhaseDialAMSS->setScale(0, 360, 45); /* Degrees */
#else
	PhaseDialAMSS->setScale(0, 360); /* Degrees */
	PhaseDialAMSS->setScaleStepSize(45);
#endif
	PhaseDialAMSS->setOrigin(270);
	PhaseDialAMSS->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow));
	PhaseDialAMSS->setFrameShadow(QwtDial::Plain);
#if QWT_VERSION < 0x060000
	PhaseDialAMSS->setScaleOptions(QwtDial::ScaleTicks);
#elif QWT_VERSION < 0x060100
	PhaseDialAMSS->setScaleComponents(QwtAbstractScaleDraw::Ticks);
#else
	PhaseDialAMSS->setFrameShadow(QwtDial::Sunken);
	PhaseDialAMSS->setScaleMaxMinor(0);
	PhaseDialAMSS->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Labels, false);
#endif

	TextAMSSServiceLabel->setText("");
	TextAMSSCountryCode->setText("");
	TextAMSSTimeDate->setText("");
	TextAMSSLanguage->setText("");
	TextAMSSServiceID->setText("");
	TextAMSSAMCarrierMode->setText("");
	TextAMSSInfo->setText("");

	ListBoxAMSSAFSList->setEnabled(false);


	/* Connect controls ----------------------------------------------------- */
	/* Timers */
	connect(&Timer, SIGNAL(timeout()),
		this, SLOT(OnTimer()));
	connect(&TimerPLLPhaseDial, SIGNAL(timeout()),
		this, SLOT(OnTimerPLLPhaseDial()));
}

void CAMSSDlg::eventHide(QHideEvent*)
{
	/* stop real-time timers */
	Timer.stop();
	TimerPLLPhaseDial.stop();
}

void CAMSSDlg::eventShow(QShowEvent*)
{
	OnTimer();
	OnTimerPLLPhaseDial();

	/* Activate real-time timers */
	Timer.start(GUI_CONTROL_UPDATE_TIME);
	TimerPLLPhaseDial.start(PLL_PHASE_DIAL_UPDATE_TIME);
}

void CAMSSDlg::OnTimer()
{
	int j;

    CParameter& Parameters = *rx.GetParameters();
	Parameters.Lock();

	/* Show label if available */
	if ((Parameters.Service[0].IsActive()) && (Parameters.Service[0].strLabel != ""))
	{
		/* Service label (UTF-8 encoded string -> convert) */
		TextAMSSServiceLabel->setText(QString().fromUtf8(
			Parameters.Service[0].strLabel.c_str()
		));
	}
	else
		TextAMSSServiceLabel->setText(tr(""));

	/* Country code */
	const string strCntryCode = Parameters.Service[0].strCountryCode; /* must be of 2 lowercase chars */

	if ((Parameters.Service[0].IsActive()) && (!strCntryCode.empty()) && (strCntryCode != "--"))
	{
		TextAMSSCountryCode->
			setText(QString(GetISOCountryName(strCntryCode).c_str()));
	}
	else
		TextAMSSCountryCode->setText("");

	/* SDC Language code */

	if (Parameters.Service[0].IsActive())
	{
		const string strLangCode = Parameters.Service[0].strLanguageCode; /* must be of 3 lowercase chars */

		if ((!strLangCode.empty()) && (strLangCode != "---"))
			 TextAMSSLanguage->
				setText(QString(GetISOLanguageName(strLangCode).c_str()));
		else
			TextAMSSLanguage->setText(QString(strTableLanguageCode[Parameters.Service[0].iLanguage].c_str()));
	}
	else
		TextAMSSLanguage->setText("");

	/* Time, date */
	if ((Parameters.iUTCHour == 0) &&
		(Parameters.iUTCMin == 0) &&
		(Parameters.iDay == 0) &&
		(Parameters.iMonth == 0) &&
		(Parameters.iYear == 0))
	{
		/* No time service available */
		TextAMSSTimeDate->setText("");
	}
	else
	{
		QDateTime DateTime;
		DateTime.setDate(QDate(Parameters.iYear, Parameters.iMonth, Parameters.iDay));
		DateTime.setTime(QTime(Parameters.iUTCHour, Parameters.iUTCMin));

		TextAMSSTimeDate->setText(DateTime.toString());
	}

	/* Get number of alternative services */
	const size_t iNumAltServices = Parameters.AltFreqSign.vecOtherServices.size();

	if (iNumAltServices != 0)
	{
		QString val = QString().setNum((long) iNumAltServices);
		ListBoxAMSSAFSList->insertItem(10, val);

		ListBoxAMSSAFSList->clear();
		ListBoxAMSSAFSList->setEnabled(true);

		QString freqEntry;

		for (size_t i = 0; i < iNumAltServices; i++)
		{
			switch (Parameters.AltFreqSign.vecOtherServices[i].iSystemID)
			{
			case 0:
				freqEntry = "DRM:";
				break;

			case 1:
			case 2:
				freqEntry = "AM:   ";
				break;

			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				freqEntry = "FM:   ";
				break;

			default:
				freqEntry = "";
				break;
			}

			const int iNumAltFreqs = Parameters.AltFreqSign.vecOtherServices[i].veciFrequencies.size();

			const int iSystemID = Parameters.AltFreqSign.vecOtherServices[i].iSystemID;

			switch (iSystemID)
			{
			case 0:
			case 1:
			case 2:
				/* AM or DRM, freq in kHz */
				for (j = 0; j < iNumAltFreqs; j++)
				{
					freqEntry +=
						QString().setNum((long) Parameters.
						AltFreqSign.vecOtherServices[i].
						veciFrequencies[j], 10);

					if (j != iNumAltFreqs-1)
						freqEntry += ",";
				}
				freqEntry += " kHz";


				if (iSystemID == 0 || iSystemID == 1)
				{
					freqEntry += " ID:";
					freqEntry +=
						QString().setNum((long) Parameters.
						AltFreqSign.vecOtherServices[i].
						iServiceID, 16).toUpper();
				}
				break;

			case 3:
			case 4:
			case 5:
				/* 'FM1 frequency' - 87.5 to 107.9 MHz (100 kHz steps) */
				for (j = 0; j < iNumAltFreqs; j++)
				{
					freqEntry +=
                        QString().setNum((float) (87.5 + 0.1 * rx.
						GetParameters()->AltFreqSign.
						vecOtherServices[i].veciFrequencies[j]), 'f', 1);

					if (j != iNumAltFreqs-1)
						freqEntry += ",";
				}
				freqEntry += " MHz";

				if (iSystemID == 3)
				{
					freqEntry += " ECC+PI:";
					freqEntry +=
						QString().setNum((long) Parameters.
						AltFreqSign.vecOtherServices[i].
						iServiceID, 16).toUpper();
				}

				if (iSystemID == 4)
				{
					freqEntry += " PI:";
					freqEntry +=
						QString().setNum((long) Parameters.
						AltFreqSign.vecOtherServices[i].
						iServiceID, 16).toUpper();
				}
				break;

			case 6:
			case 7:
			case 8:
				/* 'FM2 frequency'- 76.0 to 90.0 MHz (100 kHz steps) */
				for (j = 0; j < iNumAltFreqs; j++)
				{
					freqEntry +=
                        QString().setNum((float) (76.0 + 0.1 * rx.
						GetParameters()->AltFreqSign.
						vecOtherServices[i].veciFrequencies[j]), 'f', 1);

					if (j != iNumAltFreqs-1)
						freqEntry += ",";
				}
				freqEntry += " MHz";

				if (iSystemID == 6)
				{
					freqEntry += " ECC+PI:";
					freqEntry +=
						QString().setNum((long) Parameters.
						AltFreqSign.vecOtherServices[i].
						iServiceID, 16).toUpper();
				}

				if (iSystemID == 7)
				{
					freqEntry += " PI:";
					freqEntry +=
						QString().setNum((long) Parameters.
						AltFreqSign.vecOtherServices[i].
						iServiceID, 16).toUpper();
				}
				break;

			default:
				freqEntry = "DAB";
				break;
			}

			if (Parameters.AltFreqSign.
				vecOtherServices[i].bSameService)
			{
				freqEntry += " (same service)";
			}
			else
			{
				freqEntry += " (alt service)";
			}
			ListBoxAMSSAFSList->insertItem(0, freqEntry);
		}
	}
	else
	{
		ListBoxAMSSAFSList->clear();
		ListBoxAMSSAFSList->setEnabled(false);
	}

	TextAMSSServiceID->setText("");
	TextAMSSAMCarrierMode->setText("");

    if (rx.GetAMSSLockStatus() == NO_SYNC
	|| Parameters.Service[0].iServiceID == SERV_ID_NOT_USED
	)
	{
		TextAMSSInfo->setText(tr("No AMSS detected"));
	}
	else
	{
		TextAMSSInfo->setText(tr("Awaiting AMSS data..."));

		/* Display 'block 1' info */
        if (rx.GetAMSSBlock1Status())
		{
			TextAMSSInfo->setText("");

            TextAMSSLanguage->setText(QString(strTableLanguageCode[rx.
				GetParameters()->Service[0].iLanguage].c_str()));

			TextAMSSServiceID->setText("ID:" + QString().setNum(
				(long) Parameters.Service[0].iServiceID, 16).toUpper());

            TextAMSSAMCarrierMode->setText(QString(strTableAMSSCarrierMode[rx.
				GetParameters()->iAMSSCarrierMode].c_str()));
		}
	}

    TextDataEntityGroupStatus->setText(rx.GetAMSSDataEntityGroupStatus());

    TextCurrentBlock->setText(QString().setNum(rx.GetAMSSCurrentBlock(), 10));

    TextBlockBits->setText(rx.GetAMSSCurrentBlockBits());

    int val = rx.GetAMSSPercentageDataEntityGroupComplete();
	Parameters.Unlock();
	ProgressBarAMSS->setValue(val);
}

void CAMSSDlg::OnTimerPLLPhaseDial()
{
	CReal rCurPLLPhase;

    if (rx.GetAMSSPLLPhase(rCurPLLPhase))
	{
		/* Set current PLL phase (convert radiant in degree) */
		PhaseDialAMSS->setValue(rCurPLLPhase * (CReal) 360.0 / (2 * crPi));

		/* Check if control is enabled */
		if (!PhaseDialAMSS->isEnabled())
			PhaseDialAMSS->setEnabled(true);
	}
	else
	{
		/* Reset dial */
		PhaseDialAMSS->setValue((CReal) 0.0);

		/* Check if control is disabled */
		if (PhaseDialAMSS->isEnabled())
			PhaseDialAMSS->setEnabled(false);
	}
}

void CAMSSDlg::AddWhatsThisHelp()
{
	// TODO
}
