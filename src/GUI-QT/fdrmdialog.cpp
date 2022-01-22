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

#include "fdrmdialog.h"
#include <iostream>
#include <QFileInfo>
#include <QWhatsThis>
#include <QHideEvent>
#include <QEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include "SlideShowViewer.h"
#include "JLViewer.h"
#ifdef QT_WEBENGINE_LIB
# include "BWSViewer.h"
#endif
#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
#endif
#include "../Scheduler.h"
#include "../util-QT/Util.h"

// Simone's values
// static _REAL WMERSteps[] = {8.0, 12.0, 16.0, 20.0, 24.0};
// David's values
static _REAL WMERSteps[] = {6.0, 12.0, 18.0, 24.0, 30.0};

/* Implementation *************************************************************/
#ifdef HAVE_LIBHAMLIB
FDRMDialog::FDRMDialog(CTRx* pRx, CSettings& Settings, CRig& rig, QWidget* parent)
#else
FDRMDialog::FDRMDialog(CTRx* pRx, CSettings& Settings, QWidget* parent)
#endif
    :
    CWindow(parent, Settings, "DRM"),
    rx(*reinterpret_cast<CRx*>(pRx)),
    done(false),
    serviceLabels(4), pLogging(nullptr),
    pSysTray(nullptr), pCurrentWindow(this),
    iMultimediaServiceBit(0),
    iLastMultimediaServiceSelected(-1),
    pScheduler(nullptr), pScheduleTimer(nullptr)
{
    setupUi(this);

    /* Set help text for the controls */
    AddWhatsThisHelp();

    CParameter& Parameters = *rx.GetParameters();

    pLogging = new CLogging(Parameters);
    pLogging->LoadSettings(Settings);

    /* Creation of file and sound card menu */
    pFileMenu = new CFileMenu(rx, this, menu_View);
    pSoundCardMenu = new CSoundCardSelMenu(rx, pFileMenu, this);
    menu_Settings->addMenu(pSoundCardMenu);
    connect(&rx, SIGNAL(soundFileChanged(QString)), this, SLOT(OnSoundFileChanged(QString)));
    connect(&rx, SIGNAL(finished()), this, SLOT(OnWorkingThreadFinished()));

    /* Analog demodulation window */
    pAnalogDemDlg = new AnalogDemDlg(rx, Settings, pFileMenu, pSoundCardMenu);

    /* FM window */
    pFMDlg = new FMDialog(rx, Settings, pFileMenu, pSoundCardMenu);

    /* Parent list for Stations and Live Schedule window */
	QMap <QWidget*,QString> parents;
	parents[this] = "drm";
	parents[pAnalogDemDlg] = "analog";

    /* Stations window */
#ifdef HAVE_LIBHAMLIB
    pStationsDlg = new StationsDlg(rx, Settings, rig, parents);
#else
    pStationsDlg = new StationsDlg(rx, Settings, parents);
#endif

    /* Live Schedule window */
    pLiveScheduleDlg = new LiveScheduleDlg(rx, Settings, parents);

    /* MOT broadcast website viewer window */
#ifdef QT_WEBENGINE_LIB
    pBWSDlg = new BWSViewer(rx, Settings, this);
#endif

    /* Journaline viewer window */
    pJLDlg = new JLViewer(rx, Settings, this);

    /* MOT slide show window */
    pSlideShowDlg = new SlideShowViewer(rx, Settings, this);

    /* Programme Guide Window */
    pEPGDlg = new EPGDlg(rx, Settings, this);

    /* Evaluation window */
    pSysEvalDlg = new systemevalDlg(rx, Settings, this);

    /* general settings window */
    pGeneralSettingsDlg = new GeneralSettingsDlg(Parameters, Settings, this);

    /* Multimedia settings window */
    pMultSettingsDlg = new MultSettingsDlg(Parameters, Settings, this);

    connect(action_Evaluation_Dialog, SIGNAL(triggered()), pSysEvalDlg, SLOT(show()));
    connect(action_Multimedia_Dialog, SIGNAL(triggered()), this, SLOT(OnViewMultimediaDlg()));
    connect(action_Stations_Dialog, SIGNAL(triggered()), pStationsDlg, SLOT(show()));
    connect(action_Live_Schedule_Dialog, SIGNAL(triggered()), pLiveScheduleDlg, SLOT(show()));
    connect(action_Programme_Guide_Dialog, SIGNAL(triggered()), pEPGDlg, SLOT(show()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

    action_Multimedia_Dialog->setEnabled(false);

    connect(actionMultimediaSettings, SIGNAL(triggered()), pMultSettingsDlg, SLOT(show()));
    connect(actionGeneralSettings, SIGNAL(triggered()), pGeneralSettingsDlg, SLOT(show()));

    connect(actionAM, SIGNAL(triggered()), this, SLOT(OnSwitchToAM()));
    connect(actionFM, SIGNAL(triggered()), this, SLOT(OnSwitchToFM()));
    connect(actionDRM, SIGNAL(triggered()), this, SLOT(OnNewAcquisition()));

    connect(actionDisplayColor, SIGNAL(triggered()), this, SLOT(OnMenuSetDisplayColor()));

    /* Plot style settings */
    QSignalMapper* plotStyleMapper = new QSignalMapper(this);
    QActionGroup* plotStyleGroup = new QActionGroup(this);
    plotStyleGroup->addAction(actionBlueWhite);
    plotStyleGroup->addAction(actionGreenBlack);
    plotStyleGroup->addAction(actionBlackGrey);
    plotStyleMapper->setMapping(actionBlueWhite, 0);
    plotStyleMapper->setMapping(actionGreenBlack, 1);
    plotStyleMapper->setMapping(actionBlackGrey, 2);
    connect(actionBlueWhite, SIGNAL(triggered()), plotStyleMapper, SLOT(map()));
    connect(actionGreenBlack, SIGNAL(triggered()), plotStyleMapper, SLOT(map()));
    connect(actionBlackGrey, SIGNAL(triggered()), plotStyleMapper, SLOT(map()));
    connect(plotStyleMapper, SIGNAL(mapped(int)), this, SIGNAL(plotStyleChanged(int)));
    switch(getSetting("plotstyle", int(0), true))
    {
    case 0:
        actionBlueWhite->setChecked(true);
        break;
    case 1:
        actionGreenBlack->setChecked(true);
        break;
    case 2:
        actionBlackGrey->setChecked(true);
        break;
    }

    connect(actionAbout_Dream, SIGNAL(triggered()), this, SLOT(OnHelpAbout()));
    connect(actionWhats_This, SIGNAL(triggered()), this, SLOT(OnWhatsThis()));

    connect(this, SIGNAL(plotStyleChanged(int)), pSysEvalDlg, SLOT(UpdatePlotStyle(int)));
    connect(this, SIGNAL(plotStyleChanged(int)), pAnalogDemDlg, SLOT(UpdatePlotStyle(int)));

    pButtonGroup = new QButtonGroup(this);
    pButtonGroup->setExclusive(true);
    pButtonGroup->addButton(PushButtonService1, 0);
    pButtonGroup->addButton(PushButtonService2, 1);
    pButtonGroup->addButton(PushButtonService3, 2);
    pButtonGroup->addButton(PushButtonService4, 3);
    connect(pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(OnSelectAudioService(int)));
    connect(pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(OnSelectDataService(int)));

    connect(pFMDlg, SIGNAL(About()), this, SLOT(OnHelpAbout()));
    connect(pAnalogDemDlg, SIGNAL(About()), this, SLOT(OnHelpAbout()));

    /* Init progress bar for input signal level */
#if QWT_VERSION < 0x060100
    ProgrInputLevel->setRange(-50.0, 0.0);
    ProgrInputLevel->setOrientation(Qt::Vertical, QwtThermo::LeftScale);
#else
    ProgrInputLevel->setScale(-50.0, 0.0);
    ProgrInputLevel->setOrientation(Qt::Vertical);
    ProgrInputLevel->setScalePosition(QwtThermo::TrailingScale);
#endif
    ProgrInputLevel->setAlarmLevel(-12.5);
    QColor alarmColor(QColor(255, 0, 0));
    QColor fillColor(QColor(0, 190, 0));
#if QWT_VERSION < 0x060000
    ProgrInputLevel->setAlarmColor(alarmColor);
    ProgrInputLevel->setFillColor(fillColor);
#else
    QPalette newPalette = FrameMainDisplay->palette();
    newPalette.setColor(QPalette::Base, newPalette.color(QPalette::Window));
    newPalette.setColor(QPalette::ButtonText, fillColor);
    newPalette.setColor(QPalette::Highlight, alarmColor);
    ProgrInputLevel->setPalette(newPalette);
#endif

#ifdef HAVE_LIBHAMLIB
    connect(pStationsDlg, SIGNAL(subscribeRig()), &rig, SLOT(subscribe()));
    connect(pStationsDlg, SIGNAL(unsubscribeRig()), &rig, SLOT(unsubscribe()));
    connect(&rig, SIGNAL(sigstr(double)), pStationsDlg, SLOT(OnSigStr(double)));
    connect(pLogging, SIGNAL(subscribeRig()), &rig, SLOT(subscribe()));
    connect(pLogging, SIGNAL(unsubscribeRig()), &rig, SLOT(unsubscribe()));
#endif
    connect(pSysEvalDlg, SIGNAL(startLogging()), pLogging, SLOT(start()));
    connect(pSysEvalDlg, SIGNAL(stopLogging()), pLogging, SLOT(stop()));

    /* Update times for color LEDs */
    CLED_FAC->SetUpdateTime(1500);
    CLED_SDC->SetUpdateTime(1500);
    CLED_MSC->SetUpdateTime(600);

    connect(pAnalogDemDlg, SIGNAL(SwitchMode(int)), this, SLOT(OnSwitchMode(int)));
    connect(pAnalogDemDlg, SIGNAL(NewAMAcquisition()), this, SLOT(OnNewAcquisition()));
    connect(pAnalogDemDlg, SIGNAL(ViewStationsDlg()), pStationsDlg, SLOT(show()));
    connect(pAnalogDemDlg, SIGNAL(ViewLiveScheduleDlg()), pLiveScheduleDlg, SLOT(show()));
    connect(pAnalogDemDlg, SIGNAL(Closed()), this, SLOT(close()));

    connect(pFMDlg, SIGNAL(SwitchMode(int)), this, SLOT(OnSwitchMode(int)));
    connect(pFMDlg, SIGNAL(Closed()), this, SLOT(close()));
    connect(pFMDlg, SIGNAL(ViewStationsDlg()), pStationsDlg, SLOT(show()));
    connect(pFMDlg, SIGNAL(ViewLiveScheduleDlg()), pLiveScheduleDlg, SLOT(show()));

    connect(&Timer, SIGNAL(timeout()), this, SLOT(OnTimer()));

    connect(&rx, SIGNAL(drmModeStarted()), this, SLOT(ChangeGUIModeToDRM()));
    connect(&rx, SIGNAL(amModeStarted()), this, SLOT(ChangeGUIModeToAM()));
    connect(&rx, SIGNAL(fmModeStarted()), this, SLOT(ChangeGUIModeToFM()));

    serviceLabels[0] = TextMiniService1;
    serviceLabels[1] = TextMiniService2;
    serviceLabels[2] = TextMiniService3;
    serviceLabels[3] = TextMiniService4;

    ClearDisplay();
    UpdateWindowTitle(""); // load filename from settings if set

    /* System tray setup */
    pSysTray = CSysTray::Create(
        this,
        SLOT(OnSysTrayActivated(QSystemTrayIcon::ActivationReason)),
        SLOT(OnSysTrayTimer()),
        ":/icons/MainIcon.svg");
    CSysTray::AddAction(pSysTray, tr("&New Acquisition"), this, SLOT(OnNewAcquisition()));
    CSysTray::AddSeparator(pSysTray);
    CSysTray::AddAction(pSysTray, tr("&Exit"), this, SLOT(close()));

	/* clear signal strenght */
	setBars(0);

	/* Activate real-time timers */
    Timer.start(GUI_CONTROL_UPDATE_TIME);
}

void FDRMDialog::setBars(int bars)
{
	onebar->setAutoFillBackground(bars>0);
	twobars->setAutoFillBackground(bars>1);
	threebars->setAutoFillBackground(bars>2);
	fourbars->setAutoFillBackground(bars>3);
	fivebars->setAutoFillBackground(bars>4);
}

FDRMDialog::~FDRMDialog()
{
    /* Destroying logger */
    delete pLogging;
    /* Destroying top level windows, children are automaticaly destroyed */
    delete pAnalogDemDlg;
    delete pFMDlg;
}

void FDRMDialog::OnSysTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger
#if QT_VERSION < 0x050000
        || reason == QSystemTrayIcon::DoubleClick
#endif
    )
    {
        const Qt::WindowStates ws = pCurrentWindow->windowState();
        if (ws & Qt::WindowMinimized)
            pCurrentWindow->setWindowState((ws & ~Qt::WindowMinimized) | Qt::WindowActive);
        else
            pCurrentWindow->toggleVisibility();
    }
}

