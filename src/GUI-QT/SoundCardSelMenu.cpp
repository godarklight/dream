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
#include "../Parameter.h"
#include "../DataIO.h"
#include "../DrmReceiver.h"
#include "../DrmTransmitter.h"
#include "SoundCardSelMenu.h"
#include "DialogUtil.h"
#include <QFileDialog>
#include "../util-QT/Util.h"
#include "../util/FileTyper.h"
#include "../main-Qt/crx.h"

#ifdef QT_MULTIMEDIA_LIB
# include <QAudioDeviceInfo>
#endif

#ifdef HAVE_LIBPCAP
# define PCAP_FILES " *.pcap"
#else
# define PCAP_FILES ""
#endif
#ifdef HAVE_LIBSNDFILE
# define SND_FILES "*.aif* *.au *.flac *.ogg *.rf64 *.snd *.wav"
#else
# define SND_FILES "*.if* *.iq* *.pcm* *.txt"
#endif
#define SND_FILE1 SND_FILES " "
#define SND_FILE2 "Sound Files (" SND_FILES ");;"
#define RSCI_FILES "*.rsA *.rsB *.rsC *.rsD *.rsQ *.rsM" PCAP_FILES
#define RSCI_FILE1 RSCI_FILES " "
#define RSCI_FILE2 "MDI/RSCI Files (" RSCI_FILES ");;"


static const CHANSEL InputChannelTable[] =
{
    { "Left Channel",  CS_LEFT_CHAN    },
    { "Right Channel", CS_RIGHT_CHAN   },
    { "L + R",         CS_MIX_CHAN     },
    { "L - R",         CS_SUB_CHAN     },
    { "I/Q Pos",       CS_IQ_POS       },
    { "I/Q Neg",       CS_IQ_NEG       },
    { "I/Q Pos Zero",  CS_IQ_POS_ZERO  },
    { "I/Q Neg Zero",  CS_IQ_NEG_ZERO  },
    { "I/Q Pos Split", CS_IQ_POS_SPLIT },
    { "I/Q Neg Split", CS_IQ_NEG_SPLIT },
    { nullptr, 0 } /* end of list */
};

static const CHANSEL OutputChannelTable[] =
{
    { "Both Channels",              CS_BOTH_BOTH   },
    { "Left -> Left, Right Muted",  CS_LEFT_LEFT   },
    { "Right -> Right, Left Muted", CS_RIGHT_RIGHT },
    { "L + R -> Left, Right Muted", CS_LEFT_MIX    },
    { "L + R -> Right, Left Muted", CS_RIGHT_MIX   },
    { nullptr, 0 } /* end of list */
};

static const int AudioSampleRateTable[] =
{
    11025, 22050, 24000, 44100, 48000, 96000, 192000, 0
};

static const int SignalSampleRateTable[] =
{
    -24000, -48000, -96000, -192000, 0
};


/* Implementation *************************************************************/

/* CSoundCardSelMenu **********************************************************/

