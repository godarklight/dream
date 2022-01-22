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


#ifndef _ALSA_COMMON_H
#define _ALSA_COMMON_H

#ifdef QT_CORE_LIB
# include <qmutex.h>
# include <qthread.h>
#endif
#include "../util/Buffer.h"
#include <alsa/asoundlib.h>

/* Definitions ****************************************************************/
#define NUM_IN_CHANNELS         2       /* Stereo recording (but we only
use one channel for recording) */
#define NUM_OUT_CHANNELS        2       /* Stereo Playback */
#define BITS_PER_SAMPLE         16      /* Use all bits of the D/A-converter */
#define BYTES_PER_SAMPLE        2       /* Number of bytes per sample */

class CSoundBuf : public CCyclicBuffer<_SAMPLE> {

public:
    CSoundBuf() : keep_running(true)
#ifdef QT_CORE_LIB
        , data_accessed()
#endif
    {}
    bool keep_running;
#ifdef QT_CORE_LIB
    void lock () {
        data_accessed.lock();
    }
    void unlock () {
        data_accessed.unlock();
    }

protected:
    QMutex  data_accessed;
#else
    void lock () { }
    void unlock () { }
#endif
};

#ifdef QT_CORE_LIB
typedef QThread CThread;
#else
class CThread {
public:
    void run() {}
    void start() {}
    void wait(int) {}
    void msleep(int) {}
    bool isRunning() {
        return true;
    }
};
#endif

void enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice, snd_pcm_stream_t direction);
std::string checkName(std::vector<std::string> names, std::string sCurrentDevice);
snd_pcm_t* Init_hw(snd_pcm_uframes_t period_size, int iSampleRate, std::string name, snd_pcm_stream_t direction);
void close_hw(snd_pcm_t* handle);
int write_hw(snd_pcm_t* handle, _SAMPLE *playbuf, snd_pcm_uframes_t size );

#endif