void FDRMDialog::OnSysTrayTimer()
{
    QString Title, Message;
    if (rx.GetAcquisitionState() == AS_WITH_SIGNAL)
    {
        CParameter& Parameters = *rx.GetParameters();
        Parameters.Lock();
            const int iCurSelAudioServ = Parameters.GetCurSelAudioService();
            CService audioService = Parameters.Service[iCurSelAudioServ];
            const bool bServiceIsValid = audioService.IsActive()
                                   && (audioService.AudioParam.iStreamID != STREAM_ID_NOT_USED)
                                   && (audioService.eAudDataFlag == CService::SF_AUDIO);
            if (bServiceIsValid)
            {
                /* Service label (UTF-8 encoded string -> convert) */
                Title = QString::fromUtf8(audioService.strLabel.c_str());
                // Text message of current selected audio service (UTF-8 decoding)
                Message = QString::fromUtf8(audioService.AudioParam.strTextMessage.c_str());
            }
		if(Parameters.rWMERMSC>WMERSteps[4])
			setBars(5);
		else if(Parameters.rWMERMSC>WMERSteps[3])
			setBars(4);
		else if(Parameters.rWMERMSC>WMERSteps[2])
			setBars(3);
		else if(Parameters.rWMERMSC>WMERSteps[1])
			setBars(2);
		else if(Parameters.rWMERMSC>WMERSteps[0])
			setBars(1);
		else
			setBars(0);
        Parameters.Unlock();
    }
	else {
        Message = tr("Scanning...");
		setBars(0);
	}
    CSysTray::SetToolTip(pSysTray, Title, Message);
}

