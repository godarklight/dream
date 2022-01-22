/******************************************************************************\
 *
 * Copyright (c) 2013
 *
 * Author(s):
 *  David Flamand
 *
 * Description:
 *  Module for interacting with console, no dependency to ncurses for
 *  increased portability with various handheld devices and dev boards,
 *  compatible with ANSI terminal.
 *  For best results stderr should be redirected to file or /dev/null
 *
 * Currently defined Keys:
 *  SPACEBAR  switch between signal info and service info
 *  F         display both signal info and service info
 *  N         toggle no display
 *  1 2 3 4   audio service selection
 *  CTRL-C Q  quit
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

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cstdio>
#include "ConsoleIO.h"
using namespace std;


/* defines ANSI escape code */
#define HIDECUR "\e[?25l"
#define SHOWCUR "\e[?25h"
#define HOME "\e[H"
#define CLEAR "\e[2J"
#define CLREND "\e[J"
#define CL "\e[K" /* Clear Line */
#define NL CL "\n" /* New Line */

/* defines console output operation */
#define cinit() do { screen_ptr=screen; screen_size=sizeof(screen); } while(0)
#define cprintf(...) do { int ret = snprintf(screen_ptr, (size_t)screen_size, __VA_ARGS__); if (ret>0) { screen_ptr+=ret; screen_size-=ret; } } while(0)
#define cflush() do { size_t ret = write(tty, screen, sizeof(screen)-screen_size); (void)ret; } while(0)
#define cwrite(text) do { size_t ret = write(tty, text "", sizeof(text "")-1); (void)ret; } while(0)

/* device for accessing the tty */
#define TTY_DEVICE "/dev/tty"

/* misc defines */
#define MODE_SCREEN 0x01
#define MODE_BOTH 0x02
#define MODE_NO_DISPLAY 0x04

#define REPLACE_CHAR(str, from, to) \
	for (size_t pos=0;;) { \
		pos = str.find(from, pos); \
		if (pos == string::npos) break; \
		str.replace(pos, 1, to ""); \
		pos += sizeof(to "")-1; \
	}
#define REPLACE_STR(str, from, to) \
	for (size_t pos=0;;) { \
		pos = str.find(from "", pos); \
		if (pos == string::npos) break; \
		str.replace(pos, sizeof(from "")-1, to ""); \
		pos += sizeof(to "")-1; \
	}

#define MINIMUM_IF_LEVEL -200.0

#define NA "---"


/* Implementation *************************************************************/

CDRMReceiver* CConsoleIO::pDRMReceiver;
unsigned long long CConsoleIO::time;
int CConsoleIO::mode;
int CConsoleIO::tty;
int CConsoleIO::tty_setup;
char CConsoleIO::screen[4096];
char* CConsoleIO::screen_ptr;
int CConsoleIO::screen_size;
sigset_t CConsoleIO::sigset;
struct termios CConsoleIO::old_tio;


void
CConsoleIO::Enter(CDRMReceiver* pDRMReceiver)
{
	CConsoleIO::pDRMReceiver = pDRMReceiver;

	/* Signals to ignore */
	struct sigaction sigact;
	sigact.sa_handler = SIG_IGN;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGPIPE, &sigact, nullptr);
	sigaction(SIGUSR1, &sigact, nullptr);
	sigaction(SIGUSR2, &sigact, nullptr);

	/* Signals to block */
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	sigaddset(&sigset, SIGHUP);
	sigaddset(&sigset, SIGTERM);
	sigaddset(&sigset, SIGQUIT);
	pthread_sigmask(SIG_SETMASK, &sigset, nullptr);

	/* TTY opening */
	tty = open(TTY_DEVICE, O_RDWR | O_NONBLOCK);
	if (tty != -1)
	{
		if (!tcgetattr(tty, &old_tio))
		{
			struct termios tio = old_tio;
			tio.c_lflag &= ~(ECHO|ICANON);
			tty_setup = tcsetattr(tty, TCSANOW, &tio);
		}
		else
			tty_setup = -1;
		cwrite(HIDECUR CLEAR HOME);
	}
}

