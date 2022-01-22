Rem/******************************************************************************\
rem * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
rem * Copyright (c) 2001-2006
rem *
rem * Author(s):
rem *	Volker Fischer
rem *
rem * Description:
rem *	Script for compiling the QT resources under Windows (MOCing and UICing)
rem *
rem ******************************************************************************
rem *
rem * This program is free software; you can redistribute it and/or modify it under
rem * the terms of the GNU General Public License as published by the Free Software
rem * Foundation; either version 2 of the License, or (at your option) any later
rem * version.
rem *
rem * This program is distributed in the hope that it will be useful, but WITHOUT
rem * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
rem * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 1111
rem * details.
rem *
rem * You should have received a copy of the GNU General Public License along with
rem * this program; if not, write to the Free Software Foundation, Inc.,
rem * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
rem *
rem\******************************************************************************/

rem .h --------------
md moc
%qtdir%\bin\moc.exe ..\src\GUI-QT\Logging.h -o moc\moc_Logging.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\DialogUtil.h -o moc\moc_DialogUtil.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\MultColorLED.h -o moc\moc_MultColorLED.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\systemevalDlg.h -o moc\moc_systemevalDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\fmdialog.h -o moc\moc_fmdialog.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\fdrmdialog.h -o moc\moc_fdrmdialog.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\TransmDlg.h -o moc\moc_TransmDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\qt2\DRMPlot.h -o moc\moc_DRMPlot.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\MultimediaDlg.h -o moc\moc_MultimediaDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\StationsDlg.h -o moc\moc_StationsDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\LiveScheduleDlg.h -o moc\moc_LiveScheduleDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\AnalogDemDlg.h -o moc\moc_AnalogDemDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\EPGDlg.h -o moc\moc_EPGDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\Rig.h -o moc\moc_Rig.cpp
%qtdir%\bin\moc.exe ..\src\MDI\PacketSocketQT.h -o moc\moc_PacketSocketQT.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\MultSettingsDlg.h -o moc\moc_MultSettingsDlg.cpp
%qtdir%\bin\moc.exe ..\src\GUI-QT\GeneralSettingsDlg.h -o moc\moc_GeneralSettingsDlg.cpp

rem .ui -------------
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\fmdialogbase.ui -o moc\fmdialogbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\fmdialogbase.ui -i fmdialogbase.h -o moc\fmdialogbase.cpp
%qtdir%\bin\moc.exe moc\fmdialogbase.h -o moc\moc_fmdialogbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\fdrmdialogbase.ui -o moc\fdrmdialogbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\fdrmdialogbase.ui -i fdrmdialogbase.h -o moc\fdrmdialogbase.cpp
%qtdir%\bin\moc.exe moc\fdrmdialogbase.h -o moc\moc_fdrmdialogbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\TransmDlgbase.ui -o moc\TransmDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\TransmDlgbase.ui -i TransmDlgbase.h -o moc\TransmDlgbase.cpp
%qtdir%\bin\moc.exe moc\TransmDlgbase.h -o moc\moc_TransmDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\systemevalDlgbase.ui -o moc\systemevalDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\systemevalDlgbase.ui -i systemevalDlgbase.h -o moc\systemevalDlgbase.cpp
%qtdir%\bin\moc.exe moc\systemevalDlgbase.h -o moc\moc_systemevalDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\AboutDlgbase.ui -o moc\AboutDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\AboutDlgbase.ui -i AboutDlgbase.h -o moc\AboutDlgbase.cpp
%qtdir%\bin\moc.exe moc\AboutDlgbase.h -o moc\moc_AboutDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\MultimediaDlgbase.ui -o moc\MultimediaDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\MultimediaDlgbase.ui -i MultimediaDlgbase.h -o moc\MultimediaDlgbase.cpp
%qtdir%\bin\moc.exe moc\MultimediaDlgbase.h -o moc\moc_MultimediaDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\StationsDlgbase.ui -o moc\StationsDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\StationsDlgbase.ui -i StationsDlgbase.h -o moc\StationsDlgbase.cpp
%qtdir%\bin\moc.exe moc\StationsDlgbase.h -o moc\moc_StationsDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\LiveScheduleDlgbase.ui -o moc\LiveScheduleDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\LiveScheduleDlgbase.ui -i LiveScheduleDlgbase.h -o moc\LiveScheduleDlgbase.cpp
%qtdir%\bin\moc.exe moc\LiveScheduleDlgbase.h -o moc\moc_LiveScheduleDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\AnalogDemDlgbase.ui -o moc\AnalogDemDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\AnalogDemDlgbase.ui -i AnalogDemDlgbase.h -o moc\AnalogDemDlgbase.cpp
%qtdir%\bin\moc.exe moc\AnalogDemDlgbase.h -o moc\moc_AnalogDemDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\AMSSDlgbase.ui -o moc\AMSSDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\AMSSDlgbase.ui -i AMSSDlgbase.h -o moc\AMSSDlgbase.cpp
%qtdir%\bin\moc.exe moc\AMSSDlgbase.h -o moc\moc_AMSSDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\EPGDlgbase.ui -o moc\EPGDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\EPGDlgbase.ui -i EPGDlgbase.h -o moc\EPGDlgbase.cpp
%qtdir%\bin\moc.exe moc\EPGDlgbase.h -o moc\moc_EPGDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\MultSettingsDlgbase.ui -o moc\MultSettingsDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\MultSettingsDlgbase.ui -i MultSettingsDlgbase.h -o moc\MultSettingsDlgbase.cpp
%qtdir%\bin\moc.exe moc\MultSettingsDlgbase.h -o moc\moc_MultSettingsDlgbase.cpp

%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\GeneralSettingsDlgbase.ui -o moc\GeneralSettingsDlgbase.h
%qtdir%\bin\uic.exe ..\src\GUI-QT\qt2\GeneralSettingsDlgbase.ui -i GeneralSettingsDlgbase.h -o moc\GeneralSettingsDlgbase.cpp
%qtdir%\bin\moc.exe moc\GeneralSettingsDlgbase.h -o moc\moc_GeneralSettingsDlgbase.cpp

