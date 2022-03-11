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

#include <QTranslator>
#include <QThread>
#include <QApplication>
#include <QMessageBox>

#ifdef _WIN32
# include <windows.h>
#else
# include <csignal>
#endif
#include <iostream>

#include "../GlobalDefinitions.h"
#include "../DrmReceiver.h"
#include "../DrmTransmitter.h"
#include "../util/Settings.h"
# include "fdrmdialog.h"
# include "TransmDlg.h"
# include "DialogUtil.h"

#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
#endif
#ifdef USE_OPENSL
# include <SLES/OpenSLES.h>
  SLObjectItf engineObject = nullptr;
#endif

#include "../main-Qt/crx.h"
#include "../main-Qt/ctx.h"

int
main(int argc, char **argv)
{
    QApplication app(argc, argv);

#ifdef USE_OPENSL
    (void)slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    (void)(*engineObject)->Realize(engineObject, SLbool_false);
#endif
#if defined(__unix__) && !defined(__APPLE__)
	/* Prevent signal interaction with popen */
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGPIPE);
	sigaddset(&sigset, SIGCHLD);
	pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
#endif


#if defined(__APPLE__)
	/* find plugins on MacOs when deployed in a bundle */
	app.addLibraryPath(app.applicationDirPath()+"../PlugIns");
#endif
#ifdef _WIN32
	WSADATA wsaData;
	(void)WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

	/* Load and install multi-language support (if available) */
    QTranslator translator(nullptr);
	if (translator.load("dreamtr"))
		app.installTranslator(&translator);

	CSettings Settings;
	/* Parse arguments and load settings from init-file */
	Settings.Load(argc, argv);

	try
	{
		string mode = Settings.Get("command", "mode", string());
		if (mode == "receive")
		{
			CDRMReceiver DRMReceiver(&Settings);

			/* First, initialize the working thread. This should be done in an extra
			   routine since we cannot 100% assume that the working thread is
			   ready before the GUI thread */

#ifdef HAVE_LIBHAMLIB
			CRig rig(DRMReceiver.GetParameters());
			rig.LoadSettings(Settings); // must be before DRMReceiver for G313
#endif
            CRx rx(DRMReceiver);

#ifdef HAVE_LIBHAMLIB
            DRMReceiver.SetRig(&rig);

			if(DRMReceiver.GetDownstreamRSCIOutEnabled())
			{
				rig.subscribe();
			}
            FDRMDialog *pMainDlg = new FDRMDialog(&rx, Settings, rig);
#else
			FDRMDialog *pMainDlg = new FDRMDialog(&rx, Settings);
#endif
			(void)pMainDlg;
            rx.LoadSettings();  // load settings after GUI initialised so LoadSettings signals get captured

			/* Start working thread */
            rx.start();

			/* Set main window */
			app.exec();

#ifdef HAVE_LIBHAMLIB
			if(DRMReceiver.GetDownstreamRSCIOutEnabled())
			{
				rig.unsubscribe();
			}
			rig.SaveSettings(Settings);
#endif
            rx.SaveSettings();
		}
		else if(mode == "transmit")
		{
            CDRMTransmitter DRMTransmitter(&Settings);
            CTx tx(DRMTransmitter);
            TransmDialog* pMainDlg = new TransmDialog(tx);

            tx.LoadSettings(); // load settings after GUI initialised so LoadSettings signals get captured
            /* Show dialog */
			pMainDlg->show();
            tx.start();
			app.exec();
            tx.SaveSettings();
        }
		else
		{
			CHelpUsage HelpUsage(Settings.UsageArguments(), argv[0]);
			app.exec();
			exit(0);
		}
	}

	catch(CGenErr GenErr)
	{
        qDebug("%s", GenErr.strError.c_str());
	}
	catch(string strError)
	{
        qDebug("%s", strError.c_str());
	}
	catch(char *Error)
	{
        qDebug("%s", Error);
	}

	/* Save settings to init-file */
	Settings.Save();

	return 0;
}
