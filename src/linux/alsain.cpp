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

#include "alsain.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

/*****************************************************************************/

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

CSoundInAlsa::CSoundInAlsa():
    iBufferSize(0), iInBufferSize(0), tmprecbuf(nullptr), bBlockingRec(false),
    bChangDev(true), sCurrentDevice(""),iSampleRate(48000),handle(nullptr)
{
    RecThread.pSoundIn = this;
}

void CSoundInAlsa::Enumerate(vector<string>& choices, vector<string>& descriptions, string& defaultInput)
{
    enumerate(choices, descriptions, defaultInput, SND_PCM_STREAM_CAPTURE);
}

void
CSoundInAlsa::CRecThread::run()
{
    while (SoundBuf.keep_running) {

        int fill;

        SoundBuf.lock();
        fill = SoundBuf.GetFillLevel();
        SoundBuf.unlock();

        if (  (SOUNDBUFLEN - fill) > (FRAGSIZE * NUM_IN_CHANNELS) ) {
            // enough space in the buffer

            int size = pSoundIn->read_HW( tmprecbuf, FRAGSIZE);

            // common code
            if (size > 0) {
                CVectorEx<_SAMPLE>* ptarget;

                /* Copy data from temporary buffer in output buffer */
                SoundBuf.lock();

                ptarget = SoundBuf.QueryWriteBuffer();

                for (int i = 0; i < size * NUM_IN_CHANNELS; i++)
                    (*ptarget)[i] = tmprecbuf[i];

                SoundBuf.Put( size * NUM_IN_CHANNELS );
                SoundBuf.unlock();
            }
        } else {
            msleep( 1 );
        }
    }
    qDebug("Rec Thread stopped");
}


/* Wave in ********************************************************************/

bool CSoundInAlsa::Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    qDebug("initrec %d", iNewBufferSize);

    this->iSampleRate = iSampleRate;

    /* Save < */
    RecThread.SoundBuf.lock();
    iInBufferSize = iNewBufferSize;
    bBlockingRec = bNewBlocking;
    RecThread.SoundBuf.unlock();

    /* Check if device must be opened or reinitialized */
    if (bChangDev)
    {

        Init_HW( );

        /* Reset flag */
        bChangDev = false;
    }

    if ( RecThread.isRunning() == false ) {
        RecThread.SoundBuf.lock();
        RecThread.SoundBuf.Init( SOUNDBUFLEN );
        RecThread.SoundBuf.unlock();
        RecThread.start();
    }
    return true;
}


bool CSoundInAlsa::Read(CVector<short>& psData, CParameter& Parameters)
{
    CVectorEx<_SAMPLE>* p;

    /* Check if device must be opened or reinitialized */
    if (bChangDev)
    {
        /* Reinit sound interface */
        Init(iSampleRate, iBufferSize, bBlockingRec);

        /* Reset flag */
        bChangDev = false;
    }

    RecThread.SoundBuf.lock();  // we need exclusive access

    while ( RecThread.SoundBuf.GetFillLevel() < iInBufferSize ) {


        // not enough data, sleep a little
        RecThread.SoundBuf.unlock();
        usleep(1000); //1ms
        RecThread.SoundBuf.lock();
    }

    // copy data

    p = RecThread.SoundBuf.Get( iInBufferSize );
    for (int i=0; i<iInBufferSize; i++)
        psData[i] = (*p)[i];

    RecThread.SoundBuf.unlock();

    return false;
}

void CSoundInAlsa::Close()
{
    qDebug("stoprec");

    // stop the recording threads

    if (RecThread.isRunning() ) {
        RecThread.SoundBuf.keep_running = false;
        // wait 1sec max. for the threads to terminate
        RecThread.wait(1000);
    }

    close_HW();

    /* Set flag to open devices the next time it is initialized */
    bChangDev = true;
}

void CSoundInAlsa::SetDev(string sNewDevice)
{
    /* Change only in case new device id is not already active */
    if (sNewDevice != sCurrentDevice)
    {
        sCurrentDevice = sNewDevice;
        bChangDev = true;
    }
}


string CSoundInAlsa::GetDev()
{
    return sCurrentDevice;
}

