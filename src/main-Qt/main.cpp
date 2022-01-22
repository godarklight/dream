/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2004
 *
 * Author(s):
 *	Volker Fischer, Stephane Fillod
 *
 * Description:
 *
 * 11/10/2004 Stephane Fillod
 *	- QT translation
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

#include "../GlobalDefinitions.h"
#include "../DrmReceiver.h"
#include "../DrmTransmitter.h"
#include "../DrmSimulation.h"
#include "../util/Settings.h"
#include <iostream>

#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
#endif
#include <QCoreApplication>
#include <QTranslator>
#include <QThread>

#include "../main-Qt/crx.h"
#include "../main-Qt/ctx.h"

int
main(int argc, char **argv)
{
#ifdef _WIN32
    WSADATA wsaData;
    (void)WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
    try
    {
        QCoreApplication app(argc, argv);

        CSettings Settings;
        Settings.Load(argc, argv);

        string mode = Settings.Get("command", "mode", string());
        if (mode == "receive")
        {
            CDRMSimulation DRMSimulation;
            CDRMReceiver DRMReceiver(&Settings);

            DRMSimulation.SimScript();
            DRMReceiver.LoadSettings();

            CRx rx(DRMReceiver);

            QObject::connect(&rx, SIGNAL(finished()), &app, SLOT(quit()), Qt::QueuedConnection);

            rx.start();
            return app.exec();

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
                while (eRunState == RESTART);

                /* Closing the sound interfaces */
                DRMTransmitter.Close();
            }
            catch (CGenErr GenErr)
            {
                cerr << GenErr.strError << endl;
            }
            catch (string strError)
            {
                cerr << strError << endl;
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
        cerr << GenErr.strError << endl;
    }

    return 0;
}