void FDRMDialog::OnWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void FDRMDialog::OnSwitchToFM()
{
    OnSwitchMode(RM_FM);
}

void FDRMDialog::OnSwitchToAM()
{
    OnSwitchMode(RM_AM);
}

void FDRMDialog::SetStatus(CMultColorLED* LED, ETypeRxStatus state)
{
    switch(state)
    {
    case NOT_PRESENT:
        LED->Reset(); /* GREY */
        break;

    case CRC_ERROR:
        LED->SetLight(CMultColorLED::RL_RED);
        break;

    case DATA_ERROR:
        LED->SetLight(CMultColorLED::RL_YELLOW);
        break;

    case RX_OK:
        LED->SetLight(CMultColorLED::RL_GREEN);
        break;
    }
}

void FDRMDialog::UpdateDRM_GUI()
{
    bool bMultimediaServiceAvailable;
    CParameter& Parameters = *rx.GetParameters();

    //if (isVisible() == false)
    //    ChangeGUIModeToDRM();

    Parameters.Lock();

    /* Input level meter */
    ProgrInputLevel->setValue(Parameters.GetIFSignalLevel());
    SetStatus(CLED_FAC, Parameters.ReceiveStatus.FAC.GetStatus());
    SetStatus(CLED_SDC, Parameters.ReceiveStatus.SDC.GetStatus());
	int iShortID = Parameters.GetCurSelAudioService();
	if(Parameters.Service[iShortID].eAudDataFlag == CService::SF_AUDIO)
	    SetStatus(CLED_MSC, Parameters.AudioComponentStatus[iShortID].GetStatus());
	else
	    SetStatus(CLED_MSC, Parameters.DataComponentStatus[iShortID].GetStatus());

    Parameters.Unlock();

    /* Clear the multimedia service bit */
    iMultimediaServiceBit = 0;

    /* Check if receiver does receive a signal */
    if(rx.GetAcquisitionState() == AS_WITH_SIGNAL)
        UpdateDisplay();
    else
    {
        ClearDisplay();
        /* No signal then set the last
           multimedia service selected to none */
        iLastMultimediaServiceSelected = -1;
    }
    /* If multimedia service availability has changed
       then update the menu */
    bMultimediaServiceAvailable = iMultimediaServiceBit != 0;
    if (bMultimediaServiceAvailable != action_Multimedia_Dialog->isEnabled())
        action_Multimedia_Dialog->setEnabled(bMultimediaServiceAvailable);
}

void FDRMDialog::startLogging()
{
    pSysEvalDlg->CheckBoxWriteLog->setChecked(true);
}

void FDRMDialog::stopLogging()
{
    pSysEvalDlg->CheckBoxWriteLog->setChecked(false);
}

void FDRMDialog::OnScheduleTimer()
{
    CScheduler::SEvent e;
    e = pScheduler->front();
    if (e.frequency != -1)
    {
        rx.SetFrequency(e.frequency);
        if(!pLogging->enabled())
        {
            startLogging();
        }
    }
    else
    {
        stopLogging();
    }
    if(pScheduler->empty())
    {
        stopLogging();
    }
    else
    {
        e = pScheduler->pop();
        time_t now = time(nullptr);
        pScheduleTimer->start(1000*(e.time-now));
    }
}