CSoundCardSelMenu::CSoundCardSelMenu(CTRx& ntrx,
    CFileMenu* pFileMenu, QWidget* parent) : QMenu(parent),
    trx(ntrx),
    menuSigInput(nullptr), menuInputDev(nullptr),
    menuInputSampleRate(nullptr),menuOutputSampleRate(nullptr),
    menuOutputDev(nullptr),
    bReceiver(trx.IsReceiver())
{
    setTitle(tr("Sound Card"));


    if (bReceiver)
    {   /* Receiver */
        menuSigInput = addMenu(tr("Signal Input"));
        menuInputDev = menuSigInput->addMenu(tr("Device"));
        connect(menuInputDev, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundInDevice(QAction*)));

        QMenu* menuAudOutput = addMenu(tr("Audio Output"));
        menuOutputDev = menuAudOutput->addMenu(tr("Device"));

        menuInputChannel = InitChannel(menuSigInput, tr("Channel"), InputChannelTable);
        menuOutputChannel = InitChannel(menuAudOutput, tr("Channel"), OutputChannelTable);
        menuInputSampleRate = InitSampleRate(menuSigInput, tr("Sample Rate"), SignalSampleRateTable);

        connect(menuInputChannel, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundInChannel(QAction*)));
        connect(menuOutputChannel, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundOutChannel(QAction*)));
        connect(menuInputSampleRate, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundSampleRate(QAction*)));
        menuOutputSampleRate = InitSampleRate(menuAudOutput, tr("Sample Rate"), AudioSampleRateTable);
        connect(menuOutputSampleRate, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundSampleRate(QAction*)));
        actionUpscale = menuSigInput->addAction(tr("2:1 upscale"));
        connect(actionUpscale, SIGNAL(toggled(bool)), this, SLOT(OnSoundSignalUpscale(bool)));
        connect(this, SIGNAL(soundInDeviceChanged(QString)), &trx, SLOT(SetInputDevice(QString)));
        connect(this, SIGNAL(soundSampleRateChanged(int)), &trx, SLOT(onSoundSampleRateChanged(int)));
        connect(this, SIGNAL(soundInDeviceChanged(QString)), &trx, SLOT(SetInputDevice(QString)));
        connect(this, SIGNAL(soundInChannelChanged(int)), &trx, SLOT(onSoundInChannelChanged(int)));
        connect(this, SIGNAL(soundOutChannelChanged(EOutChanSel)), &trx, SLOT(onSoundOutChannelChanged(EOutChanSel)));
        connect(this, SIGNAL(soundSignalUpscaleChanged(int)), &trx, SLOT(SetSoundSignalUpscale(int)));

        connect(reinterpret_cast<CRx*>(&trx), SIGNAL(InputChannelChanged(int)), this, SLOT(OnSoundInChannelChanged(int)));
        connect(reinterpret_cast<CRx*>(&trx), SIGNAL(OutputChannelChanged(int)), this, SLOT(OnSoundOutChannelChanged(int)));

        connect(&trx, SIGNAL(InputDeviceChanged(QString)), this, SLOT(OnSoundInDeviceChanged(QString)));

        connect(&trx, SIGNAL(inputSampleRateChanged(int)), this, SLOT(OnSoundInSampleRateChanged(int)));
        connect(&trx, SIGNAL(outputSampleRateChanged(int)), this, SLOT(OnSoundOutSampleRateChanged(int)));

        connect(&trx, SIGNAL(soundUpscaleRatioChanged(int)), this, SLOT(OnSoundUpscaleRatioChanged(int)));

        if (pFileMenu != nullptr) {
            connect(pFileMenu, SIGNAL(soundFileChanged(QString)), this, SLOT(OnSoundFileChanged(QString)));
        }
    }
    else
    {   /* Transmitter */    
        QMenu* menuAudio = addMenu(tr("Audio Input"));
        menuInputDev = menuAudio->addMenu(tr("Device"));
        connect(menuInputDev, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundInDevice(QAction*)));

        QMenu* menuSignal = addMenu(tr("Signal Output"));
        menuOutputDev = menuSignal->addMenu(tr("Device"));

        connect(InitSampleRate(menuAudio, tr("Sample Rate"), AudioSampleRateTable), SIGNAL(triggered(QAction*)), this, SLOT(OnSoundSampleRate(QAction*)));
        connect(InitSampleRate(menuSignal, tr("Sample Rate"), SignalSampleRateTable), SIGNAL(triggered(QAction*)), this, SLOT(OnSoundSampleRate(QAction*)));
    }

    connect(menuOutputDev, SIGNAL(triggered(QAction*)), this, SLOT(OnSoundOutDevice(QAction*)));
    connect(this, SIGNAL(soundOutDeviceChanged(QString)), &trx, SLOT(SetOutputDevice(QString)));
    connect(&trx, SIGNAL(OutputDeviceChanged(QString)), this, SLOT(OnSoundOutDeviceChanged(QString)));
}

void CSoundCardSelMenu::OnSoundInDevice(QAction* action)
{
    emit soundInDeviceChanged(action->data().toString());
}

