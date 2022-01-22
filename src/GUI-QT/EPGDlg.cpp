/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide Viewer
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

#include "EPGDlg.h"
#include "../datadecoding/epgutil.h"
#include <set>
#include <QRegExp>
#include <QFile>
#include <QShowEvent>
#include <QHideEvent>
#include <QPixmap>
#include <QMetaObject>

EPGDlg::EPGDlg(CRx& nrx, CSettings& Settings, QWidget* parent):
    CWindow(parent, Settings, "EPG"),
    do_updates(false),
    epg(*nrx.GetParameters()),
    rx(nrx),
    sids(),
    greenCube(":/icons/greenCube.png"),
    next(nullptr)
{
    setupUi(this);

    /* auto resize of the programme name column */
    dateEdit->setDate(QDate::currentDate());
    connect(channel, SIGNAL(activated(const QString&)), this , SLOT(on_channel_activated(const QString&)));
    connect(dateEdit, SIGNAL(dateChanged(const QDate&)), this , SLOT(on_dateEdit_dateChanged(const QDate&)));
    connect(&Timer, SIGNAL(timeout()), this, SLOT(OnTimer()));

    TextEPGDisabled->hide();
}

EPGDlg::~EPGDlg()
{
}

void EPGDlg::setActive(QTreeWidgetItem* item)
{
    if(isActive(item))
    {
        item->setIcon(COL_START, greenCube);
        Data->scrollToItem(item);
        emit NowNext(item->text(COL_NAME));
        next = Data->itemBelow(item);
    }
    else
    {
        item->setIcon(COL_START, QPixmap()); /* no pixmap */
    }
}

void EPGDlg::OnTimer()
{
    /* Get current UTC time */
    time_t ltime;
    time(&ltime);
    tm gmtCur = *gmtime(&ltime);
    if(gmtCur.tm_sec==0) // minute boundary
    {
        /* today in UTC */
        QDate todayUTC = QDate(gmtCur.tm_year + 1900, gmtCur.tm_mon + 1, gmtCur.tm_mday);

        if ((basic->toPlainText() == tr("no basic profile data"))
                || (advanced->toPlainText() == tr("no advanced profile data")))
        {
            /* not all information is loaded */
            select();
        }
        /* Check the items now on line. */
        if (dateEdit->date() == todayUTC) /* if today */
        {
            for(int i=0; i<Data->topLevelItemCount(); i++)
                setActive(Data->topLevelItem(i));
        }
    }
}

void EPGDlg::eventShow(QShowEvent *)
{
    CParameter& Parameters = *rx.GetParameters();
    Parameters.Lock();
    int sNo = Parameters.GetCurSelAudioService();
    uint32_t sid = Parameters.Service[sNo].iServiceID;

    // use the current date
    dateEdit->setDate(QDate::currentDate());
    // update the channels combobox from the epg
    channel->clear();
    int n = -1;
    sids.clear();
    for (map < uint32_t, CServiceInformation >::const_iterator i = Parameters.ServiceInformation.begin();
            i != Parameters.ServiceInformation.end(); i++) {
        QString channel_label = QString().fromUtf8(i->second.label.begin()->c_str());
        uint32_t channel_id = i->second.id;
        sids[channel_label] = channel_id;
        channel->addItem(channel_label);
        if (channel_id == sid) {
            n = channel->currentIndex();
        }
    }
    Parameters.Unlock();
    // update the current selection
    if (n >= 0) {
        channel->setCurrentIndex(n);
    }
    do_updates = true;
    epg.progs.clear ();
    select();

    /* Activate real-time timer when window is shown */
    Timer.start(GUI_TIMER_EPG_UPDATE);
}

void EPGDlg::eventHide(QHideEvent*)
{
    /* Deactivate real-time timer */
    Timer.stop();
}

void EPGDlg::on_dateEdit_dateChanged(const QDate&)
{
    select();
}

void EPGDlg::on_channel_activated(const QString&)
{
    epg.progs.clear ();
    select();
}