void FDRMDialog::OnTimer()
{
    UpdateDRM_GUI(); // TODO move this to signal driven

    // do this here so GUI has initialised before we might pop up a message box
    if(pScheduler!=nullptr)
        return;

    string schedfile = Settings.Get("command", "schedule", string());
    if(schedfile != "")
    {
        bool testMode = Settings.Get("command", "test", false);
        pScheduler = new CScheduler(testMode);
        if(pScheduler->LoadSchedule(schedfile)) {
            pScheduleTimer = new QTimer(this);
            connect(pScheduleTimer, SIGNAL(timeout()), this, SLOT(OnScheduleTimer()));
            /* Setup the first timeout */
            CScheduler::SEvent e;
            if(!pScheduler->empty()) {
                e = pScheduler->front();
                time_t now = time(nullptr);
                time_t next = e.time - now;
                if(next > 0)
                {
                    pScheduleTimer->start(1000*next);
                }
                else // We are late starting
                {
                    startLogging();
                }
            }
        }
        else {
            QMessageBox::information(this, "Dream", tr("Schedule file requested but not found"));
        }
    }
    else
    {
        if(pLogging->enabled())
            startLogging();
    }
}

void FDRMDialog::showTextMessage(const QString& textMessage)
{
    /* Activate text window */
    TextTextMessage->setEnabled(true);

    QString formattedMessage = "";
    for (int i = 0; i < (int)textMessage.length(); i++)
    {
        switch (textMessage.at(i).unicode())
        {
        case 0x0A:
            /* Code 0x0A may be inserted to indicate a preferred
               line break */
        case 0x1F:
            /* Code 0x1F (hex) may be inserted to indicate a
               preferred word break. This code may be used to
            	   display long words comprehensibly */
            formattedMessage += "<br>";
            break;

        case 0x0B:
            /* End of a headline */
            formattedMessage = "<b><u>"
                               + formattedMessage
                               + "</u></b></center><br><center>";
            break;

        case '<':
            formattedMessage += "&lt;";
            break;

        case '>':
            formattedMessage += "&gt;";
            break;

        default:
            formattedMessage += textMessage[int(i)];
        }
    }
    Linkify(formattedMessage);
    formattedMessage = "<center>" + formattedMessage + "</center>";
    TextTextMessage->setText(formattedMessage);

}

void FDRMDialog::showServiceInfo(const CService& service)
{
    /* Service label (UTF-8 encoded string -> convert) */
    QString ServiceLabel(QString::fromUtf8(service.strLabel.c_str()));
    LabelServiceLabel->setText(ServiceLabel);

    /* Service ID (plot number in hexadecimal format) */
    const long iServiceID = (long) service.iServiceID;

    if (iServiceID != 0)
    {
        LabelServiceID->setText(QString("ID:%1").arg(iServiceID,4,16).toUpper());
    }
    else
        LabelServiceID->setText("");

    /* Codec label */
    LabelCodec->setText(GetCodecString(service));

    /* Type (Mono / Stereo) label */
    LabelStereoMono->setText(GetTypeString(service));

    /* Language and program type labels (only for audio service) */
    if (service.eAudDataFlag == CService::SF_AUDIO)
    {
        /* SDC Language */
        const string strLangCode = service.strLanguageCode;

        if ((!strLangCode.empty()) && (strLangCode != "---"))
        {
            LabelLanguage->
            setText(QString(GetISOLanguageName(strLangCode).c_str()));
        }
        else
        {
            /* FAC Language */
            const int iLanguageID = service.iLanguage;

            if ((iLanguageID > 0) &&
                    (iLanguageID < LEN_TABLE_LANGUAGE_CODE))
            {
                LabelLanguage->setText(
                    strTableLanguageCode[iLanguageID].c_str());
            }
            else
                LabelLanguage->setText("");
        }

        /* Program type */
        const int iProgrammTypeID = service.iServiceDescr;

        if ((iProgrammTypeID > 0) &&
                (iProgrammTypeID < LEN_TABLE_PROG_TYPE_CODE))
        {
            LabelProgrType->setText(
                strTableProgTypCod[iProgrammTypeID].c_str());
        }
        else
            LabelProgrType->setText("");
    }

    /* Country code */
    const string strCntryCode = service.strCountryCode;

    if ((!strCntryCode.empty()) && (strCntryCode != "--"))
    {
        LabelCountryCode->
        setText(QString(GetISOCountryName(strCntryCode).c_str()));
    }
    else
        LabelCountryCode->setText("");
}

QString FDRMDialog::serviceSelector(CParameter& Parameters, int i)
{
    Parameters.Lock();

    CService service = Parameters.Service[i];
    const _REAL rAudioBitRate = Parameters.GetBitRateKbps(i, false);
    const _REAL rDataBitRate = Parameters.GetBitRateKbps(i, true);

    /* detect if AFS mux information is available TODO - align to service */
    bool bAFS = ((i==0) && (
                     (Parameters.AltFreqSign.vecMultiplexes.size() > 0)
                     || (Parameters.AltFreqSign.vecOtherServices.size() > 0)
                 ));

    Parameters.Unlock();

    QString text;

    /* Check, if service is used */
    if (service.IsActive())
    {
        /* Do UTF-8 to string conversion with the label strings */
        QString strLabel = QString().fromUtf8(service.strLabel.c_str());

        /* Label for service selection button (service label, codec
           and Mono / Stereo information) */
		QString strCodec = GetCodecString(service);
		QString strType = GetTypeString(service);
		text = strLabel;
		if (!strCodec.isEmpty() || !strType.isEmpty())
			text += "  |   " + strCodec + " " + strType;

        /* Bit-rate (only show if greater than 0) */
        if (rAudioBitRate > (_REAL) 0.0)
        {
            text += " (" + QString().setNum(rAudioBitRate, 'f', 2) + " kbps)";
        }

        /* Audio service */
        if (service.eAudDataFlag == CService::SF_AUDIO)
        {
            /* Report missing codec */
            if (!rx.CanDecode(service.AudioParam.eAudioCoding))
                text += tr(" [no codec available]");

            /* Show, if a multimedia stream is connected to this service */
            if (service.DataParam.iStreamID != STREAM_ID_NOT_USED)
            {
                if (service.DataParam.iUserAppIdent == DAB_AT_EPG)
                    text += tr(" + EPG"); /* EPG service */
                else
                {
                    text += tr(" + MM"); /* other multimedia service */
                    /* Set multimedia service bit */
                    iMultimediaServiceBit |= 1 << i;
                }

                /* Bit-rate of connected data stream */
                text += " (" + QString().setNum(rDataBitRate, 'f', 2) + " kbps)";
            }
        }
        /* Data service */
        else
        {
            if (service.DataParam.ePacketModInd == CDataParam::PM_PACKET_MODE)
            {
                if (service.DataParam.eAppDomain == CDataParam::AD_DAB_SPEC_APP)
                {
                    switch (service.DataParam.iUserAppIdent)
                    {
                    case DAB_AT_BROADCASTWEBSITE:
                    case DAB_AT_JOURNALINE:
                    case DAB_AT_MOTSLIDESHOW:
                        /* Set multimedia service bit */
                        iMultimediaServiceBit |= 1 << i;
                        break;
                    }
                }
            }
        }

        if(bAFS)
        {
            text += tr(" + AFS");
        }

        switch (i)
        {
        case 0:
            PushButtonService1->setEnabled(true);
            break;

        case 1:
            PushButtonService2->setEnabled(true);
            break;

        case 2:
            PushButtonService3->setEnabled(true);
            break;

        case 3:
            PushButtonService4->setEnabled(true);
            break;
        }
    }
    return text;
}