void CSoundCardSelMenu::OnSoundOutDevice(QAction* action)
{
    QString dev = action->data().toString();
    cerr << "OnSoundOutDevice " << dev.toStdString() << endl;
    emit soundOutDeviceChanged(dev);
}

void CSoundCardSelMenu::OnSoundInChannel(QAction* action)
{
    emit soundInChannelChanged(action->data().toInt());
}

void CSoundCardSelMenu::OnSoundOutChannel(QAction* action)
{
    emit soundOutChannelChanged(EOutChanSel(action->data().toInt()));
}

void CSoundCardSelMenu::OnSoundSampleRate(QAction* action)
{
    emit soundSampleRateChanged(EOutChanSel(action->data().toInt()));
}

void CSoundCardSelMenu::OnSoundSignalUpscale(bool bChecked)
{
    emit soundSignalUpscaleChanged(bChecked ? 2 : 1);
}

void CSoundCardSelMenu::OnSoundInDeviceChanged(QString s)
{
    vector<string> names;
    vector<string> descriptions;
    string def;
    trx.EnumerateInputs(names, descriptions, def);
    cerr << "input device changed in trx " << s.toStdString() << " soundcard default is " << def << endl;
    UpdateDeviceMenu(menuInputDev, names, descriptions, s.toStdString());
}

void CSoundCardSelMenu::OnSoundOutDeviceChanged(QString s)
{
    cerr << "CSoundCardSelMenu::OnSoundOutDeviceChanged " << s.toStdString() << endl;
    vector<string> names;
    vector<string> descriptions;
    string def;
    trx.EnumerateOutputs(names, descriptions, def);
    cerr << "output device changed in trx " << s.toStdString() << " soundcard default is " << def << endl;
    UpdateDeviceMenu(menuOutputDev, names, descriptions, s.toStdString());
}

void CSoundCardSelMenu::UpdateDeviceMenu(QMenu* menu, const vector<string>& names, const vector<string>& descriptions, const string& selected)
{
    menu->clear();
    QActionGroup* group = nullptr;
    cerr << "UpdateDeviceMenu " << menu->title().toStdString() << " selected (" << selected << ")" << endl;
    for (int i = 0; i < int(names.size()); i++)
    {
      cerr << "enum " << names[i] << " desc " <<  descriptions[i] << endl;
        QString name(QString::fromStdString(names[i]));
        QString desc(QString::fromStdString(descriptions[i]));
        if(name.size()==0) {
		name = tr("[default]");
	}
        QString t = name;
        if(desc.size()>0) t += " [" + desc + "]";
        QAction* m = menu->addAction(t);
        m->setData(name);
        m->setCheckable(true);
        if (names[i] == selected)
            m->setChecked(true);
        if (group == nullptr)
            group = new QActionGroup(m);
        group->addAction(m);
    }
}

QMenu* CSoundCardSelMenu::InitChannel(QMenu* parent, const QString& text, const CHANSEL* ChanSel)
{
    QMenu* menu = parent->addMenu(text);
    QActionGroup* group = new QActionGroup(parent);
    for (int i = 0; ChanSel[i].Name; i++)
    {
        QAction* m = menu->addAction(tr(ChanSel[i].Name));
        int iChanSel = ChanSel[i].iChanSel;
        m->setData(iChanSel);
        m->setCheckable(true);
        group->addAction(m);
    }
    return menu;
}

void CSoundCardSelMenu::OnSoundInChannelChanged(int chan)
{
    QList<QAction*> actions = menuInputChannel->actions();
    for(int i=0; i<actions.size(); i++) {
        int n = actions[i]->data().toInt();
        if(abs(n) == chan)
            actions[i]->setChecked(true);
    }
}

void CSoundCardSelMenu::OnSoundOutChannelChanged(int chan)
{
    QList<QAction*> actions = menuOutputChannel->actions();
    for(int i=0; i<actions.size(); i++) {
        int n = actions[i]->data().toInt();
        if(abs(n) == chan)
            actions[i]->setChecked(true);
    }
}

