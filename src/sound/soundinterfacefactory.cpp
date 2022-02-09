#include "soundinterfacefactory.h"
#include "SoundInInterfaceComposite.h"

/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable
 *
 * Decription:
 * sound interface selection
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

#if defined(WIN32) && !defined(USE_PORTAUDIO) && !defined(USE_JACK) && !defined(QT_MULTIMEDIA_LIB)
/* mmsystem sound interface */
# include "../windows/Sound.h"
#else

# ifdef USE_ALSA
#  include "../linux/alsain.h"
#  include "../linux/alsaout.h"
# endif

# ifdef USE_JACK
#  include "../linux/jack.h"
# endif

# ifdef USE_PULSEAUDIO
#  include "drm_pulseaudio.h"
# endif

#ifdef USE_SOAPYSDR
#include "drm_soapySDR.h"
#endif


# ifdef USE_PORTAUDIO
#  include "drm_portaudio.h"

# endif

# ifdef USE_OPENSL
#  include "../android/soundin.h"
#  include "../android/soundout.h"

# endif

# if defined(QT_MULTIMEDIA_LIB) || (!defined(USE_OSS) && !defined(USE_ALSA) && !defined(USE_JACK) && !defined(USE_PULSEAUDIO) && !defined(USE_PORTAUDIO) && !defined(USE_OPENSL))
#  include "soundnull.h"

# endif

#endif






CSoundInterfaceFactory::CSoundInterfaceFactory()
{

}

CSoundInInterface * CSoundInterfaceFactory::CreateSoundInInterface()
{
    CSoundInInterfaceComposite *pSoundInInterfaceComposite = new CSoundInInterfaceComposite();

#if defined(USE_SOAPYSDR)
    pSoundInInterfaceComposite->AddInterface(new CSoapySDRIn());
#endif

#if defined(WIN32) && !defined(USE_PORTAUDIO) && !defined(USE_JACK) && !defined(QT_MULTIMEDIA_LIB)
/* mmsystem sound interface */
    pSoundInInterfaceComposite->AddInterface(new CSoundInMMSystem());
#elif defined(USE_ALSA)
    pSoundInInterfaceComposite->AddInterface(new CSoundInAlsa());
#elif defined (USE_JACK)
    pSoundInInterfaceComposite->AddInterface(new CSoundInJack());
#elif defined (USE_PULSEAUDIO)
    pSoundInInterfaceComposite->AddInterface(new CSoundInPulse());
#elif defined (USE_PORTAUDIO)
    pSoundInInterfaceComposite->AddInterface(new CPaIn());
#elif defined(USE_OPENSL)
    pSoundInInterfaceComposite->AddInterface(new COpenSLESIn());
// defined(QT_MULTIMEDIA_LIB) || (!defined(USE_OSS) && !defined(USE_ALSA) && !defined(USE_JACK) && !defined(USE_PULSEAUDIO) && !defined(USE_PORTAUDIO) && !defined(USE_OPENSL))
#else
    pSoundInInterfaceComposite->AddInterface(new CSoundInNull());
# endif

    return pSoundInInterfaceComposite;

}


CSoundOutInterface * CSoundInterfaceFactory::CreateSoundOutInterface()
{
#if defined(WIN32) && !defined(USE_PORTAUDIO) && !defined(USE_JACK) && !defined(QT_MULTIMEDIA_LIB)
/* mmsystem sound interface */
    return new CSoundOutMMSystem();
#elif defined(USE_ALSA)
    return new CSoundOutAlsa();
# elif defined(USE_JACK)
    return new CSoundOutJack();
#elif defined(USE_PULSEAUDIO)
    return new CSoundOutPulse();
#elif defined(USE_PORTAUDIO)
    return new CPaOut();
#elif defined(USE_OPENSL)
    return new COpenSLESOut();
#else
// if defined(QT_MULTIMEDIA_LIB) || (!defined(USE_OSS) && !defined(USE_ALSA) && !defined(USE_JACK) && !defined(USE_PULSEAUDIO) && !defined(USE_PORTAUDIO) && !defined(USE_OPENSL))
    return new CSoundOutNull();
# endif


}
