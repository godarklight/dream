/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * British Broadcasting Corporation
 * Copyright (c) 2001-2013
 *
 * Author(s):
 *	Volker Fischer, Julian Cable
 *
 * Description:
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

#include <QMenuBar>
#include <QLabel>
#include <QAction>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QWhatsThis>
#ifdef _WIN32
# include <winsock2.h>
#endif
#include "../Version.h"
#include "../util-QT/Util.h"
#include "DialogUtil.h"
#ifdef HAVE_LIBHAMLIB
# include "../util-QT/Rig.h"
#endif

#ifdef HAVE_LIBSNDFILE
# include <sndfile.h>
#endif
#ifdef HAVE_LIBPCAP
# include <pcap.h>
#endif
#ifdef QT_GUI_LIB
# include <qwt_global.h>
#endif
#ifdef USE_OPUS_LIBRARY
# include "../sourcedecoders/opus_codec.h"
#endif
#ifdef USE_FAAD2_LIBRARY
# include <neaacdec.h>
#else
# include "../sourcedecoders/neaacdec_dll.h"
#endif
#ifdef HAVE_LIBFDK_AAC
# include "../sourcedecoders/fdk_aac_codec.h"
#endif
#ifdef USE_ALSA
# include <alsa/version.h>
#endif
#ifdef USE_PORTAUDIO
# include <portaudio.h>
#endif
#ifdef USE_PULSEAUDIO
# include <pulse/pulseaudio.h>
#endif
#ifdef USE_JACK
#endif
/* fftw 3.3.2 doesn't export the symbol fftw_version
 * for windows in libfftw3-3.def
 * You can add it regenerate the lib file and it's supposed to work,
 * but for now the version string is disabled for windows. */
#ifndef _WIN32
# include <fftw3.h>
#endif

QString VersionString(QWidget* parent)
{
    QString strVersionText;
    strVersionText =
        "<center><b>" + parent->tr("Dream, Version ");
    if (dream_version_patch == 0)
        strVersionText += QString("%1.%2%3")
            .arg(dream_version_major)
            .arg(dream_version_minor)
            .arg(dream_version_build);
    else
        strVersionText += QString("%1.%2.%3%4")
            .arg(dream_version_major)
            .arg(dream_version_minor)
            .arg(dream_version_patch)
            .arg(dream_version_build);
    strVersionText +=
        "</b><br> " + parent->tr("Open-Source Software Implementation of "
                                 "a DRM-Receiver") +
        "<br>";
    strVersionText += parent->tr("Under the GNU General Public License (GPL)") +
        "</center>";
    return strVersionText;
#ifdef _MSC_VER /* MSVC 2008 */
    parent; // warning C4100: 'parent' : unreferenced formal parameter
#endif
}

/* Implementation *************************************************************/
/* About dialog ------------------------------------------------------------- */
CAboutDlg::CAboutDlg(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);
#ifdef HAVE_LIBSNDFILE
    char  sfversion [128] ;
    sf_command (nullptr, SFC_GET_LIB_VERSION, sfversion, sizeof (sfversion)) ;
#endif
#ifdef HAVE_LIBFDK_AAC
    FdkAacCodec fdk;
#endif
    QString strCredits = 
        "<p>" /* General description of Dream software */
        "<big><b>Dream</b> " + tr("is a software implementation of a Digital "
                                  "Radio Mondiale (DRM) receiver. With Dream, DRM broadcasts can be received "
                                  "with a modified analog receiver (SW, MW, LW) and a PC with a sound card.")
        + "</big></p><br>"
        "<p><font face=\"" FONT_COURIER "\">" /* GPL header text */
        "This program is free software; you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation; either version 2 of the License, or "
        "(at your option) any later version.<br>This program is distributed in "
        "the hope that it will be useful, but WITHOUT ANY WARRANTY; without "
        "even the implied warranty of MERCHANTABILITY or FITNESS FOR A "
        "PARTICULAR PURPOSE. See the GNU General Public License for more "
        "details.<br>You should have received a copy of the GNU General Public "
        "License along with his program; if not, write to the Free Software "
        "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 "
        "USA"
        "</font></p><br>" /* Our warning text */
        "<p><font color=\"#ff0000\" face=\"" FONT_COURIER "\">" +
        tr("Although this software is going to be "
           "distributed as free software under the terms of the GPL this does not "
           "mean that its use is free of rights of others. The use may infringe "
           "third party IP and thus may not be legal in some countries.") +
        "</font></p><br>"
        "<p>" /* Libraries used by this compilation of Dream */
        "<b>" + tr("This compilation of Dream uses the following libraries:") +
        "</b></p>"
        "<ul>"
#ifndef _WIN32
        "<li><b>FFTW</b> (" + QString(fftw_version) + ") <i>http://www.fftw.org</i></li>"
