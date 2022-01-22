#ifndef TUNER_H
#define TUNER_H

class CSettings;

class CTuner
{
public:
    virtual void SetFrequency(int) = 0;
    virtual void LoadSettings(CSettings& s) = 0;
    virtual void SaveSettings(CSettings& s) = 0;

    virtual ~CTuner();
};

#endif // TUNER_H
