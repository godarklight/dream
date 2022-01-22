/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2014
 *
 * Author(s):
 *  Alexander Kurpiers
 *
 * Decription:
 * Linux sound interface
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

#ifndef _SOUND_H
#define _SOUND_H

#ifdef USE_ALSA
# include "alsain.h"
# include "alsaout.h"
#endif

#ifdef USE_JACK
# include "jack.h"
typedef CSoundInJack CSoundIn;
typedef CSoundOutJack CSoundOut;
#endif

#ifdef USE_PORTAUDIO
# include "portaudio.h"
typedef CPaIn CSoundIn;
typedef CPaOut CSoundOut;
#endif

#if !defined(USE_ALSA) && !defined(USE_JACK) && !defined(USE_PORTAUDIO)
# include "../soundnull.h"
typedef CSoundInNull CSoundIn;
typedef CSoundOutNull CSoundOut;
#endif

#endif
