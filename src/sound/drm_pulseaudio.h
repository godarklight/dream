/******************************************************************************\
 *
 * Copyright (c) 2012-2013
 *
 * Author(s):
 *	David Flamand
 *
 * Decription:
 *  PulseAudio sound interface with clock drift adjustment (optional)
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

#ifndef DRM_PULSEAUDIO_H_INCLUDED
#define DRM_PULSEAUDIO_H_INCLUDED


/* Master switch */
//#define ENABLE_CLOCK_DRIFT_ADJ


#include <pulse/pulseaudio.h>
#include "../sound/soundinterface.h"
#if defined(PA_STREAM_VARIABLE_RATE) && defined(ENABLE_CLOCK_DRIFT_ADJ)
# define CLOCK_DRIFT_ADJ_ENABLED
#endif

#ifdef CLOCK_DRIFT_ADJ_ENABLED
# include "../matlib/MatlibSigProToolbox.h"
#endif

#ifndef _WIN32
# define ENABLE_STDIN_STDOUT
#endif


class CSoundOutPulse;
typedef struct pa_stream_notify_cb_userdata_t
{
	CSoundOutPulse*		SoundOutPulse;
	bool			bOverflow;
} pa_stream_notify_cb_userdata_t;

typedef struct pa_common
{
	bool		bClockDriftComp;
	int				sample_rate_offset;
} pa_common;

typedef struct pa_object
{
	pa_mainloop		*pa_m;
	pa_context		*pa_c;
	int				ref_count;
} pa_object;


/* Classes ********************************************************************/

class CSoundPulse
{
public:
	CSoundPulse(bool bPlayback);
    virtual ~CSoundPulse();
    void        Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice);
    void        SetDev(std::string sNewDevice);
    std::string GetDev();
protected:
	bool		bPlayback;
    std::string sCurrentDevice;
#ifdef ENABLE_STDIN_STDOUT
    bool        IsStdinStdout();
    bool        bStdinStdout;
#endif
};

class CSoundInPulse : public CSoundPulse, public CSoundInInterface
{
public:
	CSoundInPulse();
    virtual ~CSoundInPulse();
    void        Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice) {CSoundPulse::Enumerate(names, descriptions, defaultDevice);}
    std::string	GetDev() {return CSoundPulse::GetDev();}
    void        SetDev(std::string sNewDevice) {CSoundPulse::SetDev(sNewDevice);}
    std::string GetVersion() { return pa_get_library_version(); }

    bool        Init(int iNewSampleRate, int iNewBufferSize, bool bNewBlocking);
    bool        Read(CVector<_SAMPLE>& psData, CParameter &Parameters);
    void        Close();
#ifdef CLOCK_DRIFT_ADJ_ENABLED
    void        SetCommonParamPtr(pa_common *cp_ptr) { cp = cp_ptr; }
#endif

protected:
	void			Init_HW();
	int				Read_HW(void *recbuf, int size);
	void			Close_HW();
	void			SetBufferSize_HW();

	int				iSampleRate;
	int				iBufferSize;
    unsigned        timeToWait;
	bool		bBlockingRec;

	bool		bBufferingError;

	pa_stream		*pa_s;
	size_t			remaining_nbytes;
	const char		*remaining_data;

#ifdef CLOCK_DRIFT_ADJ_ENABLED
	int				record_sample_rate;
	bool		bClockDriftComp;
	pa_common		*cp;
#endif
};

class CSoundOutPulse : public CSoundPulse, public CSoundOutInterface
{
public:
	CSoundOutPulse();
    virtual ~CSoundOutPulse();
    void        Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice)
        {CSoundPulse::Enumerate(names, descriptions, defaultDevice);}
    std::string GetDev() {return CSoundPulse::GetDev();}
    void        SetDev(std::string sNewDevice) {CSoundPulse::SetDev(sNewDevice);}
    std::string GetVersion() { return pa_get_library_version(); }

    bool        Init(int iNewSampleRate, int iNewBufferSize, bool bNewBlocking);
    bool        Write(CVector<_SAMPLE>& psData);
    void        Close();
#ifdef CLOCK_DRIFT_ADJ_ENABLED
	pa_common *		GetCommonParamPtr() { return &cp; }
	void			EnableClockDriftAdj(bool bEnable) { bNewClockDriftComp = bEnable; }
	bool		IsClockDriftAdjEnabled() { return bNewClockDriftComp; }
#endif

	bool		bPrebuffer;
	bool		bSeek;
	bool		bBufferingError;
	bool		bMuteError;

protected:
	void			Init_HW();
	int				Write_HW(void *playbuf, int size);
	void			Close_HW();

	int				iSampleRate;
	int				iBufferSize;
    unsigned		timeToWait;
	bool		bBlockingPlay;

	pa_stream		*pa_s;
	pa_stream_notify_cb_userdata_t pa_stream_notify_cb_userdata_underflow;
	pa_stream_notify_cb_userdata_t pa_stream_notify_cb_userdata_overflow;

#ifdef CLOCK_DRIFT_ADJ_ENABLED
	int				iMaxSampleRateOffset;
	CReal			playback_usec_smoothed;
	int				target_latency;
	int				filter_stabilized;
	int				wait_prebuffer;
	CMatlibVector<CReal> B, A, X, Z;
	bool		bNewClockDriftComp;
	pa_common		cp;
	int				playback_usec; // DEBUG
	int				clock; // DEBUG
#endif
};

#endif