void FDRMDialog::UpdateDisplay()
{
    CParameter& Parameters = *rx.GetParameters();

    Parameters.Lock();

    /* Receiver does receive a DRM signal ------------------------------- */
    /* First get current selected services */
    int iCurSelAudioServ = Parameters.GetCurSelAudioService();

    CService audioService = Parameters.Service[iCurSelAudioServ];
    Parameters.Unlock();

    bool bServiceIsValid = audioService.IsActive()
                           && (audioService.AudioParam.iStreamID != STREAM_ID_NOT_USED)
                           && (audioService.eAudDataFlag == CService::SF_AUDIO);

    int iFirstAudioService=-1, iFirstDataService=-1;
    for(unsigned i=0; i < MAX_NUM_SERVICES; i++)
    {
        QString label = serviceSelector(Parameters, int(i));
        serviceLabels[i]->setText(label);
        pButtonGroup->button(int(i))->setEnabled(label != "");
        if (!bServiceIsValid && (iFirstAudioService == -1 || iFirstDataService == -1))
        {
            Parameters.Lock();
            audioService = Parameters.Service[i];
            Parameters.Unlock();
            /* If the current audio service is not valid
            	find the first audio service available */
            if (iFirstAudioService == -1
                    && audioService.IsActive()
                    && (audioService.AudioParam.iStreamID != STREAM_ID_NOT_USED)
                    && (audioService.eAudDataFlag == CService::SF_AUDIO))
            {
                iFirstAudioService = i;
            }
            /* If the current audio service is not valid
            	find the first data service available */
            if (iFirstDataService == -1
                    && audioService.IsActive()
                    && (audioService.eAudDataFlag == CService::SF_DATA))
            {
                iFirstDataService = i;
            }
        }
    }

    /* Select a valid service, priority to audio service */
    if (iFirstAudioService != -1)
    {
        iCurSelAudioServ = iFirstAudioService;
        bServiceIsValid = true;
    }
    else
    {
        if (iFirstDataService != -1)
        {
            iCurSelAudioServ = iFirstDataService;
            bServiceIsValid = true;
        }
    }

    if(bServiceIsValid)
    {
        /* Get selected service */
        Parameters.Lock();
        audioService = Parameters.Service[iCurSelAudioServ];
        Parameters.Unlock();

        pButtonGroup->button(iCurSelAudioServ)->setChecked(true);

        /* If we have text messages */
        if (audioService.AudioParam.bTextflag)
        {
            // Text message of current selected audio service (UTF-8 decoding)
            QString TextMessage(QString::fromUtf8(audioService.AudioParam.strTextMessage.c_str()));
            showTextMessage(TextMessage);
        }
        else
        {
            /* Deactivate text window */
            TextTextMessage->setEnabled(false);

            /* Clear Text */
            TextTextMessage->setText("");
        }

        /* Check whether service parameters were not transmitted yet */
        if (audioService.IsActive())
        {
            showServiceInfo(audioService);

            Parameters.Lock();
            _REAL rPartABLenRat = Parameters.PartABLenRatio(iCurSelAudioServ);
            _REAL rBitRate = Parameters.GetBitRateKbps(iCurSelAudioServ, false);
            Parameters.Unlock();

            /* Bit-rate */
            QString strBitrate = QString().setNum(rBitRate, 'f', 2) + tr(" kbps");

            /* Equal or unequal error protection */
            if (int(rPartABLenRat) != 0)
            {
                /* Print out the percentage of part A length to total length */
                strBitrate += " UEP (" + QString().setNum(rPartABLenRat * 100, 'f', 1) + " %)";
            }
            else
            {
                /* If part A is zero, equal error protection (EEP) is used */
                strBitrate += " EEP";
            }
            LabelBitrate->setText(strBitrate);

        }
        else
        {
            LabelServiceLabel->setText(tr("No Service"));
            LabelBitrate->setText("");
            LabelCodec->setText("");
            LabelStereoMono->setText("");
            LabelProgrType->setText("");
            LabelLanguage->setText("");
            LabelCountryCode->setText("");
            LabelServiceID->setText("");
        }
    }
}

void FDRMDialog::ClearDisplay()
{
    /* No signal is currently received ---------------------------------- */
    /* Disable service buttons and associated labels */
    pButtonGroup->setExclusive(false);
    for(size_t i=0; i<serviceLabels.size(); i++)
    {
        QPushButton* button = (QPushButton*)pButtonGroup->button(i);
        if (button && button->isEnabled()) button->setEnabled(false);
        if (button && button->isChecked()) button->setChecked(false);
        serviceLabels[i]->setText("");
    }
    pButtonGroup->setExclusive(true);

    /* Main text labels */
    LabelBitrate->setText("");
    LabelCodec->setText("");
    LabelStereoMono->setText("");
    LabelProgrType->setText("");
    LabelLanguage->setText("");
    LabelCountryCode->setText("");
    LabelServiceID->setText("");

    /* Hide text message label */
    TextTextMessage->setEnabled(false);
    TextTextMessage->setText("");

    LabelServiceLabel->setText(tr("Scanning..."));
}

void FDRMDialog::OnSoundFileChanged(QString s)
{
    UpdateWindowTitle(s);
    ClearDisplay();
}