#else
        "<li><b>FFTW</b> <i>http://www.fftw.org</i></li>"
#endif
#ifdef QT_CORE_LIB
        "<li><b>Qt</b> (" + QString(QT_VERSION_STR) + ") <i>http://qt-project.org</i></li>"
#endif
#ifdef QT_GUI_LIB
        "<li><b>QWT</b> (" + QString(QWT_VERSION_STR) + ") <i>Dream is based in part on the work of the Qwt "
        "project (http://qwt.sf.net).</i></li>"
#endif
#ifdef HAVE_LIBHAMLIB
        "<li><b>Hamlib</b> (" + QString(hamlib_version) + ") <i>http://hamlib.sourceforge.net</i></li>"
#endif
        "<li><b>FhG IIS Journaline Decoder</b> <i>Features NewsService "
        "Journaline(R) decoder technology by Fraunhofer IIS, Erlangen, "
        "Germany. For more information visit http://www.iis.fraunhofer.de/en/bf/db/pro.html</i></li>"
#ifdef HAVE_LIBPCAP
        "<li><b>LIBPCAP</b> (" + QString(pcap_lib_version()) + ") <i>http://www.tcpdump.org/ "
        "This product includes software developed by the Computer Systems "
        "Engineering Group at Lawrence Berkeley Laboratory.</i></li>"
#endif
#ifdef HAVE_LIBSNDFILE
        "<li><b>LIBSNDFILE</b> (" + QString(sfversion) + ") <i>http://www.mega-nerd.com/libsndfile</i></li>"
#endif
#ifdef HAVE_SPEEX
        "<li><b>LIBSPEEX</b> <i>http://www.speex.org</i></li>"
#endif
#ifdef HAVE_LIBFDK_AAC
		"<li><b>fdk-aac decoder</b> (" + fdk.DecGetVersion().c_str() + ") <i>https://www.iis.fraunhofer.de/en/ff/amm/impl.html</i></li>"
#endif
#if USE_FAAD2_LIBRARY
		"<li><b>FAAD2</b> (" + QString(FAAD2_VERSION) + ") <i>AAC/HE-AAC/HE-AACv2/DRM decoder "
		"(c) Ahead Software, www.nero.com (http://faac.sf.net)</i></li>"
#endif
#ifdef USE_FAAC_LIBRARY
		"<li><b>FAAC</b> <i>http://faac.sourceforge.net</i></li>"
#endif
#ifdef USE_OPUS_LIBRARY
		"<li><b>" + QString(OPUS_DESCRIPTION) + "</b> (" + QString(opusGetVersion()) + ") <i>" + QString(OPUS_WEBSITE_LINK) + "</i></li>"
#endif
		"<p>The audio and signal device drivers in use are as follows:</p>"
#ifdef USE_ALSA
        "<li><b>ALSA</b> (" + QString(SND_LIB_VERSION_STR) + ") <i>http://www.alsa-project.org</i></li>"
#endif
#ifdef USE_PORTAUDIO
        "<li><b>PortAudio</b> (" + QString(Pa_GetVersionText()) + ") <i>http://www.portaudio.com</i></li>"
#endif
#ifdef USE_PULSEAUDIO
        "<li><b>PulseAudio</b> (" + QString(pa_get_headers_version()) + ") <i>http://www.pulseaudio.org</i></li>"
#endif
#ifdef USE_JACK
        "<li><b>libjack</b> (The Jack Audio Connection Kit) <i>http://www.jackaudio.org</i></li>"
#endif
#ifdef QT_MULTIMEDIA_LIB
		"<li><b>Qt Multimedia</b>(" + QString(QT_VERSION_STR) + ") <i>http://qt-project.org</i></li>"
#else
# ifdef WIN32
		"<li><b>Windows Waveform Audio</b>(WinMM) <i>https://docs.microsoft.com</i></li>"
