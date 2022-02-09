#ifndef SOUNDININTERFACECOMPOSITE_H
#define SOUNDININTERFACECOMPOSITE_H

#include "soundinterface.h"
#include <vector>

class CSoundInInterfaceComposite : public CSoundInInterface
{
public:
    CSoundInInterfaceComposite();

    void AddInterface(CSoundInInterface *pInterface);

    // CSoundInInterface methods
    virtual 		~CSoundInInterfaceComposite();

    /* sound card interface - used by ReadData */
    virtual bool Init(int iSampleRate, int iNewBufferSize, bool bNewBlocking);
    virtual bool Read(CVector<short>& psData, CParameter& Parameters);
    virtual void     Close();
    virtual std::string	GetVersion();
    virtual CTuner * GetTuner();

    // CSelectionInterface methods
    virtual void		Enumerate(std::vector<std::string>& names, std::vector<std::string>& descriptions, std::string& defaultDevice);
    virtual std::string	GetDev();
    virtual void		SetDev(std::string sNewDev);

    // CTuner methods TODO later
    //virtual void SetFrequency(int);
    //virtual void LoadSettings(CSettings&);
    //virtual void SaveSettings(CSettings&);

protected:
    std::vector<CSoundInInterface *> vecSoundInInterfaces;
    CSoundInInterface *pCurrentInterface;
};

#endif // SOUNDININTERFACECOMPOSITE_H