void FDRMDialog::UpdateWindowTitle(QString filename)
{
    QFileInfo fi(filename);
    if(fi.exists()) {

        setWindowTitle(QString("Dream") + " - " + fi.baseName());
        pAnalogDemDlg->setWindowTitle(tr("Analog Demodulation") + " - " + fi.baseName());
        pFMDlg->setWindowTitle(tr("FM Receiver") + " - " + fi.baseName());
    }
    else {
        setWindowTitle("Dream");
        pAnalogDemDlg->setWindowTitle(tr("Analog Demodulation"));
        pFMDlg->setWindowTitle(tr("FM Receiver"));
    }
}

/* change mode is only called when the mode REALLY has changed
 * so no conditionals are needed in this routine
 */

void FDRMDialog::ChangeGUIModeToDRM()
{
    CSysTray::Start(pSysTray);
    pCurrentWindow = this;
    pCurrentWindow->eventUpdate();
    pCurrentWindow->show();
}

void FDRMDialog::ChangeGUIModeToAM()
{
    hide();
    Timer.stop();
    CSysTray::Stop(pSysTray, tr("Dream AM"));
    pCurrentWindow = pAnalogDemDlg;
    pCurrentWindow->eventUpdate();
    pCurrentWindow->show();
}

void FDRMDialog::ChangeGUIModeToFM()
{
    hide();
    Timer.stop();
    CSysTray::Stop(pSysTray, tr("Dream FM"));
    pCurrentWindow = pFMDlg;
    pCurrentWindow->eventUpdate();
    pCurrentWindow->show();
}

void FDRMDialog::eventUpdate()
{
    /* Put (re)initialization code here for the settings that might have
       be changed by another top level window. Called on mode switch */
    //pFileMenu->UpdateMenu();
    SetDisplayColor(CRGBConversion::int2RGB(getSetting("colorscheme", 0xff0000, true)));
}

void FDRMDialog::eventShow(QShowEvent*)
{
    /* Set timer for real-time controls */
    OnTimer();
    Timer.start(GUI_CONTROL_UPDATE_TIME);
}

void FDRMDialog::eventHide(QHideEvent*)
{
    /* Deactivate real-time timers */
    Timer.stop();
}

void FDRMDialog::OnNewAcquisition()
{
    rx.Restart();
}

void FDRMDialog::OnSwitchMode(int newMode)
{
    rx.SetReceiverMode(ERecMode(newMode));
    Timer.start(GUI_CONTROL_UPDATE_TIME);
}

void FDRMDialog::OnSelectAudioService(int shortId)
{
    CParameter& Parameters = *rx.GetParameters();

    Parameters.Lock();

    Parameters.SetCurSelAudioService(shortId);

    Parameters.Unlock();
}

void FDRMDialog::OnSelectDataService(int shortId)
{
    CParameter& Parameters = *rx.GetParameters();
    QWidget* pDlg = nullptr;

    Parameters.Lock();

    int iAppIdent = Parameters.Service[shortId].DataParam.iUserAppIdent;

    switch(iAppIdent)
    {
    case DAB_AT_EPG:
        pDlg = pEPGDlg;
        break;
    case DAB_AT_BROADCASTWEBSITE:
#ifdef QT_WEBENGINE_LIB
        pDlg = pBWSDlg;
#endif
        break;
    case DAB_AT_JOURNALINE:
        pDlg = pJLDlg;
        break;
    case DAB_AT_MOTSLIDESHOW:
        pDlg = pSlideShowDlg;
        break;
    }

    if(pDlg != nullptr)
        Parameters.SetCurSelDataService(shortId);

    CService::ETyOServ eAudDataFlag = Parameters.Service[shortId].eAudDataFlag;

    Parameters.Unlock();

    if(pDlg != nullptr)
    {
        if (pDlg != pEPGDlg)
            iLastMultimediaServiceSelected = shortId;
        pDlg->show();
    }
    else
    {
        if (eAudDataFlag == CService::SF_DATA)
        {
            QMessageBox::information(this, "Dream", tr("unsupported data application"));
        }
    }
}

void FDRMDialog::OnViewMultimediaDlg()
{
    /* Check if multimedia service is available */
    if (iMultimediaServiceBit != 0)
    {
        /* Initialize the multimedia service to show, -1 mean none */
        int iMultimediaServiceToShow = -1;

        /* Check the validity of iLastMultimediaServiceSelected,
           if invalid set it to none */
        if (((1<<iLastMultimediaServiceSelected) & iMultimediaServiceBit) == 0)
            iLastMultimediaServiceSelected = -1;

        /* Simply set to the last selected multimedia if any */
        if (iLastMultimediaServiceSelected != -1)
            iMultimediaServiceToShow = iLastMultimediaServiceSelected;

        else
        {
            /* Select the first multimedia found */
            for (int i = 0; i < MAX_NUM_SERVICES; i++)
            {
                /* A bit is set when a service is available,
                   the position of the bit is the service number */
                if ((1<<i) & iMultimediaServiceBit)
                {
                    /* Service found! */
                    iMultimediaServiceToShow = i;
                    break;
                }
            }
        }
        /* If we have a service then show it */
        if (iMultimediaServiceToShow != -1)
            OnSelectDataService(iMultimediaServiceToShow);
    }
}

void FDRMDialog::OnMenuSetDisplayColor()
{
    const QColor color = CRGBConversion::int2RGB(getSetting("colorscheme", 0xff0000, true));
    const QColor newColor = QColorDialog::getColor(color, this);
    if (newColor.isValid())
    {
        /* Store new color and update display */
        SetDisplayColor(newColor);
        putSetting("colorscheme", CRGBConversion::RGB2int(newColor), true);
    }
}

void FDRMDialog::eventClose(QCloseEvent* ce)
{
    /* The close event has been actioned and we want to shut
     * down, but the main window should be the last thing to
     * close so that the user knows the program has completed
     * when the window closes
     */

    if(done) {
        ce->accept();
    }
    else {
        /* Request that the working thread stops */
        rx.Stop();

        /* Stop real-time timer */
        Timer.stop();

        pLogging->SaveSettings(Settings);

        CSysTray::Destroy(&pSysTray);

        AboutDlg.close();
        pAnalogDemDlg->close();
        pFMDlg->close();
        // we might have already had the signal by now
        if(done) {
            ce->accept();
        }
        else {
            ce->ignore();
        }
    }
}