# endif
#endif
        "</ul><br><br><hr/><br><br>"
        "<center><b>HISTORY</b></center><br>"
        "The Dream software development was started at <i>Darmstadt University "
        "of Technology</i> at the Institute of Communication Technology by <i>Volker "
        "Fischer</i> and <i>Alexander Kurpiers</i> in 2001-2005. "
        "The core digital signal processing and most of the GUI were the "
        "result of this development.<br>In 2005, <i>Andrew Murphy</i> of the <i>British "
        "Broadcasting Corporation</i> added code for an "
        "AMSS demodulator. <i>Oliver Haffenden</i> and <i>Julian Cable</i> (also <i>BBC</i>) rewrote "
        "the MDI interface and added RSCI support."
        " Many other GUI improvements were implemented by <i>Andrea Russo and David Flamand</i>."
        "<br>Right now the code is mainly maintained by <i>David Flamand, Julian Cable and Rafael Diniz</i>."
        " Quality Assurance and user testing is provided by <i>Simone St&ouml;ppler.</i>"
        "<br><br><br>"
        "<center><b>"+tr("CREDITS")+"</b></center><br>"
        +tr("We want to thank all the contributors to the Dream software (in "
            "alphabetical order):")+"<br><br>"
        "<b>"+tr("Developers")+"</b>"
        "<center>"
        "<p>Bakker, Menno</p>"
        "<p>Cable, Julian</p>"
        "<p>Cesco</p>"
        "<p>Diniz, Rafael</p>"
        "<p>Fillod, Stephane</p>"
        "<p>Fischer, Volker</p>"
        "<p>Fine, Mark J.</p>"
        "<p>Flamand, David</p>"
        "<p>Haffenden, Oliver</p>"
        "<p>Kurpiers, Alexander</p>"
        "<p>Manninen, Tomi</p>"
        "<p>Moore, Josh</p>"
        "<p>Murphy, Andrew</p>"
        "<p>Pascutto, Gian C.</p>"
        "<p>Peca, Marek</p>"
        "<p>Richard, Doyle</p>"
        "<p>Russo, Andrea</p>"
        "<p>Turnbull, Robert</p>"
        "</center>"
        "<p>"+tr("If your name should be in the above list and its missing, please let us know.")+"</p>"
        "<br><b>"+tr("Parts of Dream are based on code by")+"</b>"
        "<center>"
        "<p>Karn, Phil (www.ka9q.net)</p>"
        "<p>Ptolemy Project (http://ptolemy.eecs.berkeley.edu)</p>"
        "<p>Tavernini, Lucio (http://tavernini.com/home.html)</p>"
        "<p>The Math Forum (http://mathforum.org)</p>"
        "<p>The Synthesis ToolKit in C++ (STK) "
        "(http://ccrma.stanford.edu/software/stk)</p>"
        "</center>"
        "<br><b>"+tr("Supporters")+"</b>"
        "<center>"
        "<p>Amorim, Roberto Jos&eacute; de</p>"
        "<p>Kainka, Burkhard</p>"
        "<p>Keil, Jens</p>"
        "<p>Kilian, Gerd</p>"
        "<p>Kn&uuml;tter, Carsten</p>"
        "<p>Ramisch, Roland</p>"
        "<p>Schall, Norbert</p>"
        "<p>Schill, Dietmar</p>"
        "<p>Schneider, Klaus</p>"
        "<p>St&ouml;ppler, Simone</p>"
        "<p>Varlamov, Oleg</p>"
        "<p>Wade, Graham</p>"
        "</center><br>";

    /* Add link to text */
    Linkify(strCredits);

    /* Set the text for the about dialog html text control */
    TextViewCredits->setText(strCredits);

    /* Set version number in about dialog */
    TextLabelVersion->setText(VersionString(this));

    /* Set author names in about dialog */
    TextLabelAuthorNames->setText("Volker Fischer, Alexander Kurpiers, Andrea Russo, Julian Cable\nAndrew Murphy, Oliver Haffenden, David Flamand, Rafael Diniz");

    /* Set copyright year in about dialog */
    TextLabelCopyright->setText("Copyright (C) 2001 - 2020");
}

/* Help Usage --------------------------------------------------------------- */
CHelpUsage::CHelpUsage(const char* usage, const char* argv0, QWidget* parent)
    : CAboutDlg(parent)
{
    setWindowTitle(tr("Dream Command Line Help"));
    TextLabelVersion->setText(VersionString(this));
    TextLabelAuthorNames->setText("");
    TextLabelCopyright->setText(tr("Command line usage:"));
    QString text(tr(usage));
    text.replace(QRegExp("\\$EXECNAME"), QString::fromUtf8(argv0));
    TextViewCredits->setFontFamily(FONT_COURIER);
    TextViewCredits->setPlainText(text);
    show();
}

/* System Tray -------------------------------------------------------------- */

CSysTray::CSysTray(QWidget* parent, const char* callbackIcon, const char* callbackTimer, const char* icon)
    : pTimer(nullptr), pContextMenu(nullptr)
{
    pSystemTrayIcon = new QSystemTrayIcon(QIcon(icon), parent);
    if (callbackIcon != nullptr)
        parent->connect(pSystemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), parent, callbackIcon);
    if (callbackTimer != nullptr)
	{
		pTimer = new QTimer();
        parent->connect(pTimer, SIGNAL(timeout()), parent, callbackTimer);
	}
    pSystemTrayIcon->show();
}

CSysTray::~CSysTray()
{
	if (pTimer != nullptr)
		delete pTimer;
    if (pSystemTrayIcon != nullptr)
        delete pSystemTrayIcon;
    if (pContextMenu != nullptr)
        delete pContextMenu;
}

