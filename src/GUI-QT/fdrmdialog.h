/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer, Andrea Russo
 *
 * Description:
 *
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

#ifndef _FDRMDIALOG_H_
#define _FDRMDIALOG_H_

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QString>
#include <QMenuBar>
#include <QLayout>
#include <QPalette>
#include <QColorDialog>
#include <QActionGroup>
#include <QSignalMapper>
#include <QDialog>
#include <QMenu>
#include <QShowEvent>
#include <QHideEvent>
#include <QCloseEvent>
#include <qwt_thermo.h>
#include "ui_DRMMainWindow.h"

#include "CWindow.h"
#include "EvaluationDlg.h"
#include "SoundCardSelMenu.h"
#include "DialogUtil.h"
#include "StationsDlg.h"
#include "LiveScheduleDlg.h"
#include "EPGDlg.h"
#include "fmdialog.h"
#include "AnalogDemDlg.h"
#include "MultSettingsDlg.h"
#include "GeneralSettingsDlg.h"
#include "MultColorLED.h"
#include "Logging.h"
#include "../main-Qt/crx.h"
#include "../DrmReceiver.h"
#include "../util/Vector.h"
#include "../datadecoding/DataDecoder.h"

/* Classes ********************************************************************/
class BWSViewer;
class JLViewer;
class SlideShowViewer;
class CScheduler;

class FDRMDialog : public CWindow, public Ui_DRMMainWindow
{
    Q_OBJECT

public:
    FDRMDialog(CTRx*, CSettings&,
#ifdef HAVE_LIBHAMLIB
               CRig&,
#endif
               QWidget* parent = 0);
    virtual ~FDRMDialog();

protected:
    CRx&                rx;
    bool                done;
    QTimer				Timer;
    std::vector<QLabel*>		serviceLabels;

    CLogging*			pLogging;
    systemevalDlg*		pSysEvalDlg;
    BWSViewer*			pBWSDlg;
    JLViewer*			pJLDlg;
    SlideShowViewer*	pSlideShowDlg;
    MultSettingsDlg*	pMultSettingsDlg;
    StationsDlg*		pStationsDlg;
    LiveScheduleDlg*	pLiveScheduleDlg;
    EPGDlg*				pEPGDlg;
    AnalogDemDlg*		pAnalogDemDlg;
    FMDialog*			pFMDlg;
    GeneralSettingsDlg* pGeneralSettingsDlg;
    QMenuBar*			pMenu;
    QButtonGroup*		pButtonGroup;
    QMenu*				pReceiverModeMenu;
    QMenu*				pSettingsMenu;
    QMenu*				pPlotStyleMenu;
    CSysTray*           pSysTray;
    CWindow*            pCurrentWindow;
    CFileMenu*			pFileMenu;
    CSoundCardSelMenu*	pSoundCardMenu;
    CAboutDlg		    AboutDlg;
    int			        iMultimediaServiceBit;
    int			        iLastMultimediaServiceSelected;
    QString             SysTrayTitle;
    QString             SysTrayMessage;
    QTimer				TimerSysTray;
    CScheduler* 	    pScheduler;
    QTimer*		        pScheduleTimer;

    void SetStatus(CMultColorLED* LED, ETypeRxStatus state);
    virtual void        eventClose(QCloseEvent* ce);
    virtual void        eventHide(QHideEvent* pEvent);
    virtual void        eventShow(QShowEvent* pEvent);
    virtual void        eventUpdate();
    void		AddWhatsThisHelp();
    void		UpdateDRM_GUI();
    void		UpdateDisplay();
    void		ClearDisplay();
    void		UpdateWindowTitle(QString);

    void		SetDisplayColor(const QColor newColor);

    QString	GetCodecString(const CService&);
    QString	GetTypeString(const CService&);
    QString serviceSelector(CParameter&, int);
    void showTextMessage(const QString&);
    void showServiceInfo(const CService&);
    void startLogging();
    void stopLogging();
    void SysTrayCreate();
    void SysTrayStart();
    void SysTrayStop(const QString&);
    void SysTrayToolTip(const QString&, const QString&);
	void setBars(int);

public slots:
    void OnTimer();
    void OnScheduleTimer();
    void OnSysTrayTimer();
    void OnSelectAudioService(int);
    void OnSelectDataService(int);
    void OnViewMultimediaDlg();
    void OnMenuSetDisplayColor();
    void OnNewAcquisition();
    void OnSwitchMode(int);
    void OnSwitchToFM();
    void OnSwitchToAM();
    void OnHelpAbout() {AboutDlg.show();}
    void OnSoundFileChanged(QString);
    void OnWhatsThis();
    void OnSysTrayActivated(QSystemTrayIcon::ActivationReason);
    void OnWorkingThreadFinished();
    void ChangeGUIModeToDRM();
    void ChangeGUIModeToAM();
    void ChangeGUIModeToFM();
signals:
    void plotStyleChanged(int);
};

#endif // _FDRMDIALOG_H_
