/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2005
 *
 * Author(s):
 *	
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

#include "Util.h"
#include "../DrmTransceiver.h"
#include <QRegExp>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include "../Parameter.h"

/* Ensure that the given filename is secure */
QString VerifyFilename(QString filename)
{
    filename.replace(QRegExp("/"), "_"); /* replace unix-like path separator with '_' */
#ifdef _WIN32
    filename.replace(QRegExp("\\\\"), "_"); /* replace windows path separator with '_' */
    filename.replace(QRegExp(":"), "_"); /* replace ':' with '_' */
#endif
    return filename;
}

/* Ensure that the given html path is secure */
QString VerifyHtmlPath(QString path)
{
    if (path == "..")
        return "_";
#ifdef _WIN32
    path.replace(QRegExp("\\\\"), "_"); /* replace windows path separator with '_' */
    path.replace(QRegExp(":"), "_"); /* replace ':' with '_' */
#endif
    path.replace(QRegExp("^\\.\\./"), "_/"); /* replace '../' at the beginning with '_/' */
    path.replace(QRegExp("/\\.\\.$"), "/_"); /* replace '/..' at the end with '/_' */
    path.replace(QRegExp("/\\.\\./"), "/_/"); /* replace '/../' with '/_/' */
    return path;
}

/* Accept both absolute and relative url, but only return the path component.
   Invalid characters in path are percent-encoded (e.g. space = %20) */
QString UrlEncodePath(QString url)
{
    /* Get path component */
    QString path(QUrl(url, QUrl::TolerantMode).path());
    /* Prepend with '/' if none present */
    if (path.size() == 0 || path.at(0) != QChar('/'))
        path.insert(0, QChar('/'));
    /* Replace multiple '/' by single '/' */
    path.replace(QRegExp("/{1,}"), "/");
    /* Replace all occurrence of '/./' with '/' */
    while (path.indexOf("/./") != -1)
        path.replace(QRegExp("/\\./"), "/");
    /* The Actual percent encoding */
    path = QString(QUrl(path, QUrl::TolerantMode).toEncoded(
        QUrl::RemoveScheme | QUrl::RemoveAuthority |
        QUrl::RemoveQuery | QUrl::RemoveFragment));
    return path;
}

/* Determine if the given url is a directory */
bool IsUrlDirectory(QString url)
{
    return url.endsWith(QChar('/'));
}

/* Convert all www. or http:// or email to real
   clickable link, for use with QLabel and such.
   Code by David Flamand */
QString& Linkify(QString& text)
{
    int i, j, posWWW=-2, posHTTP=-2, posMAIL=-2, posBegin, posEnd, size;
    size = text.size();
    for (i = 0; i < size;)
    {
        if (posWWW != -1 && posWWW < i)
            posWWW  = text.indexOf("www.", i, Qt::CaseInsensitive);
        if (posHTTP != -1 && posHTTP < i)
            posHTTP = text.indexOf("http://", i, Qt::CaseInsensitive);
        if (posMAIL != -1 && posMAIL < i)
            posMAIL = text.indexOf(QRegExp("\\b[0-9a-z._-]+@[0-9a-z.-]+\\.[a-z]{2,4}\\b", Qt::CaseInsensitive), i);
        if (posMAIL>=0 && (posMAIL<=posWWW || posWWW<0) && (posMAIL<posHTTP || posHTTP<0))
            posBegin = posMAIL;
        else if (posWWW>=0 && (posWWW<posHTTP || posHTTP<0))
            posBegin = posWWW;
        else
            posBegin = posHTTP;
        if (posBegin >= 0)
        {
            posEnd = size;
            for (j = posBegin; j < size; j++)
            {
                int chr = text[j].unicode();
                if (!((chr=='@' && posBegin==posMAIL) ||
                      chr=='.' || chr=='/' ||
                      chr=='~' || chr=='-' ||
                      chr=='_' || chr==':' ||
                     (chr>='a' && chr<='z') ||
                     (chr>='A' && chr<='Z') ||
                     (chr>='0' && chr<='9')))
                {
                    posEnd = j;
                    break;
                }
            }
            const int rawLinkSize = posEnd-posBegin;
            QStringRef rawLink(&text, posBegin, rawLinkSize);
            QString newLink;
            if (posBegin == posMAIL)
                newLink = "<a href=\"mailto:%1\">%1</a>";
            else if (posBegin == posWWW)
                newLink = "<a href=\"http://%1\">%1</a>";
            else /* posBegin == posHTTP */
                newLink = "<a href=\"%1\">%1</a>";
            newLink = newLink.arg(rawLink.toString());
            const int newLinkSize = newLink.size();
            text.replace(posBegin, rawLinkSize, newLink);
            const int diff = newLinkSize - rawLinkSize;
            i = posEnd + diff;
            size += diff;
            if (posWWW >= 0)
                posWWW += diff;
            if (posHTTP >= 0)
                posHTTP += diff;
            if (posMAIL >= 0)
                posMAIL += diff;
        }
        else
            break;
    }
    return text;
}

void CreateDirectories(const QString& strFilename)
{
    /*
    	This function is for creating a complete directory structure to a given
    	file name. If there is a pathname like this:
    	/html/files/images/myimage.gif
    	this function create all the folders into MOTCache:
    	/html
    	/html/files
    	/html/files/images
    	QFileInfo only creates a file if the pathname is valid. If not all folders
    	are created, QFileInfo will not save the file. There was no QT function
    	or a hint the QT mailing list found in which does the job of this function.
    */
    for (int i = 0;; i++)
    {
#ifdef _WIN32
        int i1 = strFilename.indexOf(QChar('/'), i);
        int i2 = strFilename.indexOf(QChar('\\'), i);
        i = (i1 >= 0 && ((i1 < i2) || (i2<0))) ? i1 : i2;
#else
        i = strFilename.indexOf(QChar('/'), i);
#endif
        if (i < 0)
            break;
        const QString strDirName = strFilename.left(i);
        if (!strDirName.isEmpty() && !QFileInfo(strDirName).exists())
            QDir().mkdir(strDirName);
    }
}