void FDRMDialog::OnWorkingThreadFinished()
{
    done = true;
    close();
}

QString FDRMDialog::GetCodecString(const CService& service)
{
    QString strReturn;

    /* First check if it is audio or data service */
    if (service.eAudDataFlag == CService::SF_AUDIO)
    {
        /* Audio service */
        const CAudioParam::EAudSamRat eSamRate = service.AudioParam.eAudioSamplRate;

        /* Audio coding */
        switch (service.AudioParam.eAudioCoding)
        {
        case CAudioParam::AC_AAC:
            /* Only 12 and 24 kHz sample rates are supported for AAC encoding in DRM30 */
            if (eSamRate == CAudioParam::AS_12KHZ)
                strReturn = "aac";
            else
                strReturn = "AAC";
            break;

        case CAudioParam::AC_xHE_AAC:
            strReturn = "xHE-AAC";
            break;

		case CAudioParam::AC_OPUS:
			strReturn = "OPUS ";
			/* Opus Audio sub codec */
			switch (service.AudioParam.eOPUSSubCod)
			{
				case CAudioParam::OS_SILK:
					strReturn += "SILK ";
					break;
				case CAudioParam::OS_HYBRID:
					strReturn += "HYBRID ";
					break;
				case CAudioParam::OS_CELT:
					strReturn += "CELT ";
					break;
			}
			/* Opus Audio bandwidth */
			switch (service.AudioParam.eOPUSBandwidth)
			{
				case CAudioParam::OB_NB:
					strReturn += "NB";
					break;
				case CAudioParam::OB_MB:
					strReturn += "MB";
					break;
				case CAudioParam::OB_WB:
					strReturn += "WB";
					break;
				case CAudioParam::OB_SWB:
					strReturn += "SWB";
					break;
				case CAudioParam::OB_FB:
					strReturn += "FB";
					break;
			}
            break;
        default:;
        }

        /* SBR */
        if (service.AudioParam.eSBRFlag == CAudioParam::SB_USED)
        {
            strReturn += "+";
        }
    }
    else
    {
        /* Data service */
        strReturn = "Data:";
    }

    return strReturn;
}

QString FDRMDialog::GetTypeString(const CService& service)
{
    QString strReturn;

    /* First check if it is audio or data service */
    if (service.eAudDataFlag == CService::SF_AUDIO)
    {
        /* Audio service */
        switch (service.AudioParam.eAudioCoding)
        {
        case CAudioParam::AC_NONE:
            break;

        case CAudioParam::AC_OPUS:
            /* Opus channels configuration */
            switch (service.AudioParam.eOPUSChan)
            {
            case CAudioParam::OC_MONO:
            strReturn = "MONO";
            break;

            case CAudioParam::OC_STEREO:
            strReturn = "STEREO";
            break;
            }
            break;

        default:
            /* Mono-Stereo */
            switch (service.AudioParam.eAudioMode)
            {
            case CAudioParam::AM_MONO:
                strReturn = "Mono";
                break;

            case CAudioParam::AM_P_STEREO:
                strReturn = "P-Stereo";
                break;

            case CAudioParam::AM_STEREO:
                strReturn = "Stereo";
                break;
            case CAudioParam::AM_RESERVED:;
            }
        }
    }
    else
    {
        /* Data service */
        if (service.DataParam.ePacketModInd == CDataParam::PM_PACKET_MODE)
        {
            if (service.DataParam.eAppDomain == CDataParam::AD_DAB_SPEC_APP)
            {
                switch (service.DataParam.iUserAppIdent)
                {
                case 1:
                    strReturn = tr("Dynamic labels");
                    break;

                case DAB_AT_MOTSLIDESHOW:
                    strReturn = tr("MOT Slideshow");
                    break;

                case DAB_AT_BROADCASTWEBSITE:
                    strReturn = tr("MOT WebSite");
                    break;

                case DAB_AT_TPEG:
                    strReturn = tr("TPEG");
                    break;

                case DAB_AT_DGPS:
                    strReturn = tr("DGPS");
                    break;

                case DAB_AT_TMC:
                    strReturn = tr("TMC");
                    break;

                case DAB_AT_EPG:
                    strReturn = tr("EPG - Electronic Programme Guide");
                    break;

                case DAB_AT_JAVA:
                    strReturn = tr("Java");
                    break;

                case DAB_AT_JOURNALINE: /* Journaline */
                    strReturn = tr("Journaline");
                    break;
                }
            }
            else
                strReturn = tr("Unknown Service");
        }
        else
            strReturn = tr("Unknown Service");
    }

    return strReturn;
}

void FDRMDialog::SetDisplayColor(const QColor newColor)
{
    /* Collect pointers to the desired controls in a vector */
    vector<QWidget*> vecpWidgets;
    vecpWidgets.push_back(TextTextMessage);
    vecpWidgets.push_back(LabelBitrate);
    vecpWidgets.push_back(LabelCodec);
    vecpWidgets.push_back(LabelStereoMono);
    vecpWidgets.push_back(FrameAudioDataParams);
    vecpWidgets.push_back(LabelProgrType);
    vecpWidgets.push_back(LabelLanguage);
    vecpWidgets.push_back(LabelCountryCode);
    vecpWidgets.push_back(LabelServiceID);
    vecpWidgets.push_back(TextLabelInputLevel);
    vecpWidgets.push_back(ProgrInputLevel);
    vecpWidgets.push_back(CLED_FAC);
    vecpWidgets.push_back(CLED_SDC);
    vecpWidgets.push_back(CLED_MSC);
    vecpWidgets.push_back(FrameMainDisplay);

    for (size_t i = 0; i < vecpWidgets.size(); i++)
    {
        /* Request old palette */
        QPalette CurPal(vecpWidgets[i]->palette());

        /* Change colors */
        if (vecpWidgets[i] != TextTextMessage)
        {
            CurPal.setColor(QPalette::Active, QPalette::Text, newColor);
            CurPal.setColor(QPalette::Active, QPalette::Foreground, newColor);
            CurPal.setColor(QPalette::Inactive, QPalette::Text, newColor);
            CurPal.setColor(QPalette::Inactive, QPalette::Foreground, newColor);
        }
        CurPal.setColor(QPalette::Active, QPalette::Button, newColor);
        CurPal.setColor(QPalette::Active, QPalette::Light, newColor);
        CurPal.setColor(QPalette::Active, QPalette::Dark, newColor);

        CurPal.setColor(QPalette::Inactive, QPalette::Button, newColor);
        CurPal.setColor(QPalette::Inactive, QPalette::Light, newColor);
        CurPal.setColor(QPalette::Inactive, QPalette::Dark, newColor);

        /* Special treatment for text message window */
        if (vecpWidgets[i] == TextTextMessage)
        {
            /* We need to specify special color for disabled */
            CurPal.setColor(QPalette::Disabled, QPalette::Light, Qt::black);
            CurPal.setColor(QPalette::Disabled, QPalette::Dark, Qt::black);
        }

        /* Set new palette */
        vecpWidgets[i]->setPalette(CurPal);
    }
}

