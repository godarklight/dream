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
#include "AACCodecParams.h"
#include "DialogUtil.h"
#include "../util-QT/Util.h"
#include "../DrmTransmitter.h"
#ifdef HAVE_LIBFDK_AAC
# include "../sourcedecoders/fdk_aac_codec.h"
#endif

AACCodecParams::AACCodecParams(CSettings& Settings, CParameter& Parameters,
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

    QString strWebSite("https://www.iis.fraunhofer.de/en/ff/amm/prod/audiocodec/audiocodecs/heaac.html");
    LabelInfoDescription->setText(tr("HE-AAC"));
    LabelInfoVersion->setText("2.0.0");
	LabelInfoWebSite->setText(Linkify(strWebSite));

	Parameters.Lock();
	/* Channels */
    switch (Parameters.Service[iShortID].AudioParam.eAudioMode)
	{
    case CAudioParam::AM_MONO:
		RadioButtonChannelsMono->setChecked(true);
		break;
    case CAudioParam::AM_STEREO:
		RadioButtonChannelsStereo->setChecked(true);
		break;
    case CAudioParam::AM_P_STEREO:
        RadioButtonChannelsParametricStereo->setChecked(true);
        break;
    case CAudioParam::AM_RESERVED:
        ;
    }
    /* Sample Rate */
    switch (Parameters.Service[iShortID].AudioParam.eAudioSamplRate)
	{
    case CAudioParam::AS_12KHZ:
        RadioButton12kHz->setChecked(true);
		break;
    case CAudioParam::AS_24KHZ:
        RadioButton24kHz->setChecked(true);
		break;
	}
    /* options */
    CheckBoxSBR->setCheckState(Parameters.Service[iShortID].AudioParam.eSBRFlag?Qt::Checked:Qt::Unchecked);
	Parameters.Unlock();

	/* Connections */
    connect(ButtonGroupChannels, SIGNAL(buttonClicked(int)), this, SLOT(OnButtonGroupChannels(int)));
    connect(ButtonGroupSampleRate, SIGNAL(buttonClicked(int)),		this, SLOT(OnButtonGroupSampleRate(int)));
    connect(CheckBoxSBR, SIGNAL(toggled(bool)),		this, SLOT(OnCheckBoxSBR(bool)));

    ButtonGroupChannels->addButton(RadioButtonChannelsMono, CAudioParam::AM_MONO);
    ButtonGroupChannels->addButton(RadioButtonChannelsStereo, CAudioParam::AM_STEREO);
    ButtonGroupChannels->addButton(RadioButtonChannelsParametricStereo, CAudioParam::AM_P_STEREO);
    //ButtonGroupChannels->addButton(RadioButtonChannelsSurround, CAudioParam::AM_SURROUND);

    ButtonGroupSampleRate->addButton(RadioButton12kHz, CAudioParam::AS_12KHZ);
    ButtonGroupSampleRate->addButton(RadioButton24kHz, CAudioParam::AS_24KHZ);
}

AACCodecParams::~AACCodecParams()
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

void AACCodecParams::reject()
{ 
	GetDialogPosition();
	QDialog::reject();
}

void AACCodecParams::OnButtonGroupChannels(int iID)
{
    Parameters.Lock();
    Parameters.Service[iShortID].AudioParam.eAudioMode = static_cast<CAudioParam::EAudioMode>(iID);
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void AACCodecParams::OnButtonGroupSampleRate(int iID)
{
	Parameters.Lock();
    Parameters.Service[iShortID].AudioParam.eAudioSamplRate =  static_cast<CAudioParam::EAudSamRat>(iID);
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void AACCodecParams::OnCheckBoxSBR(bool checked)
{
	Parameters.Lock();
    Parameters.Service[iShortID].AudioParam.eSBRFlag = checked?CAudioParam::SB_USED:CAudioParam::SB_NOT_USED;
	Parameters.Service[iShortID].AudioParam.bParamChanged = true;
	Parameters.Unlock();
}

void AACCodecParams::Toggle(void)
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

void AACCodecParams::Show(bool bShow)
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

void AACCodecParams::GetDialogPosition(void)
{
	QRect WinGeom = geometry();
	if (WinGeom.isValid() && !WinGeom.isEmpty() && !WinGeom.isNull())
	{
		iLastXPosition = WinGeom.x();
		iLastYPosition = WinGeom.y();
		bLastPositionValid = true;
	}
}