void CSoundInAlsa::Init_HW() {

    if (handle != NULL)
        return;

    int err, dir=0;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;
    snd_pcm_uframes_t period_size = FRAGSIZE * NUM_IN_CHANNELS/2;
    snd_pcm_uframes_t buffer_size;

    vector<string> names;
    vector<string> descriptions;
    string def;
    Enumerate(names, descriptions, def);
    /* Default ? */
    if (sCurrentDevice == "")
    {
        sCurrentDevice = def;
    }
    /* might be invalid due to command line parameter or USB device unplugged */
    bool found = false;
    for(size_t i=0; i<names.size(); i++) {
        if(names[i] == sCurrentDevice) found = true;
    }
    if(!found) sCurrentDevice = names[names.size()-1];
    err = snd_pcm_open( &handle, sCurrentDevice.c_str(), SND_PCM_STREAM_CAPTURE, 0 );
    if ( err != 0)
    {
        qDebug("open error: %s", snd_strerror(err));
        throw CGenErr("alsa CSoundInAlsa::Init_HW record, can't open "+sCurrentDevice);
    }

    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_sw_params_alloca(&swparams);

    /* Choose all parameters */
    err = snd_pcm_hw_params_any(handle, hwparams);
    if (err < 0) {
        qDebug("Broken configuration : no configurations available: %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);

    if (err < 0) {
        qDebug("Access type not available : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));

    }
    /* Set the sample format */
    err = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
        qDebug("Sample format not available : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Set the count of channels */
    err = snd_pcm_hw_params_set_channels(handle, hwparams, NUM_IN_CHANNELS);
    if (err < 0) {
        qDebug("Channels count (%i) not available s: %s", NUM_IN_CHANNELS, snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Set the stream rate */
    dir=0;
    err = snd_pcm_hw_params_set_rate(handle, hwparams, iSampleRate, dir);
    if (err < 0) {
        qDebug("Rate %iHz not available : %s", iSampleRate, snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    dir=0;
    unsigned int buffer_time = 500000;              /* ring buffer length in us */
    /* set the buffer time */
    err = snd_pcm_hw_params_set_buffer_time_near(handle, hwparams, &buffer_time, &dir);
    if (err < 0) {
        qDebug("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    err = snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size);
    if (err < 0) {
        qDebug("Unable to get buffer size for playback: %s\n", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    // qDebug("buffer size %d", buffer_size);
    /* set the period time */
    unsigned int period_time = 100000;              /* period time in us */
    err = snd_pcm_hw_params_set_period_time_near(handle, hwparams, &period_time, &dir);
    if (err < 0) {
        qDebug("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    err = snd_pcm_hw_params_get_period_size_min(hwparams, &period_size, &dir);
    if (err < 0) {
        qDebug("Unable to get period size for playback: %s\n", snd_strerror(err));
        throw CGenErr(string("alsa CSoundIn::Init_HW ")+snd_strerror(err));
    }
    // qDebug("period size %d", period_size);

    /* Write the parameters to device */
    err = snd_pcm_hw_params(handle, hwparams);
    if (err < 0) {
        qDebug("Unable to set hw params : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Get the current swparams */
    err = snd_pcm_sw_params_current(handle, swparams);
    if (err < 0) {
        qDebug("Unable to determine current swparams : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Start the transfer when the buffer immediately */
    err = snd_pcm_sw_params_set_start_threshold(handle, swparams, 0);
    if (err < 0) {
        qDebug("Unable to set start threshold mode : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Allow the transfer when at least period_size samples can be processed */
    err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_size);
    if (err < 0) {
        qDebug("Unable to set avail min : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    /* Write the parameters to the record/playback device */
    err = snd_pcm_sw_params(handle, swparams);
    if (err < 0) {
        qDebug("Unable to set sw params : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundInAlsa::Init_HW ")+snd_strerror(err));
    }
    snd_pcm_start(handle);
    qDebug("alsa init done");

}

int CSoundInAlsa::read_HW( void * recbuf, int size) {

    int ret = snd_pcm_readi(handle, recbuf, size);


    if (ret < 0)
    {
        if (ret == -EPIPE)
        {
            qDebug("rpipe");
            /* Under-run */
            qDebug("rprepare");
            ret = snd_pcm_prepare(handle);
            if (ret < 0)
                qDebug("Can't recover from underrun, prepare failed: %s", snd_strerror(ret));

            ret = snd_pcm_start(handle);

            if (ret < 0)
                qDebug("Can't recover from underrun, start failed: %s", snd_strerror(ret));
            return 0;

        }
        else if (ret == -ESTRPIPE)
        {
            qDebug("strpipe");

            /* Wait until the suspend flag is released */
            while ((ret = snd_pcm_resume(handle)) == -EAGAIN)
                sleep(1);

            if (ret < 0)
            {
                ret = snd_pcm_prepare(handle);

                if (ret < 0)
                    qDebug("Can't recover from suspend, prepare failed: %s", snd_strerror(ret));
                throw CGenErr("CSound:Read");
            }
            return 0;
        }
        else
        {
            qDebug("CSoundInAlsa::Read: %s", snd_strerror(ret));
            throw CGenErr("CSound:Read");
        }
    } else
        return ret;

}

void CSoundInAlsa::close_HW( void ) {

    if (handle != NULL)
        snd_pcm_close( handle );

    handle = NULL;
}
