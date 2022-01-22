/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2012
 *
 * Author(s):
 *      Julian Cable, David Flamand
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
#ifndef SOUNDCARDMENU_H
#define SOUNDCARDMENU_H

#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include <QMainWindow>
#include "../DrmReceiver.h"
#include "../DrmTransceiver.h"
#include "../sound/selectioninterface.h"
#include "../main-Qt/ctrx.h"

typedef struct CHANSEL {
    const char* Name;
    const int iChanSel;
} CHANSEL;

class CFileMenu;
class CSoundCardSelMenu : public QMenu
{
    Q_OBJECT

public:
    CSoundCardSelMenu(
        CTRx& ntrx,
        CFileMenu* pFileMenu,
        QWidget* parent = 0);

protected:
    CTRx&               trx;
    QMenu*				menuSigInput;
    QMenu*				menuInputDev;
    QMenu*				menuInputSampleRate;
    QMenu*				menuOutputSampleRate;
    QMenu*				menuOutputDev;
    QMenu*              menuInputChannel;
    QMenu*              menuOutputChannel;

    const bool			bReceiver;
    QAction*            actionUpscale;

    QMenu* InitChannel(QMenu* parent, const QString& text, const CHANSEL* ChanSel);
    QMenu* InitSampleRate(QMenu* parent, const QString& text, const int* SampleRate);
    void UpdateDeviceMenu(QMenu* menu, const std::vector<string>& names, const std::vector<string>& descriptions, const std::string& selected);

public slots:
    // slots connected to signals from user
    void OnSoundInChannel(QAction*);
    void OnSoundOutChannel(QAction*);
    void OnSoundInDevice(QAction*);
    void OnSoundOutDevice(QAction*);
    void OnSoundSampleRate(QAction*);
    void OnSoundSignalUpscale(bool);
    void OnSoundFileChanged(QString);
    // slots connected to signals from receiver
    void OnSoundInDeviceChanged(QString);
    void OnSoundOutDeviceChanged(QString);
    void OnSoundInSampleRateChanged(int);
    void OnSoundOutSampleRateChanged(int);
    void OnSoundUpscaleRatioChanged(int);
    void OnSoundInChannelChanged(int chan);
    void OnSoundOutChannelChanged(int chan);

signals:
    void soundSampleRateChanged(int);
    void soundInDeviceChanged(QString);
    void soundOutDeviceChanged(QString);
    void soundInChannelChanged(int);
    void soundOutChannelChanged(EOutChanSel);
    void soundSignalUpscaleChanged(int);
};

class CFileMenu : public QMenu
{
    Q_OBJECT

public:
    CFileMenu(CTRx& ntrx, QMainWindow* parent, QMenu* menuInsertBefore);

protected:
    CTRx&               trx;
    QAction*			actionOpenFile;
    QAction*			actionCloseFile;

public slots:
    void OnOpenFile();
    void OnCloseFile();

signals:
    void soundFileChanged(QString);
};

#endif
