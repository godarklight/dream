/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 * Volker Fischer
 *
 * Description:
 * See Sound.cpp
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

#if !defined(AFX_SOUNDIN_H__9518A621_7F78_11D3_8C0D_EEBF182CF549__INCLUDED_)
#define AFX_SOUNDIN_H__9518A621_7F78_11D3_8C0D_EEBF182CF549__INCLUDED_

#include "../sound/soundinterface.h"
#include <windows.h>
#include <mmsystem.h>

/* Set this number as high as we have to prebuffer symbols for one MSC block.
   In case of robustness mode D we have 24 symbols */
#define NUM_SOUND_BUFFERS_IN 24  /* Number of sound card buffers */

#ifdef USE_QT_GUI
# if QT_VERSION >= 0x030000
#  define NUM_SOUND_BUFFERS_OUT 6  /* Number of sound card buffers */
# else
#  define NUM_SOUND_BUFFERS_OUT 12  /* Number of sound card buffers */
# endif
#else
# define NUM_SOUND_BUFFERS_OUT 3  /* Number of sound card buffers */
#endif
#define NUM_IN_OUT_CHANNELS  2  /* Stereo */
#define BITS_PER_SAMPLE   16  /* Use all bits of the D/A-converter */
#define BYTES_PER_SAMPLE  2  /* Number of bytes per sample */

/* Classes ********************************************************************/
class CSoundInMMSystem : public CSoundInInterface
{
    public:
    CSoundInMMSystem();
virtual ~CSoundInMMSystem();

virtual bool Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking);
virtual bool Read(CVector<short>& psData);
virtual void  Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultInput);
virtual std::string GetDev();
virtual void  SetDev(std::string sNewDev);
virtual void  Close();
virtual std::string GetVersion() { return ""; };

protected:
void  OpenDevice();
void  PrepareBuffer(int iBufNum);
void  AddBuffer();

std::vector<std::string> vecstrDevices;
std::string   sCurDev;
WAVEFORMATEX sWaveFormatEx;
bool  bChangDev;
HANDLE   m_WaveEvent;
int   iSampleRate;
int   iBufferSize;
int   iWhichBuffer;
bool  bBlocking;

/* Wave in */
WAVEINCAPS  m_WaveInDevCaps;
HWAVEIN   m_WaveIn;
WAVEHDR   m_WaveInHeader[NUM_SOUND_BUFFERS_IN];
short*   psSoundcardBuffer[NUM_SOUND_BUFFERS_IN];

};

class CSoundOutMMSystem : public CSoundOutInterface
{
    public:
    CSoundOutMMSystem();
virtual ~CSoundOutMMSystem();

virtual bool Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking);
virtual bool Write(CVector<short>& psData);
virtual void  Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultOutput);

virtual std::string  GetDev();
virtual void  SetDev(std::string sNewDev);
virtual void  Close();
virtual std::string  GetVersion() { return ""; };

protected:
void  OpenDevice();
void  PrepareBuffer(int iBufNum);
void  AddBuffer(int iBufNum);
void  GetDoneBuffer(int& iCntPrepBuf, int& iIndexDoneBuf);

std::vector<std::string> vecstrDevices;
std::string   sCurDev;
WAVEFORMATEX sWaveFormatEx;
bool  bChangDev;
HANDLE   m_WaveEvent;
int   iSampleRate;
int   iBufferSize;
int   iWhichBuffer;
bool  bBlocking;

/* Wave out */
WAVEOUTCAPS  m_WaveOutDevCaps;
HWAVEOUT  m_WaveOut;
short*   psPlaybackBuffer[NUM_SOUND_BUFFERS_OUT];
WAVEHDR   m_WaveOutHeader[NUM_SOUND_BUFFERS_OUT];
};

#endif // !defined(AFX_SOUNDIN_H__9518A621_7F78_11D3_8C0D_EEBF182CF549__INCLUDED_)
