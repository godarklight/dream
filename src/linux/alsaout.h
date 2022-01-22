/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2014
 *
 * Author(s):
 *  Alexander Kurpiers
 *
 * Decription:
 * Linux sound interface
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

#ifndef _SOUNDOUT_H
#define _SOUNDOUT_H

#include "../sound/soundinterface.h"
#include "../util/Buffer.h"
#include "alsacommon.h"

/* Definitions ****************************************************************/
#define SOUNDBUFLEN 102400
#define FRAGSIZE 8192
//#define FRAGSIZE 1024

/* Classes ********************************************************************/
class CSoundOutAlsa : public CSoundOutInterface
{
public:
    CSoundOutAlsa();
    virtual ~CSoundOutAlsa() {}

    virtual void Enumerate(std::vector<std::string>&, std::vector<std::string>&, std::string&);
    virtual void SetDev(std::string sNewDevice);
    virtual std::string GetDev();

    bool Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking = false);
    bool Write(CVector<short>& psData);

    void Close();
	virtual std::string		GetVersion() { return "alsa audio output"; }

protected:
    void Init_HW();
    int write_HW( _SAMPLE *playbuf, int size );
    void close_HW();

    class CPlayThread : public CThread
    {
    public:
        virtual ~CPlayThread() {}
        virtual void run();
        CSoundBuf SoundBuf;
        CSoundOutAlsa*  pSoundOut;
    protected:
        _SAMPLE tmpplaybuf[NUM_OUT_CHANNELS * FRAGSIZE];
    } PlayThread;

    int     iBufferSize, iInBufferSize;
    short int *tmpplaybuf;
    bool    bBlockingPlay;
    bool bChangDev;
    std::string sCurrentDevice;
    int iSampleRate;
    snd_pcm_t *handle;
};

#endif
