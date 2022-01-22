/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2005
 *
 * Author(s):
 *	Volker Fischer, Andrew Murphy
 *
 * Description:
 *	See AnalogDemDlg.cpp
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


#include "ui_AMMainWindow.h"
#include "ui_AMSSDlgbase.h"
#include "CWindow.h"
#include "SoundCardSelMenu.h"
#include "DRMPlot.h"
#include "../GlobalDefinitions.h"
#include "../main-Qt/crx.h"
#include "../tables/TableAMSS.h"
#include <QTimer>
#include <QDialog>
#include <QButtonGroup>


/* Definitions ****************************************************************/
/* Update time of PLL phase dial control */
#define PLL_PHASE_DIAL_UPDATE_TIME				100


/* Classes ********************************************************************/


/* AMSS dialog -------------------------------------------------------------- */
class CAMSSDlg : public CWindow, public Ui_CAMSSDlgBase
{
	Q_OBJECT

public:
    CAMSSDlg(CRx&, CSettings&, QWidget* parent = 0);

protected:
    CRx&            rx;

	QTimer			Timer;
	QTimer			TimerPLLPhaseDial;
	void			AddWhatsThisHelp();
	virtual void	eventShow(QShowEvent*);
	virtual void	eventHide(QHideEvent*);

public slots:
	void OnTimer();
	void OnTimerPLLPhaseDial();
};


/* Analog demodulation dialog ----------------------------------------------- */
class AnalogDemDlg : public CWindow, public Ui_AMMainWindow
{
	Q_OBJECT

public:
    AnalogDemDlg(CRx&, CSettings&, CFileMenu*, CSoundCardSelMenu*,
	QWidget* parent = 0);

protected:
    CRx&        		rx;

	QTimer				Timer;
	QTimer				TimerPLLPhaseDial;
	QTimer				TimerClose;
	CAMSSDlg			AMSSDlg;
    CDRMPlot*			MainPlot;
	CFileMenu*			pFileMenu;
	CSoundCardSelMenu*	pSoundCardMenu;

	void UpdateControls();
	void UpdateSliderBandwidth();
	void AddWhatsThisHelp();
	virtual void eventClose(QCloseEvent* pEvent);
	virtual void eventShow(QShowEvent* pEvent);
	virtual void eventHide(QHideEvent* pEvent);
	virtual void eventUpdate();

public slots:
	void UpdatePlotStyle(int);
    void OnSampleRateChanged(int);
    void OnSoundFileChanged(QString);
	void OnTimer();
	void OnTimerPLLPhaseDial();
	void OnTimerClose();
	void OnRadioDemodulation(int iID);
	void OnRadioAGC(int iID);
	void OnCheckBoxMuteAudio();
	void OnCheckSaveAudioWAV();
	void OnCheckAutoFreqAcq();
	void OnCheckPLL();
	void OnChartxAxisValSet(double dVal);
	void OnSliderBWChange(int value);
	void OnRadioNoiRed(int iID);
	void OnButtonWaterfall();
	void on_ButtonFreqOffset_clicked(bool);
	void on_SpinBoxNoiRedLevel_valueChanged(int value);
	void OnSwitchToDRM();
	void OnSwitchToFM();
	void OnHelpAbout() {emit About();}
	void OnWhatsThis();

signals:
	void SwitchMode(int);
	void NewAMAcquisition();
	void ViewStationsDlg();
	void ViewLiveScheduleDlg();
	void Closed();
	void About();
};