void EPGDlg::select()
{
    QTreeWidgetItem* CurrActiveItem = nullptr;
    QDate date = dateEdit->date();

    if (!do_updates)
        return;
    Data->clear();
    basic->setText(tr("no basic profile data"));
    advanced->setText(tr("no advanced profile data"));
    QString chan = channel->currentText();
    // get schedule for date +/- 1 - will allow user timezones sometime
    QDomDocument *doc;
    QDate o = date.addDays(-1);
    doc = getFile (o, sids[chan], false);
    if(doc)
        epg.parseDoc(*doc);
    doc = getFile (o, sids[chan], true);
    if(doc)
        epg.parseDoc(*doc);
    o = date.addDays(1);
    doc = getFile (o, sids[chan], false);
    if(doc)
        epg.parseDoc(*doc);
    doc = getFile (o, sids[chan], true);
    if(doc)
        epg.parseDoc(*doc);

    QString xml;
    doc = getFile (date, sids[chan], false);
    if(doc)
    {
        epg.parseDoc(*doc);
        xml = doc->toString();
        if (xml.length() > 0)
            basic->setText(xml);
    }

    doc = getFile (date, sids[chan], true);
    if(doc)
    {
        epg.parseDoc(*doc);
        xml = doc->toString();
        if (xml.length() > 0)
            advanced->setText(xml);
    }

    if (epg.progs.count()==0) {
        (void) new QTreeWidgetItem(Data, QStringList() << tr("no data"));
        return;
    }
    Data->sortItems(COL_START, Qt::AscendingOrder);

    for (QMap < time_t, EPG::CProg >::Iterator i = epg.progs.begin();
            i != epg.progs.end(); i++)
    {
        const EPG::CProg & p = i.value();
        // TODO - let user choose time or actualTime if available, or show as tooltip
        time_t start;
        int duration;
        if (p.actualTime!=0)
        {
            start = p.actualTime;
            duration = p.actualDuration;
        }
        else
        {
            start = p.time;
            duration = p.duration;
        }
        QString s_start, s_duration;
        tm bdt = *gmtime(&start);

        // skip entries not on the wanted day
        if((bdt.tm_year+1900) != date.year())
        {
            continue;
        }
        if((bdt.tm_mon+1) != date.month())
        {
            continue;
        }
        if(bdt.tm_mday != date.day())
        {
            continue;
        }

        char s[40];
        sprintf(s, "%02d:%02d", bdt.tm_hour, bdt.tm_min);
        s_start = s;
        int min = duration / 60;
        sprintf(s, "%02d:%02d", int(min/60), min%60);
        s_duration = s;
        QString name, description, genre;
        if (p.name=="" && p.mainGenre.size()>0)
            name = "unknown " + p.mainGenre[0] + " programme";
        else
            name = p.name;
        description = p.description;
        // collapse white space in description
        description.replace(QRegExp("[\t\r\n ]+"), " ");
        if (p.mainGenre.size()==0)
            genre = "";
        else
        {
            // remove duplicate genres
            set<QString> genres;
            for (size_t i=0; i<p.mainGenre.size(); i++) {
                if (p.mainGenre[i] != "Audio only") {
                    genres.insert(p.mainGenre[i]);
                }
            }
            QString sep="";
            for(set<QString>::const_iterator g = genres.begin(); g!=genres.end(); g++)
            {
                genre = genre+sep+(*g);
                sep = ", ";
            }
        }
        QStringList l;
        l << s_start << name << genre << description << s_duration;
        QTreeWidgetItem* CurrItem = new QTreeWidgetItem(Data, l);
        QDateTime dt;
        dt.setTime_t(start);
        CurrItem->setData(COL_START, Qt::UserRole, dt);
        CurrItem->setData(COL_DURATION, Qt::UserRole, duration);
        if (isActive(CurrItem))
            CurrActiveItem = CurrItem;
    }
    if (CurrActiveItem) /* programme is now on line */
        setActive(CurrActiveItem);
}

QString EPGDlg::getFileName(const QDate& date, uint32_t sid, bool bAdvanced)
{
    CDateAndTime d;
    d.year = date.year();
    d.month = date.month();
    d.day = date.day();
    return epg.dir + "/" + epgFilename(d, sid, 1, bAdvanced).c_str();
}

QString EPGDlg::getFileName_etsi(const QDate& date, uint32_t sid, bool bAdvanced)
{
    CDateAndTime d;
    d.year = date.year();
    d.month = date.month();
    d.day = date.day();
    return epg.dir + "/" + epgFilename_etsi(d, sid, 1, bAdvanced).c_str();
}

QDomDocument*
EPGDlg::getFile(const QString& path)
{
    QFile file (path);
    if (!file.open (QIODevice::ReadOnly))
    {
        return nullptr;
    }
    vector<_BYTE> vecData;
    vecData.resize (file.size ());
    file.read((char *) &vecData.front (), file.size ());
    file.close ();
    CEPGDecoder *epg = new CEPGDecoder();
    epg->decode (vecData);
    epg->doc.documentElement().insertBefore(
        epg->doc.createComment(path),
        epg->doc.documentElement().firstChild()
    );
    return &(epg->doc);
}

QDomDocument*
EPGDlg::getFile (const QDate& date, uint32_t sid, bool bAdvanced)
{
    QString path = getFileName(date, sid, bAdvanced);
    QDomDocument* doc = getFile(path);
    if(doc != nullptr)
        return doc;
    return getFile(getFileName_etsi(date, sid, bAdvanced));
}

bool EPGDlg::isActive(QTreeWidgetItem* item)
{
    QDateTime start = item->data(COL_START, Qt::UserRole).toDateTime();
    int duration = item->data(COL_DURATION, Qt::UserRole).toInt();
    QDateTime end = start.addSecs(duration);
    QDateTime now = QDateTime::currentDateTime();
    if(now<start)
        return false;
    if(now>=end)
        return false;
    return true;
}
