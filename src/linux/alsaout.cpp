/******************************************************************************\
* Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
* Copyright (c) 2001-2014
*
* Author(s):
*   Alexander Kurpiers
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

#include "alsaout.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

/*****************************************************************************/

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#include <alsa/asoundlib.h>

CSoundOutAlsa::CSoundOutAlsa():
    iBufferSize(0), iInBufferSize(0), tmpplaybuf(nullptr), bBlockingPlay(false),
    bChangDev(true), sCurrentDevice(""), iSampleRate(48000),handle(nullptr)
{
    PlayThread.pSoundOut = this;
}

void CSoundOutAlsa::Enumerate(vector<string>& choices, vector<string>& descriptions, string& defaultOutput)
{
    enumerate(choices, descriptions, defaultOutput, SND_PCM_STREAM_PLAYBACK);
}

void CSoundOutAlsa::CPlayThread::run()
{
    while ( SoundBuf.keep_running ) {
        int fill;

        SoundBuf.lock();
        fill = SoundBuf.GetFillLevel();
        SoundBuf.unlock();

        if ( fill > (FRAGSIZE * NUM_OUT_CHANNELS) ) {

            // enough data in the buffer

            CVectorEx<_SAMPLE>* p;

            SoundBuf.lock();
            p = SoundBuf.Get( FRAGSIZE * NUM_OUT_CHANNELS );

            for (int i=0; i < FRAGSIZE * NUM_OUT_CHANNELS; i++)
                tmpplaybuf[i] = (*p)[i];

            SoundBuf.unlock();

            pSoundOut->write_HW( tmpplaybuf, FRAGSIZE );

        } else {

            do {
                msleep( 1 );

                SoundBuf.lock();
                fill = SoundBuf.GetFillLevel();
                SoundBuf.unlock();

            } while ((SoundBuf.keep_running) && ( fill < SOUNDBUFLEN/2 ));  // wait until buffer is at least half full
        }
    }
    qDebug("Play Thread stopped");
}

bool CSoundOutAlsa::Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    qDebug("initplay %d", iNewBufferSize);

    this->iSampleRate = iSampleRate;

    /* Save buffer size */
    PlayThread.SoundBuf.lock();
    iBufferSize = iNewBufferSize;
    bBlockingPlay = bNewBlocking;
    PlayThread.SoundBuf.unlock();

    /* Check if device must be opened or reinitialized */
    if (bChangDev)
    {

        Init_HW( );

        /* Reset flag */
        bChangDev = false;
    }

    if ( PlayThread.isRunning() == false ) {
        PlayThread.SoundBuf.lock();
        PlayThread.SoundBuf.Init( SOUNDBUFLEN );
        PlayThread.SoundBuf.unlock();
        PlayThread.start();
    }

    return true;
}


bool CSoundOutAlsa::Write(CVector< _SAMPLE >& psData)
{
    /* Check if device must be opened or reinitialized */
    if (bChangDev)
    {
        /* Reinit sound interface */
        Init(iSampleRate, iBufferSize, bBlockingPlay);

        /* Reset flag */
        bChangDev = false;
    }

    if ( bBlockingPlay ) {
        // blocking write
        while ( PlayThread.SoundBuf.keep_running ) {
            PlayThread.SoundBuf.lock();
            int fill = SOUNDBUFLEN - PlayThread.SoundBuf.GetFillLevel();
            PlayThread.SoundBuf.unlock();
            if ( fill > iBufferSize) break;
        }
    }

    PlayThread.SoundBuf.lock(); // we need exclusive access

    if ( ( SOUNDBUFLEN - PlayThread.SoundBuf.GetFillLevel() ) > iBufferSize) {

        CVectorEx<_SAMPLE>* ptarget;

        // data fits, so copy
        ptarget = PlayThread.SoundBuf.QueryWriteBuffer();
        for (int i=0; i < iBufferSize; i++)
        {
            (*ptarget)[i] = psData[i];
        }

        PlayThread.SoundBuf.Put( iBufferSize );
    }

    PlayThread.SoundBuf.unlock();

    return false;
}

void CSoundOutAlsa::Close()
{
    qDebug("stopplay");

    // stop the playback thread
    if (PlayThread.isRunning() ) {
        PlayThread.SoundBuf.keep_running = false;
        PlayThread.wait(1000);
    }

    close_HW();

    /* Set flag to open devices the next time it is initialized */
    bChangDev = true;
}

void CSoundOutAlsa::SetDev(string sNewDevice)
{
    /* Change only in case new device id is not already active */
    if (sNewDevice != sCurrentDevice)
    {
        sCurrentDevice = sNewDevice;
        bChangDev = true;
    }
}

string CSoundOutAlsa::GetDev()
{
    return sCurrentDevice;
}

void CSoundOutAlsa::Init_HW()
{
    vector<string> names;
    vector<string> descriptions;
    string def;
    Enumerate(names, descriptions, def);
    sCurrentDevice = checkName(names, sCurrentDevice);
    if(sCurrentDevice == "") sCurrentDevice = def;
    handle = Init_hw(FRAGSIZE * NUM_OUT_CHANNELS/2, iSampleRate, sCurrentDevice, SND_PCM_STREAM_PLAYBACK);
}

int CSoundOutAlsa::write_HW(_SAMPLE *playbuf, int size )
{
    return write_hw(handle, playbuf, size);
}

void CSoundOutAlsa::close_HW()
{
    close_hw(handle);
    handle = nullptr;
}
