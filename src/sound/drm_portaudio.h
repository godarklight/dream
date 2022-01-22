/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable
 *
 * Decription:
 * PortAudio sound interface
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

#ifndef _DRM_PORTAUDIO_H
#define _DRM_PORTAUDIO_H

#include "../sound/soundinterface.h"
#include <portaudio.h>
#include "pa_ringbuffer.h"

class CPaCommon: public CSelectionInterface
{
public:
    CPaCommon(bool);
    virtual 		~CPaCommon();

    virtual void	Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice);
    virtual void	SetDev(std::string sNewDevice);
    virtual std::string	GetDev();

    bool		Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking);
    void			ReInit();
    bool		Read(CVector<short>& psData);
    bool		Write(CVector<short>& psData);
    void			Close();

    PaUtilRingBuffer ringBuffer;
    int xruns;

protected:

    PaStream *stream;
    std::vector<std::string> names;
    std::vector<PaDeviceIndex> devices;
    std::string dev;
    bool is_capture,blocking,device_changed,xrun;
    int framesPerBuffer;
    int iBufferSize;
    char *ringBufferData;
    double samplerate;

    static int pa_count;
};

class CPaIn: public CSoundInInterface
{
public:
    CPaIn();
    virtual 		~CPaIn();
    virtual void Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice)
    {
        hw.Enumerate(names, descriptions);
    }
    virtual void	SetDev(std::string sNewDevice)
    {
        hw.SetDev(sNewDevice);
    }
    virtual std::string	GetDev()
    {
        return hw.GetDev();
    }
    virtual std::string	GetVersion()
    {
        return Pa_GetVersionInfo()->versionText;
    }

    virtual bool	Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking);
    virtual void		Close();
    virtual bool	Read(CVector<short>& psData);

protected:

    CPaCommon hw;
};

class CPaOut: public CSoundOutInterface
{
public:
    CPaOut();
    virtual 			~CPaOut();
    virtual void	Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultOutput)
    {
        hw.Enumerate(names, descriptions);
    }
    virtual void	SetDev(std::string sNewDevice)
    {
        hw.SetDev(sNewDevice);
    }
    virtual std::string	GetDev()
    {
        return hw.GetDev();
    }
    virtual std::string	GetVersion()
    {
        return Pa_GetVersionInfo()->versionText;
    }
    virtual bool	Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking);
    virtual void		Close();
    virtual bool	Write(CVector<short>& psData);

protected:

    CPaCommon hw;
};

#endif
