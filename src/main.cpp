/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2004
 *
 * Author(s):
 *	Volker Fischer, Stephane Fillod, Julian Cable
 *
 * Description: main programme for console mode
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

#ifdef _WIN32
# include <windows.h>
#endif
#if defined(__unix__) && !defined(__APPLE__)
# include <csignal>
#endif
#ifdef USE_CONSOLEIO
# include "linux/ConsoleIO.h"
#endif
#include "GlobalDefinitions.h"
#include "DrmReceiver.h"
#include "DrmTransmitter.h"
#include "DrmSimulation.h"
#include "util/Settings.h"
#include <iostream>

int
main(int argc, char **argv)
{
    try
	{
		CSettings Settings;
		Settings.Load(argc, argv);

		string mode = Settings.Get("command", "mode", string());
		if (mode == "receive")
		{
			CDRMSimulation DRMSimulation;
			CDRMReceiver DRMReceiver(&Settings);

			DRMSimulation.SimScript();
			DRMReceiver.LoadSettings();

#ifdef _WIN32
	WSADATA wsaData;
	(void)WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
            try
            {
                // set the frequency from the command line or ini file
                int iFreqkHz = DRMReceiver.GetParameters()->GetFrequency();
                if (iFreqkHz != -1)
                    DRMReceiver.SetFrequency(iFreqkHz);

#ifdef USE_CONSOLEIO
                CConsoleIO::Enter(&DRMReceiver);
#endif
                ERunState eRunState = RESTART;
                do
                {
                    DRMReceiver.InitReceiverMode();
                    DRMReceiver.SetInStartMode();
                    eRunState = RUNNING;
                    do
                    {
                        DRMReceiver.updatePosition();
                        DRMReceiver.process();
#ifdef USE_CONSOLEIO
                        eRunState = CConsoleIO::Update();
#endif
                    }
                    while (eRunState == RUNNING);
                }
                while (eRunState == RESTART);
                DRMReceiver.CloseSoundInterfaces();
#ifdef USE_CONSOLEIO
                CConsoleIO::Leave();
#endif
            }
            catch (CGenErr GenErr)
            {
                perror(GenErr.strError.c_str());
            }
            catch (string strError)
            {
                perror(strError.c_str());
            }
        }

		else if (mode == "transmit")
		{
			CDRMTransmitter DRMTransmitter(&Settings);
			DRMTransmitter.LoadSettings();
            try
            {
                /* Set restart flag */
                ERunState eRunState = RESTART;
                do
                {
                    /* Initialization of the modules */
                    DRMTransmitter.Init();

                    /* Set run flag */
                    eRunState = RUNNING;

                    /* Start the transmitter run routine */
                    DRMTransmitter.Run();
                }
                while(eRunState == RESTART);

                /* Closing the sound interfaces */
                DRMTransmitter.Close();
            }
            catch (CGenErr GenErr)
            {
                perror(GenErr.strError.c_str());
            }
            catch (string strError)
            {
                perror(strError.c_str());
            }
        }
		else
		{
			string usage(Settings.UsageArguments());
			for (;;)
			{
				size_t pos = usage.find("$EXECNAME");
				if (pos == string::npos) break;
				usage.replace(pos, sizeof("$EXECNAME")-1, argv[0]);
			}
			cerr << usage << endl << endl;
			exit(0);
		}
	}
	catch(CGenErr GenErr)
	{
        perror(GenErr.strError.c_str());
    }

	return 0;
}

void
DebugError(const char *pchErDescr, const char *pchPar1Descr,
		   const double dPar1, const char *pchPar2Descr, const double dPar2)
{
	FILE *pFile = fopen("test/DebugError.dat", "a");
	fprintf(pFile, "%s", pchErDescr);
	fprintf(pFile, " ### ");
	fprintf(pFile, "%s", pchPar1Descr);
	fprintf(pFile, ": ");
	fprintf(pFile, "%e ### ", dPar1);
	fprintf(pFile, "%s", pchPar2Descr);
	fprintf(pFile, ": ");
	fprintf(pFile, "%e\n", dPar2);
	fclose(pFile);
	fprintf(stderr, "\nDebug error! For more information see test/DebugError.dat\n");
	exit(1);
}
