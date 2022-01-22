/******************************************************************************\
 * BBC and Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001-2019
 *
 * Author(s):
 * Volker Fischer, Julian Cable
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

#ifndef CTRANSMITDATA_H
#define CTRANSMITDATA_H

#include "util/Modul.h"
#include "sound/soundinterface.h"
#include "util/Utilities.h"
#ifdef QT_MULTIMEDIA_LIB
# include <QIODevice>
#endif

class CTransmitData : public CTransmitterModul<_COMPLEX, _COMPLEX>
{
public:
    enum EOutFormat {OF_REAL_VAL /* real valued */, OF_IQ_POS,
                     OF_IQ_NEG /* I / Q */, OF_EP /* envelope / phase */
                    };

    CTransmitData();

    virtual ~CTransmitData();

    void SetIQOutput(const EOutFormat eFormat) {
        eOutputFormat = eFormat;
    }
    EOutFormat GetIQOutput() {
        return eOutputFormat;
    }

    void SetAmplifiedOutput(bool bEnable) {
        bAmplified = bEnable;
    }
    bool GetAmplifiedOutput() {
        return bAmplified;
    }

    void SetHighQualityIQ(bool bEnable) {
        bHighQualityIQ = bEnable;
    }
    bool GetHighQualityIQ() {
        return bHighQualityIQ;
    }

    void SetCarOffset(const CReal rNewCarOffset)
    {
        rDefCarOffset = rNewCarOffset;
    }

    void SetSoundInterface(std::string);
    std::string GetSoundInterface() { return soundDevice; }
    void Enumerate(std::vector<string>& names, std::vector<string>& descriptions, std::string& defaultOutput);
    void Stop();
#ifdef QT_MULTIMEDIA_LIB
    std::string GetSoundInterfaceVersion() { return "QtMultimedia"; }
#else
    std::string GetSoundInterfaceVersion() { return pSound->GetVersion(); }
#endif

    void SetWriteToFile(const std::string strNFN)
    {
        strOutFileName = strNFN;
        bUseSoundcard = false;
    }

    void FlushData();

protected:
    FILE*			pFileTransmitter;
#ifdef QT_MULTIMEDIA_LIB
    QIODevice*      pIODevice;
#endif
    CSoundOutInterface*	pSound;
    std::string     soundDevice;
    CVector<short>	vecsDataOut;
    int				iBlockCnt;
    int				iNumBlocks;
    EOutFormat		eOutputFormat;

    CDRMBandpassFilt	BPFilter;
    CReal			rDefCarOffset;

    CReal			rNormFactor;

    int				iBigBlockSize;

    std::string		strOutFileName;
    bool			bUseSoundcard;
    int				iSampleRate;

    bool			bAmplified;
    bool			bHighQualityIQ;
    CVector<_REAL>	vecrReHist;

    void HilbertFilt(_COMPLEX& vecData);

    virtual void InitInternal(CParameter& TransmParam);
    virtual void ProcessDataInternal(CParameter& Parameter);
};


#endif // CTRANSMITDATA_H
