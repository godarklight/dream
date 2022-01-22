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
#include "alsacommon.h"
#include <alsa/asoundlib.h>
using namespace std;

string checkName(vector<string> names, string sCurrentDevice)
{
    if(names.size()==0) {
        return sCurrentDevice;
    }

    /* might be invalid due to command line parameter or USB device unplugged */
    string sDefault = "";
    for(size_t i=0; i<names.size(); i++) {
        if(names[i] == sCurrentDevice) return sCurrentDevice;
        if(names[i].find("default") != string::npos) sDefault = names[i];
    }
    if(sDefault == "") return names[0];
    return sDefault;
}

snd_pcm_t* Init_hw(snd_pcm_uframes_t period_size, int iSampleRate, string name, snd_pcm_stream_t direction)
{

    int err, dir;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_t* handle;

    err = snd_pcm_open( &handle, name.c_str(), direction, 0);
    if ( err != 0)
    {
        qDebug("open error: %s", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW playback, can't open "+name);
    }

    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_sw_params_alloca(&swparams);

    /* Choose all parameters */
    err = snd_pcm_hw_params_any(handle, hwparams);
    if (err < 0) {
        qDebug("Broken configuration : no configurations available: %s", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    /* Set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);

    if (err < 0) {
        qDebug("Access type not available : %s", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");

    }
    /* Set the sample format */
    err = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
        qDebug("Sample format not available : %s", snd_strerror(err));
        throw CGenErr(string("alsa CSoundOut::Init_HW ")+snd_strerror(err));
    }
    /* Set the count of channels */
    err = snd_pcm_hw_params_set_channels(handle, hwparams, NUM_OUT_CHANNELS);
    if (err < 0) {
        qDebug("Channels count (%i) not available s: %s", NUM_OUT_CHANNELS, snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    /* Set the stream rate */
    dir=0;
    err = snd_pcm_hw_params_set_rate(handle, hwparams, iSampleRate, dir);
    if (err < 0) {
        qDebug("Rate %iHz not available : %s", iSampleRate, snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    dir=0;
    unsigned int buffer_time = 500000;              /* ring buffer length in us */
    /* set the buffer time */
    err = snd_pcm_hw_params_set_buffer_time_near(handle, hwparams, &buffer_time, &dir);
    if (err < 0) {
        qDebug("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    err = snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size);
    if (err < 0) {
        qDebug("Unable to get buffer size for playback: %s\n", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    // qDebug("buffer size %d", buffer_size);
    /* set the period time */
    unsigned int period_time = 100000;              /* period time in us */
    err = snd_pcm_hw_params_set_period_time_near(handle, hwparams, &period_time, &dir);
    if (err < 0) {
        qDebug("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    err = snd_pcm_hw_params_get_period_size_min(hwparams, &period_size, &dir);
    if (err < 0) {
        qDebug("Unable to get period size for playback: %s\n", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    // qDebug("period size %d", period_size);

    /* Write the parameters to device */
    err = snd_pcm_hw_params(handle, hwparams);
    if (err < 0) {
        qDebug("Unable to set hw params : %s", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    /* Get the current swparams */
    err = snd_pcm_sw_params_current(handle, swparams);
    if (err < 0) {
        qDebug("Unable to determine current swparams : %s", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    /* Write the parameters to the playback device */
    err = snd_pcm_sw_params(handle, swparams);
    if (err < 0) {
        qDebug("Unable to set sw params : %s", snd_strerror(err));
        throw CGenErr("alsa CSoundOut::Init_HW ");
    }
    snd_pcm_start(handle);
    qDebug("alsa init done");
    return handle;
}

int write_hw(snd_pcm_t* handle, _SAMPLE *playbuf, snd_pcm_uframes_t size )
{

    int start = 0;
    int ret;

    while (size) {

        ret = snd_pcm_writei(handle, &playbuf[int(start)], size );
        if (ret < 0) {
            if (ret ==  -EAGAIN) {
                if ((ret = snd_pcm_wait (handle, 100)) < 0) {
                    qDebug ("poll failed (%s)", snd_strerror (ret));
                    break;
                }
                continue;
            } else if (ret == -EPIPE) {   /* under-run */
                qDebug("underrun");
                ret = snd_pcm_prepare(handle);
                if (ret < 0)
                    qDebug("Can't recover from underrun, prepare failed: %s", snd_strerror(ret));
                continue;
            } else if (ret == -ESTRPIPE) {
                qDebug("strpipe");
                while ((ret = snd_pcm_resume(handle)) == -EAGAIN)
                    sleep(1);       /* wait until the suspend flag is released */
                if (ret < 0) {
                    ret = snd_pcm_prepare(handle);
                    if (ret < 0)
                        qDebug("Can't recover from suspend, prepare failed: %s", snd_strerror(ret));
                }
                continue;
            } else {
                qDebug("Write error: %s", snd_strerror(ret));
                throw CGenErr("Write error");
            }
            break;  /* skip one period */
        }
        size -= ret;
        start += ret;
    }
    return 0;
}

void close_hw(snd_pcm_t* handle)
{

    if (handle != nullptr)
        snd_pcm_close( handle );
}

void enumerate(vector<string>& names, vector<string>& descriptions, string& defaultDevice, snd_pcm_stream_t direction)
{
    names.resize(0);
    descriptions.resize(0);

    char **hints;
    int err = snd_device_name_hint(-1, "pcm", (void***)&hints);
    if (err != 0)
       return;//Error! Just return

    char** n = hints;
    while (*n != nullptr) {

        char *ioid = snd_device_name_get_hint(*n, "IOID");
        char *name = snd_device_name_get_hint(*n, "NAME");
        char *desc = snd_device_name_get_hint(*n, "DESC");

        const char *d = (direction==SND_PCM_STREAM_PLAYBACK)?"Output":"Input";
	if(ioid==NULL) {
            descriptions.push_back(desc);
            names.push_back(name);
	}
	else if (0 == strcmp(d, ioid)) {
            descriptions.push_back(desc);
            names.push_back(name);
	}
        n++;
    }

    snd_device_name_free_hint((void**)hints);

    if(descriptions.size()==0) {
        return;
    }

    // set default as device with shortest description
    defaultDevice = names[0];
    string d = descriptions[0];
    for(size_t i=1; i<descriptions.size(); i++) {
        if(d.length()>descriptions[i].length()) {
            d = descriptions[i];
            defaultDevice = names[i];
        }
    }
}
