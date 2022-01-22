/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2006
 *
 * Author(s):
 *	Andrea Russo
 *
 * Description:
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

#include "MultSettingsDlg.h"
#include <QFileDialog>
#include <QShowEvent>
#include <QHideEvent>
#include <QLabel>
#include <QToolTip>

/* Implementation *************************************************************/

MultSettingsDlg::MultSettingsDlg(CParameter& NP, CSettings& NSettings,
	QWidget* parent) :
	QDialog(parent), Parameters(NP), Settings(NSettings)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	setupUi(this);

	/* Set help text for the controls */
	AddWhatsThisHelp();

	// TODO
	CheckBoxAddRefresh->hide();
	EdtSecRefresh->hide();

	/* Connect buttons */

	connect(PushButtonChooseDir, SIGNAL(clicked()),
		this, SLOT(OnbuttonChooseDir()));

	connect(buttonClearCacheMOT, SIGNAL(clicked()),
		this, SLOT(OnbuttonClearCacheMOT()));

	connect(buttonClearCacheEPG, SIGNAL(clicked()),
		this, SLOT(OnbuttonClearCacheEPG()));

	EdtSecRefresh->setValidator(new QIntValidator(MIN_MOT_BWS_REFRESH_TIME, MAX_MOT_BWS_REFRESH_TIME, EdtSecRefresh));
}

MultSettingsDlg::~MultSettingsDlg()
{
}

void MultSettingsDlg::hideEvent(QHideEvent*)
{
}

void MultSettingsDlg::showEvent(QShowEvent*)
{
    SetDataDirectoryControls();
}

void MultSettingsDlg::ClearCache(QString sPath, QString sFilter = "", bool bDeleteDirs)
{
	/* Delete files into sPath directory with scan recursive */

	QDir dir(sPath);

	/* Check if the directory exists */
	if (dir.exists())
	{
		dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks);

		/* Eventually apply the filter */
		if (sFilter != "")
			dir.setNameFilters(QStringList(sFilter));

		dir.setSorting( QDir::DirsFirst );
		const QList<QFileInfo> list = dir.entryInfoList();
		for(QList<QFileInfo>::const_iterator fi = list.begin(); fi!=list.end(); fi++)
		{

			/* for each file/dir */
			/* if directory...=> scan recursive */
			if (fi->isDir())
			{
				if(fi->fileName()!="." && fi->fileName()!="..")
				{
					ClearCache(fi->filePath(), sFilter, bDeleteDirs);

					/* Eventually delete the directory */
					if (bDeleteDirs)
						dir.rmdir(fi->fileName());
				}
			}
			else
			{
				/* Is a file so remove it */
				dir.remove(fi->fileName());
			}
		}
	}
}

void MultSettingsDlg::OnbuttonChooseDir()
{
	QString strFilename = QFileDialog::getExistingDirectory(this, TextLabelDir->text());
	/* Check if user not hit the cancel button */
	if (!strFilename.isEmpty())
	{
#ifdef _WIN32
		strFilename.replace(QRegExp("/"), "\\");
#endif
		Parameters.SetDataDirectory(string(strFilename.toUtf8().constData()));
		SetDataDirectoryControls();
	}
}

void MultSettingsDlg::OnbuttonClearCacheMOT()
{
	/* Delete all files and directories in the MOT directory */
	ClearCache(QString::fromUtf8(Parameters.GetDataDirectory("MOT").c_str()), "", true);
}

void MultSettingsDlg::OnbuttonClearCacheEPG()
{
	/* Delete all EPG files */
	ClearCache(QString::fromUtf8(Parameters.GetDataDirectory("EPG").c_str()), "*.EHA;*.EHB");
}

void MultSettingsDlg::SetDataDirectoryControls()
{
	QString strFilename(QString::fromUtf8(Parameters.GetDataDirectory().c_str()));
#undef PATH_SEP
#ifdef _WIN32
	strFilename.replace(QRegExp("/"), "\\");
# define PATH_SEP '\\'
#else
# define PATH_SEP '/'
#endif
	if (!strFilename.isEmpty() && strFilename.at(strFilename.length()-1) == QChar(PATH_SEP))
		strFilename.remove(strFilename.length()-1, 1);
	TextLabelDir->setToolTip(strFilename);
	TextLabelDir->setText(strFilename);
}

void MultSettingsDlg::AddWhatsThisHelp()
{
	//TODO
}
