#ifndef SOUNDFACTORY_H
#define SOUNDFACTORY_H

#include "soundinterface.h"



class CSoundInterfaceFactory
{
public:
    CSoundInterfaceFactory();
    static CSoundInInterface *CreateSoundInInterface();
    static CSoundOutInterface *CreateSoundOutInterface();

};

#endif // SOUNDFACTORY_H