void FDRMDialog::AddWhatsThisHelp()
{
    /*
    	This text was taken from the only documentation of Dream software
    */
    /* Text Message */
    QString strTextMessage =
        tr("<b>Text Message:</b> On the top right the text "
           "message label is shown. This label only appears when an actual text "
           "message is transmitted. If the current service does not transmit a "
           "text message, the label will be disabled.");

    /* Input Level */
    const QString strInputLevel =
        tr("<b>Input Level:</b> The input level meter shows "
           "the relative input signal peak level in dB. If the level is too high, "
           "the meter turns from green to red. The red region should be avoided "
           "since overload causes distortions which degrade the reception "
           "performance. Too low levels should be avoided too, since in this case "
           "the Signal-to-Noise Ratio (SNR) degrades.");


    /* Status LEDs */
    const QString strStatusLEDS =
        tr("<b>Status LEDs:</b> The three status LEDs show "
           "the current CRC status of the three logical channels of a DRM stream. "
           "These LEDs are the same as the top LEDs on the Evaluation Dialog.");


    /* Station Label and Info Display */
    const QString strStationLabelOther =
        tr("<b>Station Label and Info Display:</b> In the "
           "big label with the black background the station label and some other "
           "information about the current selected service is displayed. The "
           "magenta text on the top shows the bit-rate of the current selected "
           "service (The abbreviations EEP and "
           "UEP stand for Equal Error Protection and Unequal Error Protection. "
           "UEP is a feature of DRM for a graceful degradation of the decoded "
           "audio signal in case of a bad reception situation. UEP means that "
           "some parts of the audio is higher protected and some parts are lower "
           "protected (the ratio of higher protected part length to total length "
           "is shown in the brackets)), the audio compression format "
           "(e.g. AAC), if SBR is used and what audio mode is used (Mono, Stereo, "
           "P-Stereo -> low-complexity or parametric stereo). In case SBR is "
           "used, the actual sample rate is twice the sample rate of the core AAC "
           "decoder. The next two types of information are the language and the "
           "program type of the service (e.g. German / News).<br>The big "
           "turquoise text in the middle is the station label. This label may "
           "appear later than the magenta text since this information is "
           "transmitted in a different logical channel of a DRM stream. On the "
           "right, the ID number connected with this service is shown.");


    /* Service Selectors */
    const QString strServiceSel =
        tr("<b>Service Selectors:</b> In a DRM stream up to "
           "four services can be carried. The service can be an audio service, "
           "a data service or an audio service with data. "
           "Audio services can have associated text messages, in addition to any data component. "
           "If a Multimedia data service is selected, the Multimedia Dialog will automatically show up. "
           "On the right of each service selection button a short description of the service is shown. "
           "If an audio service has associated Multimedia data, \"+ MM\" is added to this text. "
           "If such a service is selected, opening the Multimedia Dialog will allow the data to be viewed "
           "while the audio is still playing. If the data component of a service is not Multimedia, "
           "but an EPG (Electronic Programme Guide) \"+ EPG\" is added to the description. "
           "The accumulated Programme Guides for all stations can be viewed by opening the Programme Guide Dialog. "
           "The selected channel in the Programme Guide Dialog defaults to the station being received. "
           "If Alternative Frequency Signalling is available, \"+ AFS\" is added to the description. "
           "In this case the alternative frequencies can be viewed by opening the Live Schedule Dialog."
          );

    TextTextMessage->setWhatsThis(strTextMessage);
    TextLabelInputLevel->setWhatsThis(strInputLevel);
    ProgrInputLevel->setWhatsThis(strInputLevel);
    CLED_MSC->setWhatsThis(strStatusLEDS);
    CLED_SDC->setWhatsThis(strStatusLEDS);
    CLED_FAC->setWhatsThis(strStatusLEDS);
    LabelBitrate->setWhatsThis(strStationLabelOther);
    LabelCodec->setWhatsThis(strStationLabelOther);
    LabelStereoMono->setWhatsThis(strStationLabelOther);
    LabelServiceLabel->setWhatsThis(strStationLabelOther);
    LabelProgrType->setWhatsThis(strStationLabelOther);
    LabelServiceID->setWhatsThis(strStationLabelOther);
    LabelLanguage->setWhatsThis(strStationLabelOther);
    LabelCountryCode->setWhatsThis(strStationLabelOther);
    FrameAudioDataParams->setWhatsThis(strStationLabelOther);
    PushButtonService1->setWhatsThis(strServiceSel);
    PushButtonService2->setWhatsThis(strServiceSel);
    PushButtonService3->setWhatsThis(strServiceSel);
    PushButtonService4->setWhatsThis(strServiceSel);
    TextMiniService1->setWhatsThis(strServiceSel);
    TextMiniService2->setWhatsThis(strServiceSel);
    TextMiniService3->setWhatsThis(strServiceSel);
    TextMiniService4->setWhatsThis(strServiceSel);

    const QString strBars = tr("from 1 to 5 bars indicates WMER in the range 8 to 24 dB");
	onebar->setWhatsThis(strBars);
	twobars->setWhatsThis(strBars);
	threebars->setWhatsThis(strBars);
	fourbars->setWhatsThis(strBars);
	fivebars->setWhatsThis(strBars);
}