QMenu* CSoundCardSelMenu::InitSampleRate(QMenu* parent, const QString& text, const int* SampleRate)
{
    QMenu* menu = parent->addMenu(text);
    QActionGroup* group = new QActionGroup(parent);
    for (int i = 0; SampleRate[i]; i++)
    {
        const int iSampleRate = SampleRate[i];
        const int iAbsSampleRate = abs(iSampleRate);
        QAction* m = menu->addAction(QString::number(iAbsSampleRate) + tr(" Hz"));
        m->setData(iSampleRate);
        m->setCheckable(true);
        group->addAction(m);
    }
    return menu;
}

void CSoundCardSelMenu::OnSoundInSampleRateChanged(int sr)
{
    if (bReceiver) {
        QList<QAction*> actions = menuInputSampleRate->actions();
        for(int i=0; i<actions.size(); i++) {
            int n = actions[i]->data().toInt();
            if(abs(n) == sr)
                actions[i]->setChecked(true);
        }
    }
    else {

    }
}

void CSoundCardSelMenu::OnSoundOutSampleRateChanged(int sr)
{
    if (bReceiver) {
        QList<QAction*> actions = menuOutputSampleRate->actions();
        for(int i=0; i<actions.size(); i++) {
            int n = actions[i]->data().toInt();
            if(abs(n) == sr)
                actions[i]->setChecked(true);
        }
    }
    else {

    }
}

void CSoundCardSelMenu::OnSoundUpscaleRatioChanged(int upscaleRatio)
{
    actionUpscale->setChecked(upscaleRatio == 2);
}

void CSoundCardSelMenu::OnSoundFileChanged(QString filename)
{
    if(filename == "") {
        menuSigInput->setEnabled(true);
        menuInputDev->setEnabled(true);
        menuInputSampleRate->setEnabled(true);
    }
    else {
        FileTyper::type t = FileTyper::resolve(filename.toStdString());
        switch(t) {
        case FileTyper::unrecognised:
            menuSigInput->setEnabled(true);
            menuInputDev->setEnabled(true);
            menuInputSampleRate->setEnabled(true);
            break;
        case FileTyper::pcap:
        case FileTyper::file_framing:
        case FileTyper::raw_af:
        case FileTyper::raw_pft:
        case FileTyper::pcm:
            menuSigInput->setEnabled(false);
            menuInputDev->setEnabled(false);
            menuInputSampleRate->setEnabled(false);
        }
    }
    emit soundInDeviceChanged(filename);
}

/* CFileMenu ******************************************************************/
// TODO DRMTransmitter

CFileMenu::CFileMenu(CTRx& ntrx, QMainWindow* parent,
    QMenu* menuInsertBefore)
    : QMenu(parent), trx(ntrx)
{
    setTitle(tr("&File"));
    if (trx.IsReceiver())
    {
        QString openFile(tr("&Open File..."));
        QString closeFile(tr("&Close File"));
        actionOpenFile = addAction(openFile, this, SLOT(OnOpenFile()), QKeySequence(tr("Alt+O")));
        actionCloseFile = addAction(closeFile, this, SLOT(OnCloseFile()), QKeySequence(tr("Alt+C")));
        addSeparator();
    }
    addAction(tr("&Exit"), parent, SLOT(close()), QKeySequence(tr("Alt+X")));
    parent->menuBar()->insertMenu(menuInsertBefore->menuAction(), this);
}


void CFileMenu::OnOpenFile()
{
#define FILE_FILTER \
	"Supported Files (" \
	SND_FILE1 \
	RSCI_FILE1 \
	");;" \
	SND_FILE2 \
	RSCI_FILE2 \
	"All Files (*)"
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr(FILE_FILTER));
    /* Check if user hit the cancel button */
    if (!filename.isEmpty())
    {
        emit soundFileChanged(filename);
        actionCloseFile->setEnabled(true);
    }
}

void CFileMenu::OnCloseFile()
{
    emit soundFileChanged("");
    actionCloseFile->setEnabled(false);
}
