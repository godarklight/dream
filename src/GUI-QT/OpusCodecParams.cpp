/******************************************************************************\
 *
 * Copyright (c) 2012-2013
 *
 * Author(s):
 *  David Flamand
 *
 * Description:
 *  Opus Codec Parameters Dialog
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

#include <QPushButton>
#include <QString>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLayout>
#include "OpusCodecParams.h"
#include "DialogUtil.h"
#include "../util-QT/Util.h"
#include "../DrmTransmitter.h"
#include "../sourcedecoders/opus_codec.h"


OpusCodecParams::OpusCodecParams(CSettings& Settings, CParameter& Parameters,
	int iShortID, QWidget* parent)
	:
	QDialog(parent), Settings(Settings), Parameters(Parameters),
	iShortID(iShortID), bWasVisible(false), bLastPositionValid(false)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	setupUi(this);

	/* Recover window size and position */
	CWinGeom s;
	Settings.Get("Codec Dialog", s);
	if (s.iXPos && s.iYPos)
		move(s.iXPos, s.iYPos);

	QString strWebSite(OPUS_WEBSITE_LINK);
	LabelInfoDescription->setText(tr(OPUS_DESCRIPTION));
	LabelInfoVersion->setText(opusGetVersion());
	LabelInfoWebSite->setText(Linkify(strWebSite));

	Parameters.Lock();
	/* Channels */
	switch (Parameters.Service[iShortID].AudioParam.eOPUSChan)
	{
	case CAudioParam::OC_MONO:
		RadioButtonChannelsMono->setChecked(true);
		break;
	case CAudioParam::OC_STEREO:
		RadioButtonChannelsStereo->setChecked(true);
		break;
	}
	/* Bandwith */
	switch (Parameters.Service[iShortID].AudioParam.eOPUSBandwidth)
	{
	case CAudioParam::OB_NB:
		RadioButtonBandwidthNB->setChecked(true);
		break;
	case CAudioParam::OB_MB:
		RadioButtonBandwidthMB->setChecked(true);
		break;
	case CAudioParam::OB_WB:
		RadioButtonBandwidthWB->setChecked(true);
		break;
	case CAudioParam::OB_SWB:
		RadioButtonBandwidthSWB->setChecked(true);
		break;
	case CAudioParam::OB_FB:
		RadioButtonBandwidthFB->setChecked(true);
		break;
	}
	/* FEC */
	if (Parameters.Service[iShortID].AudioParam.bOPUSForwardErrorCorrection)
		RadioButtonFECOn->setChecked(true);
	else
		RadioButtonFECOff->setChecked(true);
	/* Signal */
	switch (Parameters.Service[iShortID].AudioParam.eOPUSSignal)
	{
	case CAudioParam::OG_VOICE:
		RadioButtonSignalVoice->setChecked(true);
		break;
	case CAudioParam::OG_MUSIC:
		RadioButtonSignalMusic->setChecked(true);
		break;
	}
	/* Application */
	switch (Parameters.Service[iShortID].AudioParam.eOPUSApplication)
	{
	case CAudioParam::OA_VOIP:
		RadioButtonAppVOIP->setChecked(true);
		break;
	case CAudioParam::OA_AUDIO:
		RadioButtonAppAudio->setChecked(true);
		break;
	}
	Parameters.Unlock();

	/* Connections */
	connect(ButtonGroupChannels, SIGNAL(buttonClicked(int)),
		this, SLOT(OnButtonGroupChannels(int)));
	connect(ButtonGroupBandwidth, SIGNAL(buttonClicked(int)),
		this, SLOT(OnButtonGroupBandwidth(int)));
	connect(ButtonGroupFEC, SIGNAL(buttonClicked(int)),
		this, SLOT(OnButtonGroupFEC(int)));
	connect(ButtonGroupSignal, SIGNAL(buttonClicked(int)),
		this, SLOT(OnButtonGroupSignal(int)));
	connect(ButtonGroupApplication, SIGNAL(buttonClicked(int)),
		this, SLOT(OnButtonGroupApplication(int)));
}

