/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide utilities
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

#include <QDomDocument>
#include <QMap>
#include <QString>
#include <QDateTime>
#include "../GlobalDefinitions.h"
#include "../Parameter.h"
#include "../datadecoding/DABMOT.h"
#include "epgdec.h"

class EPG
{
  public:
    EPG (CParameter&);
    virtual ~ EPG ()
    {
	saveChannels (servicesFilename);
    }
	/* assignment operator to help MSVC8 */
	EPG& operator=(const EPG&);

    void loadChannels (const QString & fileName);
    void saveChannels (const QString & fileName);
    void addChannel (const std::string& label, uint32_t sid);
    void parseDoc (const QDomDocument &);

    class CProg
    {
      public:

	  CProg(): time(0), actualTime(0), duration(0), actualDuration(0),
                name(""), description(""),
			  crid(""), shortId(0), mainGenre(), secondaryGenre(), otherGenre()
		{}
        void augment(const CProg&);

		time_t time, actualTime;
		int duration, actualDuration;
		QString name, description;
		QString crid;
		uint32_t shortId;
		std::vector<QString> mainGenre, secondaryGenre, otherGenre;
    };

    QMap < time_t, CProg > progs;
    QMap < QString, QString > genres;
    QString dir, servicesFilename;
    CParameter& Parameters;
    QMap < QString, time_t > filesLoaded;
private:
    static const struct gl { const char *genre; const char* desc; } genre_list[];
    time_t parseTime(const QString & time);
    int parseDuration (const QString & duration);
};