void
CConsoleIO::Leave()
{
	if (tty != -1)
	{
		cwrite(CLEAR HOME SHOWCUR);
		if (!tty_setup)
			tcsetattr(tty, TCSANOW, &old_tio);
		close(tty);
		tty = -1;
	}
}

ERunState
CConsoleIO::Update()
{
	/* Check for pending signals */
	if (!sigpending(&sigset)) {
		if (sigismember(&sigset, SIGINT) ||
			sigismember(&sigset, SIGHUP) ||
			sigismember(&sigset, SIGTERM) ||
			sigismember(&sigset, SIGQUIT)) {
			return STOP_REQUESTED;
		}
	}

	/* No TTY return */
	if (tty < 0)
		return RUNNING;

	CParameter& Parameters = *pDRMReceiver->GetParameters();
	cinit();

	/* Read key input */
	for (;;)
	{
		unsigned char buffer[1];
		int ret = read(tty, buffer, sizeof(buffer));
		if (ret != sizeof(buffer))
			break;
		int key = buffer[0];
		if (key == '\3' /* Control-C */ || key == 'q' || key == 'Q')
		{
			return STOP_REQUESTED;
		}
		if (key == ' ')
		{
			if (mode & MODE_NO_DISPLAY)
				mode ^= MODE_NO_DISPLAY;
			else
				if (mode & MODE_BOTH)
					mode ^= MODE_BOTH;
				else
					mode = mode ^ MODE_SCREEN;
			cprintf(CLEAR);
			time = 0;
		}
		if (key >= '1' && key <= '4')
		{
			Parameters.SetCurSelAudioService(key - '1');
		}
		if (key == 'n' || key == 'N')
		{
			mode ^= MODE_NO_DISPLAY;
			cwrite(CLEAR HOME);
			time = 0;
		}
		if (key == 'f' || key == 'F')
		{
			mode ^= MODE_BOTH;
			cwrite(CLEAR);
			time = 0;
		}
	}

	if (mode & MODE_NO_DISPLAY)
		return RUNNING;

	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
		return RUNNING;
	unsigned long long curtime = (unsigned long long)ts.tv_sec*1000 + (unsigned long long)ts.tv_nsec/1000000;
	if ((curtime - time) < GUI_CONTROL_UPDATE_TIME)
		return RUNNING;
	time = curtime;

    char msc = ETypeRxStatus2char(Parameters.ReceiveStatus.SLAudio.GetStatus());
    char sdc = ETypeRxStatus2char(Parameters.ReceiveStatus.SDC.GetStatus());
    char fac = ETypeRxStatus2char(Parameters.ReceiveStatus.FAC.GetStatus());
    char time = ETypeRxStatus2char(Parameters.ReceiveStatus.TSync.GetStatus());
    char frame = ETypeRxStatus2char(Parameters.ReceiveStatus.FSync.GetStatus());
    ETypeRxStatus soundCardStatusI = Parameters.ReceiveStatus.InterfaceI.GetStatus(); /* Input */
    ETypeRxStatus soundCardStatusO = Parameters.ReceiveStatus.InterfaceO.GetStatus(); /* Output */
    char inter = ETypeRxStatus2char(soundCardStatusO == NOT_PRESENT ||
		(soundCardStatusI != NOT_PRESENT && soundCardStatusI != RX_OK) ? soundCardStatusI : soundCardStatusO);
	cprintf(HOME "        IO:%c  Time:%c  Frame:%c  FAC:%c  SDC:%c  MSC:%c" NL, inter, time, frame, fac, sdc, msc);

	_REAL rIFLevel = Parameters.GetIFSignalLevel();
	if (rIFLevel > MINIMUM_IF_LEVEL)
		cprintf("                   IF Level: %.1f dB" NL, rIFLevel);
	else
		cprintf("                   IF Level: " NA NL);

	if (!(mode & MODE_SCREEN) || mode & MODE_BOTH)
	{
		int signal = pDRMReceiver->GetAcquiState() == AS_WITH_SIGNAL;
		if (signal)
		{
			_REAL rSNR = Parameters.GetSNR();
			cprintf("                        SNR: %.1f dB" NL, rSNR);

			_REAL rWMERMSC = Parameters.rWMERMSC;
			_REAL rMER = Parameters.rMER;
			cprintf("         MSC WMER / MSC MER: %.1f dB / %.1f dB" NL, rWMERMSC, rMER);

			_REAL rDCFreq = Parameters.GetDCFrequency();
			cprintf(" DC Frequency of DRM Signal: %.2f Hz" NL, rDCFreq);

		    _REAL rCurSamROffs = Parameters.rResampleOffset;
			int iCurSamROffsPPM = (int)(rCurSamROffs / Parameters.GetSigSampleRate() * 1e6);
			cprintf("    Sample Frequency Offset: %.2f Hz (%i ppm)" NL, rCurSamROffs, iCurSamROffsPPM);

			_REAL rSigma = Parameters.rSigmaEstimate;
			_REAL rMinDelay = Parameters.rMinDelay;
			if (rSigma >= 0.0)
			cprintf("            Doppler / Delay: %.2f Hz / %.2f ms" NL, rSigma, rMinDelay);
			else
			cprintf("            Doppler / Delay: " NA NL);

			const char *strRob;
			switch (Parameters.GetWaveMode()) {
			case RM_ROBUSTNESS_MODE_A: strRob = "A"; break;
			case RM_ROBUSTNESS_MODE_B: strRob = "B"; break;
			case RM_ROBUSTNESS_MODE_C: strRob = "C"; break;
			case RM_ROBUSTNESS_MODE_D: strRob = "D"; break;
			default:                   strRob = "?"; }
			const char *strOcc;
			switch (Parameters.GetSpectrumOccup()) {
			case SO_0: strOcc = "4.5 kHz"; break;
			case SO_1: strOcc = "5 kHz";   break;
			case SO_2: strOcc = "9 kHz";   break;
	 		case SO_3: strOcc = "10 kHz";  break;
			case SO_4: strOcc = "18 kHz";  break;
			case SO_5: strOcc = "20 kHz";  break;
			default:   strOcc = "?";       }
			cprintf("       DRM Mode / Bandwidth: %s / %s" NL, strRob, strOcc);

			const char *strInter;
		    switch (Parameters.eSymbolInterlMode) {
		    case CParameter::SI_LONG:  strInter = "2 s";    break;
		    case CParameter::SI_SHORT: strInter = "400 ms"; break;
		    default:                   strInter = "?";      }
			cprintf("          Interleaver Depth: %s" NL, strInter);

			const char *strSDC;
			switch (Parameters.eSDCCodingScheme) {
			case CS_1_SM: strSDC = "4-QAM";  break;
			case CS_2_SM: strSDC = "16-QAM"; break;
			default:      strSDC = "?";      }
			const char *strMSC;
			switch (Parameters.eMSCCodingScheme) {
			case CS_2_SM:    strMSC = "SM 16-QAM";    break;
			case CS_3_SM:    strMSC = "SM 64-QAM";    break;
			case CS_3_HMSYM: strMSC = "HMsym 64-QAM"; break;
			case CS_3_HMMIX: strMSC = "HMmix 64-QAM"; break;
			default:         strMSC = "?";            }
			cprintf("             SDC / MSC Mode: %s / %s" NL, strSDC, strMSC);

			int iPartB = Parameters.MSCPrLe.iPartB;
			int iPartA = Parameters.MSCPrLe.iPartA;
			cprintf("        Prot. Level (B / A): %i / %i" NL, iPartB, iPartA);

			int iNumAudio = Parameters.iNumAudioService;
			int iNumData = Parameters.iNumDataService;
			cprintf("         Number of Services: Audio: %i / Data: %i" NL, (int)iNumAudio, (int)iNumData);

			int iYear = Parameters.iYear;
			int iMonth = Parameters.iMonth;
			int iDay = Parameters.iDay;
			int iUTCHour = Parameters.iUTCHour;
			int iUTCMin = Parameters.iUTCMin;
			if (iYear==0 && iMonth==0 && iDay==0 && iUTCHour==0 && iUTCMin==0)
			cprintf("       Received time - date: Service not available" CL);
			else
			cprintf("       Received time - date: %04i-%02i-%02i %02i:%02i:00" CL, iYear, iMonth, iDay, iUTCHour, iUTCMin);

		}
		else {
			cprintf("                        SNR: " NA NL
					"         MSC WMER / MSC MER: " NA NL
					" DC Frequency of DRM Signal: " NA NL
					"    Sample Frequency Offset: " NA NL
					"            Doppler / Delay: " NA NL
					"       DRM Mode / Bandwidth: " NA NL
					"          Interleaver Depth: " NA NL
					"             SDC / MSC Mode: " NA NL
					"        Prot. Level (B / A): " NA NL
					"         Number of Services: " NA NL
					"       Received time - date: " NA CL);
		}
	}
	if (mode & MODE_SCREEN || mode & MODE_BOTH)
	{
		if (mode & MODE_BOTH)
			cprintf(NL);
		cprintf(NL "Service:" NL);
		const char* strTextMessage = nullptr;
		int iCurAudService = Parameters.GetCurSelAudioService();
		for (int i = 0; i < MAX_NUM_SERVICES; i++)
		{
			CService service = Parameters.Service[i];
			if (service.IsActive())
			{
				const char *strLabel = service.strLabel.c_str();
				int ID = service.iServiceID;
				bool audio = service.eAudDataFlag == CService::SF_AUDIO;
				cprintf("%c%i | %X | %s ", i==iCurAudService ? '>' : ' ', i+1, ID, strLabel);
				_REAL rBitRate = Parameters.GetBitRateKbps(i, !audio);
				_REAL rPartABLenRat = Parameters.PartABLenRatio(i);
		        if (rPartABLenRat != (_REAL) 0.0)
					cprintf("| UEP (%.1f%%) |", rPartABLenRat * 100);
		        else
					cprintf("| EEP |");
				if (rBitRate > 0.0)
					cprintf(" %s %.2f kbps", audio ? "Audio" : "Data", rBitRate);
				if (audio && service.DataParam.iStreamID != STREAM_ID_NOT_USED)
				{
					_REAL rBitRate = Parameters.GetBitRateKbps(i, true);
					cprintf(" + Data %.2f kbps", rBitRate);
				}
				cprintf(NL);
				if (i == iCurAudService)
					strTextMessage = service.AudioParam.strTextMessage.c_str();
			}
			else
				cprintf(" %i" NL, i+1);
		}
		if (strTextMessage != nullptr)
		{
			string msg(strTextMessage);
			REPLACE_STR(msg, "\r\n", "\r");  /* Windows CR-LF */
			REPLACE_CHAR(msg, '\n', "\r");   /* New Line */
			REPLACE_CHAR(msg, '\f', "\r");   /* Form Feed */
			REPLACE_CHAR(msg, '\v', "\r");   /* Vertical Tab */
			REPLACE_CHAR(msg, '\t', "    "); /* Horizontal Tab */
			REPLACE_CHAR(msg, '\r', NL);     /* Carriage Return */
			cprintf(NL "%s", msg.c_str());
		}
	}
	cprintf(CLREND HOME);
	cflush();
	return RUNNING;
}

char
CConsoleIO::ETypeRxStatus2char(ETypeRxStatus eTypeRxStatus)
{
	switch (eTypeRxStatus) {
	case CRC_ERROR:  return 'X';
	case DATA_ERROR: return '*';
	case RX_OK:      return 'O';
	default:         return ' '; }
}

