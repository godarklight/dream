/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2009
 *
 * Author(s):
 *	 Julian Cable
 *
 * Description: Journaline Specialisation of TextBrowser
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

#include "jlbrowser.h"
#include "../datadecoding/DataDecoder.h"
#include "../datadecoding/Journaline.h"
#include <iostream>

JLBrowser::JLBrowser(QWidget * parent)
    : QTextBrowser(parent),decoder(nullptr),strFhGIISText(),strJournalineHeadText(),
      total(0),ready(0)
{

    /* Set FhG IIS text */
    strFhGIISText =
        "<table><tr><td><img src=\":/icons/LogoFhGIIS.png\"></td>"
        "<td width=\"4\"></td>"
        "<td><font face=\"Courier\" size=\"-1\">Features NewsService "
        "Journaline" + QString(QChar(174)) /* (R) */ +
        " decoder technology by Fraunhofer IIS, Erlangen, "
        "Germany. For more information visit http://www.iis.fhg.de/dab"
        "</font></td></tr></table>";

    /* Set Journaline headline text */
    strJournalineHeadText =
        "<table><tr><td><img src=\":/icons/LogoJournaline.png\"></td>"
        "<td width=\"4\"></td>"
        "<td valign=\"middle\"><h2>NewsService Journaline" + QString(QChar(174)) /* (R) */ +
        "</h2></td></tr></table>";
}

bool JLBrowser::changed()
{
    if(decoder==nullptr)
    {
        return false;
    }
    int JourID = source().toString().toInt();

    CNews News;
    decoder->GetNews(JourID, News);

    int new_total=News.vecItem.Size();
    int new_ready=0;
    bool dirty = false;

    for (int i = 0; i < new_total; i++)
    {
        switch(News.vecItem[i].iLink)
        {
        case JOURNALINE_IS_NO_LINK: /* Only text, no link */
        case JOURNALINE_LINK_NOT_ACTIVE:
            break;
        default:
            new_ready++;
        }
    }
    if(new_total != total)
    {
        total = new_total;
        dirty = true;
    }
    if(new_ready != ready)
    {
        ready = new_ready;
        dirty = true;
    }
    return dirty;
}

QVariant JLBrowser::loadResource( int, const QUrl & name )
{
    QString strItems, strTitle;

    if(decoder)
    {
        /* Get news from actual Journaline decoder */
        int JourID = name.toString().toInt();

        CNews News;
        decoder->GetNews(JourID, News);

        /* Decode UTF-8 coding for title */
        strTitle = QString().fromUtf8(News.sTitle.c_str());

        ready = 0;
        total = News.vecItem.Size();
        for (int i = 0; i < total; i++)
        {
            QString strCurItem = QString().fromUtf8(News.vecItem[i].sText.c_str());

            /* Replace \n by html command <br> */
            strCurItem = strCurItem.replace(QRegExp("\n"), "<br>");

            switch(News.vecItem[i].iLink)
            {
            case JOURNALINE_IS_NO_LINK: /* Only text, no link */
                strItems += strCurItem + QString("<br>");
                break;
            case JOURNALINE_LINK_NOT_ACTIVE:
                /* Un-ordered list item without link */
                strItems += QString("<li>") + strCurItem + QString("</li>");
                break;
            default:
                ready++;
                QString strLinkStr = QString("%1").arg(News.vecItem[i].iLink);
                /* Un-ordered list item with link */
                strItems += QString("<li><a href=\"") + strLinkStr +
                            QString("\">") + strCurItem + QString("</a></li>");
            }
        }
    }

    return
        strJournalineHeadText +
        "<table>"
        "<tr><td width=\"100%\"><hr></td></tr>" /* horizontial line */
        "<tr><td><stylebody><b><center>" + strTitle + "</center></b></stylebody></td></tr>"
        "<tr><td><stylebody><ul type=\"square\">" + strItems +
        "</ul></stylebody></td></tr>"
        "<tr><td width=\"100%\"><hr></td></tr>" /* horizontial line */
        "</table>"
        + strFhGIISText;
}

void JLBrowser::setDecoder(CDataDecoder* d)
{
    decoder = d;
}