OpusCodecParams::~OpusCodecParams()
{
	/* Save window position */
	CWinGeom s;
	if (bLastPositionValid)
	{
		s.iXPos = iLastXPosition;
		s.iYPos = iLastYPosition;
	}
	else {
		QRect WinGeom = geometry();
		if (WinGeom.isValid() && !WinGeom.isEmpty() && !WinGeom.isNull()) {
			s.iXPos = WinGeom.x();
			s.iYPos = WinGeom.y();
		}
	}
	Settings.Put("Codec Dialog", s);
}

void OpusCodecParams::reject()
{ 
	GetDialogPosition();
	QDialog::reject();
}

void OpusCodecParams::OnButtonGroupChannels(int iID)
{
	iID = -iID - 2; // TODO understand why
	CAudioParam::EOPUSChan eOPUSChan;
	switch (iID)
	{
	case 0:
		eOPUSChan = CAudioParam::OC_MONO;
		break;
	default:
	case 1:
		eOPUSChan = CAudioParam::OC_STEREO;
		break;
	}
	Parameters.Lock();
	Parameters.Service[iShortID].AudioParam.eOPUSChan = eOPUSChan;
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void OpusCodecParams::OnButtonGroupBandwidth(int iID)
{
	iID = -iID - 2; // TODO understand why
	CAudioParam::EOPUSBandwidth eOPUSBandwidth;
	switch (iID)
	{
	case 0:
		eOPUSBandwidth = CAudioParam::OB_NB;
		break;
	case 1:
		eOPUSBandwidth = CAudioParam::OB_MB;
		break;
	case 2:
		eOPUSBandwidth = CAudioParam::OB_WB;
		break;
	case 3:
		eOPUSBandwidth = CAudioParam::OB_SWB;
		break;
	default:
	case 4:
		eOPUSBandwidth = CAudioParam::OB_FB;
		break;
	}
	Parameters.Lock();
	Parameters.Service[iShortID].AudioParam.eOPUSBandwidth = eOPUSBandwidth;
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void OpusCodecParams::OnButtonGroupFEC(int iID)
{
	iID = -iID - 2; // TODO understand why
	bool bOPUSForwardErrorCorrection;
	switch (iID)
	{
	default:
	case 0:
		bOPUSForwardErrorCorrection = false;
		break;
	case 1:
		bOPUSForwardErrorCorrection = true;
		break;
	}
	Parameters.Lock();
	Parameters.Service[iShortID].AudioParam.bOPUSForwardErrorCorrection = bOPUSForwardErrorCorrection;
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void OpusCodecParams::OnButtonGroupSignal(int iID)
{
	iID = -iID - 2; // TODO understand why
	CAudioParam::EOPUSSignal eOPUSSignal;
	switch (iID)
	{
	case 0:
		eOPUSSignal = CAudioParam::OG_VOICE;
		break;
	default:
	case 1:
		eOPUSSignal = CAudioParam::OG_MUSIC;
		break;
	}
	Parameters.Lock();
	Parameters.Service[iShortID].AudioParam.eOPUSSignal = eOPUSSignal;
	Parameters.Service[iShortID].AudioParam.bOPUSRequestReset = true;
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void OpusCodecParams::OnButtonGroupApplication(int iID)
{
	iID = -iID - 2; // TODO understand why
	CAudioParam::EOPUSApplication eOPUSApplication;
	switch (iID)
	{
	case 0:
		eOPUSApplication = CAudioParam::OA_VOIP;
		break;
	default:
	case 1:
		eOPUSApplication = CAudioParam::OA_AUDIO;
		break;
	}
	Parameters.Lock();
	Parameters.Service[iShortID].AudioParam.eOPUSApplication = eOPUSApplication;
	Parameters.Service[iShortID].AudioParam.bOPUSRequestReset = true;
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void OpusCodecParams::Toggle(void)
{
	bWasVisible = isVisible();
	if (bWasVisible)
	{
		GetDialogPosition();
		hide();
	}
	else
		show();
}

void OpusCodecParams::Show(bool bShow)
{
	if (bShow)
	{
		if (bWasVisible)
			show();
	}
	else {
		bWasVisible = isVisible();
		if (bWasVisible)
			GetDialogPosition();
		hide();
	}
}

void OpusCodecParams::GetDialogPosition(void)
{
	QRect WinGeom = geometry();
	if (WinGeom.isValid() && !WinGeom.isEmpty() && !WinGeom.isNull())
	{
		iLastXPosition = WinGeom.x();
		iLastYPosition = WinGeom.y();
		bLastPositionValid = true;
	}
}

