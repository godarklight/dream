#ifndef SOUNDIN_H
#define SOUNDIN_H

#include "../sound/soundinterface.h"

/* Classes ********************************************************************/
class COpenSLESIn : public CSoundInInterface
{
public:
    COpenSLESIn();
    virtual ~COpenSLESIn();

    virtual void		Enumerate(std::vector<string>&, std::vector<string>&);
    virtual void		SetDev(std::string sNewDevice);
    virtual std::string		GetDev();
    virtual int			GetSampleRate();

    virtual bool	Init(int iNewSampleRate, int iNewBufferSize, bool bNewBlocking);
    virtual bool 	Read(CVector<short>& psData);
    virtual void 		Close();

protected:
    std::string currentDevice;
};

#endif // SOUNDIN_H