void CSysTray::CreateContextMenu()
{
    if (pContextMenu == nullptr)
    {
        pContextMenu = new QMenu();
        pSystemTrayIcon->setContextMenu(pContextMenu);
    }
}

CSysTray* CSysTray::Create(QWidget* parent, const char* callbackIcon, const char* callbackTimer, const char* icon)
{
    CSysTray* pSysTray = nullptr;
    if (QSystemTrayIcon::isSystemTrayAvailable())
        pSysTray = new CSysTray(parent, callbackIcon, callbackTimer, icon);
    return pSysTray;
}

void CSysTray::Destroy(CSysTray** pSysTray)
{
    if (*pSysTray != nullptr)
    {
        delete *pSysTray;
        *pSysTray = nullptr;
    }
}

void CSysTray::Start(CSysTray* pSysTray)
{
    if (pSysTray == nullptr) return;
    if (pSysTray->pTimer != nullptr)
        pSysTray->pTimer->start(GUI_CONTROL_UPDATE_TIME);
}

void CSysTray::Stop(CSysTray* pSysTray, const QString& Message)
{
    if (pSysTray == nullptr) return;
	if (pSysTray->pTimer != nullptr)
        pSysTray->pTimer->stop();
    SetToolTip(pSysTray, QString(), Message);
}

QAction* CSysTray::AddAction(CSysTray* pSysTray, const QString& text, const QObject* receiver, const char* member)
{
    if (pSysTray == nullptr) return nullptr;
    pSysTray->CreateContextMenu();
    return pSysTray->pContextMenu->addAction(text, receiver, member);
}

QAction* CSysTray::AddSeparator(CSysTray* pSysTray)
{
    if (pSysTray == nullptr) return nullptr;
    pSysTray->CreateContextMenu();
    return pSysTray->pContextMenu->addSeparator();
}

void CSysTray::SetToolTip(CSysTray* pSysTray, const QString& Title, const QString& Message)
{
    if (pSysTray != nullptr &&
        (pSysTray->Title != Title || pSysTray->Message != Message))
    {
        pSysTray->Title = Title;
        pSysTray->Message = Message;
        QString ToolTip;
#ifdef _WIN32
        ToolTip = Title;
        if (!Message.isEmpty())
        {
            if (!Title.isEmpty())
                ToolTip += " |  ";
            ToolTip += Message;
        }
        ToolTip.replace(QRegExp("(\r|\n|\v|\t|\b)"), " ");
#else
        if (!Title.isEmpty())
        {
            QString NewTitle(Title);
            NewTitle.replace('&', "&amp;");
            NewTitle.replace(' ', "&nbsp;");
            NewTitle.replace('<', "&lt;");
            NewTitle.replace('>', "&gt;");
            ToolTip = "<b>" + NewTitle + "</b>";
        }
        if (!Message.isEmpty())
        {
            QString NewMessage(Message);
            if (!Title.isEmpty())
                ToolTip += "<br>";
            NewMessage.replace('&', "&amp;");
            NewMessage.replace('<', "&lt;");
            NewMessage.replace('>', "&gt;");
            ToolTip += NewMessage;
        }
        ToolTip.replace(QRegExp("(\r|\n|\v)"), "<br>");
#endif
        pSysTray->pSystemTrayIcon->setToolTip(ToolTip);
    }
}

/* -------------------------------------------------------------------------- */

void InitSMeter(QWidget* parent, QwtThermo* sMeter)
{
#if QWT_VERSION < 0x060100
    sMeter->setRange(S_METER_THERMO_MIN, S_METER_THERMO_MAX);
    sMeter->setScale(S_METER_THERMO_MIN, S_METER_THERMO_MAX, 10.0);
#else
    sMeter->setScale(S_METER_THERMO_MIN, S_METER_THERMO_MAX);
    sMeter->setScaleStepSize(10.0);
#endif
    sMeter->setAlarmLevel(S_METER_THERMO_ALARM);
    sMeter->setAlarmLevel(-12.5);
    sMeter->setAlarmEnabled(true);
    sMeter->setValue(S_METER_THERMO_MIN);
#if QWT_VERSION < 0x060000
    (void)parent;
    sMeter->setAlarmColor(QColor(255, 0, 0));
    sMeter->setFillColor(QColor(0, 190, 0));
#else
    QPalette newPalette = parent->palette();
    newPalette.setColor(QPalette::Base, newPalette.color(QPalette::Window));
    newPalette.setColor(QPalette::ButtonText, QColor(0, 190, 0));
    newPalette.setColor(QPalette::Highlight,  QColor(255, 0, 0));
    sMeter->setPalette(newPalette);
#endif
}
