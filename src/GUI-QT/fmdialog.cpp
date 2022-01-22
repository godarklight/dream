/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2010
 *
 * Author(s):
 *	Julian Cable
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

#include "fmdialog.h"
#include "DialogUtil.h"
#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
#endif
#include <QMessageBox>
#include <QInputDialog>
#include <QWhatsThis>
#include <QShowEvent>
#include <QHideEvent>
#include <QCloseEvent>
#include <QEvent>
#include <qwt_thermo.h>
#include "SoundCardSelMenu.h"

/* Implementation *************************************************************/
FMDialog::FMDialog(CRx& nrx, CSettings& Settings,
	CFileMenu* pFileMenu, CSoundCardSelMenu* pSoundCardMenu, QWidget* parent) :
	CWindow(parent, Settings, "FM"),
    rx(nrx),
	pFileMenu(pFileMenu), pSoundCardMenu(pSoundCardMenu), eReceiverMode(RM_NONE)
{
	setupUi(this);

	/* Set help text for the controls */
	AddWhatsThisHelp();

	/* Add file and sound card menu */
	menuBar()->insertMenu(menu_View->menuAction(), pFileMenu);
	menu_Settings->addMenu(pSoundCardMenu);

	connect(actionTune, SIGNAL(triggered()), this, SLOT(OnTune()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(actionAM, SIGNAL(triggered()), this, SLOT(OnSwitchToAM()));
	connect(actionDRM, SIGNAL(triggered()), this, SLOT(OnSwitchToDRM()));
	connect(actionDisplayColor, SIGNAL(triggered()), this, SLOT(OnMenuSetDisplayColor()));
	connect(actionAbout_Dream, SIGNAL(triggered()), this, SLOT(OnHelpAbout()));
	connect(actionWhats_This, SIGNAL(triggered()), this, SLOT(OnWhatsThis()));

	/* Reset text */
	LabelBitrate->setText("");
	LabelCodec->setText("");
	LabelStereoMono->setText("");
	LabelServiceLabel->setText("");
	LabelProgrType->setText("");
	LabelLanguage->setText("");
	LabelCountryCode->setText("");
	LabelServiceID->setText("");

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

	/* Update times for color LEDs */
	CLED_FAC->SetUpdateTime(1500);
	CLED_SDC->SetUpdateTime(1500);
	CLED_MSC->SetUpdateTime(600);

	/* Connect timers */
	connect(&Timer, SIGNAL(timeout()),
		this, SLOT(OnTimer()));
	connect(&TimerClose, SIGNAL(timeout()),
		this, SLOT(OnTimerClose()));

	/* Activate real-time timers */
 	Timer.start(GUI_CONTROL_UPDATE_TIME);
}

void FMDialog::OnWhatsThis()
{
	QWhatsThis::enterWhatsThisMode();
}

void FMDialog::OnSwitchToDRM()
{
	emit SwitchMode(RM_DRM);
}

void FMDialog::OnSwitchToAM()
{
	emit SwitchMode(RM_AM);
}

void FMDialog::OnTune()
{
	bool ok;
    double freq = double(rx.GetFrequency())/1000.0;
	double f = QInputDialog::getDouble(this, tr("Dream FM"), tr("Frequency (MHz):"), freq, 86.0, 110.0, 2, &ok);
	if (ok)
	{
        rx.SetFrequency(int(1000.0*f));
	}
}

void FMDialog::SetStatus(CMultColorLED* LED, ETypeRxStatus state)
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

void FMDialog::OnTimer()
{
    ERecMode eNewReceiverMode = rx.GetReceiverMode();
	switch(eNewReceiverMode)
	{
	case RM_DRM:
		this->hide();
		break;
	case RM_AM:
		this->hide();
		break;
	case RM_FM:
		{
            CParameter& Parameters = *rx.GetParameters();
			Parameters.Lock();

			/* Input level meter */
			ProgrInputLevel->setValue(Parameters.GetIFSignalLevel());

			SetStatus(CLED_SDC, Parameters.ReceiveStatus.SDC.GetStatus());
			SetStatus(CLED_FAC, Parameters.ReceiveStatus.FAC.GetStatus());
			// TODO Data broadcasts
			int iShortID = Parameters.GetCurSelAudioService();
			SetStatus(CLED_MSC, Parameters.AudioComponentStatus[iShortID].GetStatus());

            int freq = rx.GetFrequency();
			QString fs = QString("%1 MHz").arg(double(freq)/1000.0, 5, 'f', 2);

			LabelServiceLabel->setText(fs);

			Parameters.Unlock();

			/* Check if receiver does receive a signal */
            if(rx.GetAcquisitionState() == AS_WITH_SIGNAL)
				UpdateDisplay();
			else
				ClearDisplay();
		}
		break;
	default: // wait until working thread starts operating
		break;
	}
}

void FMDialog::OnTimerClose()
{
		close();
}

void FMDialog::UpdateDisplay()
{
    CParameter& Parameters = *(rx.GetParameters());

	Parameters.Lock();

	/* Receiver does receive a DRM signal ------------------------------- */
	/* First get current selected services */
	int iCurSelAudioServ = Parameters.GetCurSelAudioService();

	/* If the current audio service is not active or is an only data service
	   select the first audio service available */

	if (!Parameters.Service[iCurSelAudioServ].IsActive() ||
	    Parameters.Service[iCurSelAudioServ].AudioParam.iStreamID == STREAM_ID_NOT_USED ||
	    Parameters.Service[iCurSelAudioServ].eAudDataFlag == CService::SF_DATA)
	{
		int i = 0;
		bool bStop = false;

		while ((bStop == false) && (i < MAX_NUM_SERVICES))
		{
			if (Parameters.Service[i].IsActive() &&
			    Parameters.Service[i].AudioParam.iStreamID != STREAM_ID_NOT_USED &&
			    Parameters.Service[i].eAudDataFlag == CService::SF_AUDIO)
			{
				iCurSelAudioServ = i;
				bStop = true;
			}
			else
				i++;
		}
	}

	//const int iCurSelDataServ = Parameters.GetCurSelDataService();

	/* Check whether service parameters were not transmitted yet */
	if (Parameters.Service[iCurSelAudioServ].IsActive())
	{
		/* Service label (UTF-8 encoded string -> convert)
		LabelServiceLabel->setText(QString().fromUtf8(QCString(
			Parameters.Service[iCurSelAudioServ].
			strLabel.c_str())));
*/
		/* Bit-rate */
		QString strBitrate = QString().setNum(Parameters.
			GetBitRateKbps(iCurSelAudioServ, false), 'f', 2) +
			tr(" kbps");

		/* Equal or unequal error protection */
		const _REAL rPartABLenRat =
			Parameters.PartABLenRatio(iCurSelAudioServ);

		if (rPartABLenRat != (_REAL) 0.0)
		{
			/* Print out the percentage of part A length to total length */
			strBitrate += " UEP (" +
				QString().setNum(rPartABLenRat * 100, 'f', 1) + " %)";
		}
		else
		{
			/* If part A is zero, equal error protection (EEP) is used */
			strBitrate += " EEP";
		}
		LabelBitrate->setText(strBitrate);

		/* Service ID (plot number in hexadecimal format) */
		const long iServiceID = (long) Parameters.
			Service[iCurSelAudioServ].iServiceID;

		if (iServiceID != 0)
		{
			LabelServiceID->setText(QString("ID:%1").arg(iServiceID,4,16));
		}
		else
			LabelServiceID->setText("");

		/* Codec label */
		LabelCodec->setText(GetCodecString(iCurSelAudioServ));

		/* Type (Mono / Stereo) label */
		LabelStereoMono->setText(GetTypeString(iCurSelAudioServ));

		/* Language and program type labels (only for audio service) */
		if (Parameters.Service[iCurSelAudioServ].
			eAudDataFlag == CService::SF_AUDIO)
		{
		/* SDC Language */
		const string strLangCode = Parameters.
			Service[iCurSelAudioServ].strLanguageCode;

		if ((!strLangCode.empty()) && (strLangCode != "---"))
		{
			 LabelLanguage->
				setText(QString(GetISOLanguageName(strLangCode).c_str()));
		}
		else
		{
			/* FAC Language */
			const int iLanguageID = Parameters.
				Service[iCurSelAudioServ].iLanguage;

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
			const int iProgrammTypeID = Parameters.
				Service[iCurSelAudioServ].iServiceDescr;

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
		const string strCntryCode = Parameters.
			Service[iCurSelAudioServ].strCountryCode;

		if ((!strCntryCode.empty()) && (strCntryCode != "--"))
		{
			LabelCountryCode->
				setText(QString(GetISOCountryName(strCntryCode).c_str()));
		}
		else
			LabelCountryCode->setText("");
		}
	else
	{
		//LabelServiceLabel->setText(tr("No Service"));

		LabelBitrate->setText("");
		LabelCodec->setText("");
		LabelStereoMono->setText("");
		LabelProgrType->setText("");
		LabelLanguage->setText("");
		LabelCountryCode->setText("");
		LabelServiceID->setText("");
	}


	/* Service selector ------------------------------------------------- */
	/* Enable only so many number of channel switches as present in the stream */
	const int iNumServices = Parameters.GetTotNumServices();
	(void)iNumServices; // probably irrelevant for FM

	QString m_StaticService[MAX_NUM_SERVICES] = {"", "", "", ""};

	/* detect if AFS informations are available */
	if ((Parameters.AltFreqSign.vecMultiplexes.size() > 0) || (Parameters.AltFreqSign.vecOtherServices.size() > 0))
	{
		/* show AFS label */
		if (Parameters.Service[0].eAudDataFlag
				== CService::SF_AUDIO) m_StaticService[0] += tr(" + AFS");
	}
	Parameters.Unlock();
}

void FMDialog::ClearDisplay()
{
	/* Main text labels */
	LabelBitrate->setText("");
	LabelCodec->setText("");
	LabelStereoMono->setText("");
	LabelProgrType->setText("");
	LabelLanguage->setText("");
	LabelCountryCode->setText("");
	LabelServiceID->setText("");
	//LabelServiceLabel->setText(tr("Scanning..."));
}

void FMDialog::eventUpdate()
{
	/* Put (re)initialization code here for the settings that might have
	   be changed by another top level window. Called on mode switch */
    //pFileMenu->UpdateMenu();
    cerr << "FMDialog eventUpdate" << endl;
	SetDisplayColor(CRGBConversion::int2RGB(getSetting("colorscheme", 0xff0000, true)));
}

void FMDialog::eventShow(QShowEvent*)
{
	/* Set timer for real-time controls */
	OnTimer();
 	Timer.start(GUI_CONTROL_UPDATE_TIME);
}

void FMDialog::eventHide(QHideEvent*)
{
	/* Deactivate real-time timer */
	Timer.stop();
}

void FMDialog::SetService(int iNewServiceID)
{
    CParameter& Parameters = *rx.GetParameters();
	Parameters.Lock();
	Parameters.SetCurSelAudioService(iNewServiceID);
	Parameters.Unlock();
}

void FMDialog::OnMenuSetDisplayColor()
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

void FMDialog::eventClose(QCloseEvent*)
{
	if (!TimerClose.isActive())
	{
		/* Stop real-time timer */
		Timer.stop();

		/* Tell every other window to close too */
		emit Closed();

		/* Set the timer for polling the working thread state */
		TimerClose.start(50);
    }

    /* TODO Stay open until working thread is done */
}

QString FMDialog::GetCodecString(const int iServiceID)
{
	QString strReturn;

    CParameter& Parameters = *rx.GetParameters();

	/* First check if it is audio or data service */
	if (Parameters.Service[iServiceID].eAudDataFlag == CService::SF_AUDIO)
	{
		/* Audio service */
		const CAudioParam::EAudSamRat eSamRate = Parameters.
			Service[iServiceID].AudioParam.eAudioSamplRate;

		/* Audio coding */
		switch (Parameters.Service[iServiceID].AudioParam.eAudioCoding)
		{
        case CAudioParam::AC_RESERVED:
        case CAudioParam::AC_NONE:
			break;

		case CAudioParam::AC_AAC:
			/* Only 12 and 24 kHz sample rates are supported for AAC encoding */
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
			/* Opus audio sub codec */
			switch (Parameters.Service[iServiceID].AudioParam.eOPUSSubCod)
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
			/* Opus audio bandwidth */
			switch (Parameters.Service[iServiceID].AudioParam.eOPUSBandwidth)
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
		}

		/* SBR */
		if (Parameters.Service[iServiceID].
			AudioParam.eSBRFlag == CAudioParam::SB_USED)
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

QString FMDialog::GetTypeString(const int iServiceID)
{
	QString strReturn;

    CParameter& Parameters = *rx.GetParameters();

	/* First check if it is audio or data service */
	if (Parameters.Service[iServiceID].
		eAudDataFlag == CService::SF_AUDIO)
	{
        /* Audio service */
        switch (Parameters.Service[iServiceID].AudioParam.eAudioCoding)
        {
        case CAudioParam::AC_NONE:
            break;

        case CAudioParam::AC_OPUS:
            /* Opus channels configuration */
            switch (Parameters.Service[iServiceID].AudioParam.eOPUSChan)
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
            switch (Parameters.Service[iServiceID].AudioParam.eAudioMode)
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
            case CAudioParam::AM_RESERVED:
                ;
            }
        }
	}

	return strReturn;
}

void FMDialog::SetDisplayColor(const QColor newColor)
{
	/* Collect pointer to the desired controls in a vector */
	vector<QWidget*> vecpWidgets;
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
		CurPal.setColor(QPalette::Active, QPalette::Foreground, newColor);
		CurPal.setColor(QPalette::Active, QPalette::Button, newColor);
		CurPal.setColor(QPalette::Active, QPalette::Text, newColor);
		CurPal.setColor(QPalette::Active, QPalette::Light, newColor);
		CurPal.setColor(QPalette::Active, QPalette::Dark, newColor);

		CurPal.setColor(QPalette::Inactive, QPalette::Foreground, newColor);
		CurPal.setColor(QPalette::Inactive, QPalette::Button, newColor);
		CurPal.setColor(QPalette::Inactive, QPalette::Text, newColor);
		CurPal.setColor(QPalette::Inactive, QPalette::Light, newColor);
		CurPal.setColor(QPalette::Inactive, QPalette::Dark, newColor);
		/* Set new palette */
		vecpWidgets[i]->setPalette(CurPal);
	}
}

void FMDialog::AddWhatsThisHelp()
{
/*
	This text was taken from the only documentation of Dream software
*/
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
}
