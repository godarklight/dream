#ifndef CRX_H
#define CRX_H

#include "ctrx.h"
#include <QObject>
#include "../Parameter.h"

class CDRMReceiver;
class CPlotManager;
class CDataDecoder;

class CRx : public CTRx
{
    Q_OBJECT
public:
    explicit CRx(CDRMReceiver& nRx, CTRx *parent = nullptr);
    virtual ~CRx() override;
    void run() override;

    virtual void    GetInputPSD(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
    virtual void    GetPowDenSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
    virtual void    GetInputSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
    virtual void    GetAudioSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale);
    virtual void    GetAMBWParameters(double& rCenterFreq, double& rBW);
    virtual int     GetAMNoiseReductionLevel();
    virtual ENoiRedType GetAMNoiseReductionType();
    virtual EDemodType  GetAMDemodulationType();
    virtual EAmAgcType GetAMAGCType();
    virtual int GetAMFilterBW();
    virtual double GetAMMixerFrequencyOffset() const;
    virtual bool GetAMPLLPhase(_REAL&);

    virtual ERecMode GetReceiverMode();
    virtual int GetInChanSel();
    virtual int GetMSCMLInitNumIterations();
    virtual CPlotManager* GetPlotManager();
    _REAL ConvertFrequency(_REAL rFrequency, bool bInvert=false) const;
    virtual void GetMSCMLCVectorSpace(CVector<_COMPLEX>&);
    virtual void GetSDCMLCVectorSpace(CVector<_COMPLEX>&);
    virtual void GetFACMLCVectorSpace(CVector<_COMPLEX>&);

    virtual bool inputIsRSCI();
    virtual bool isWriteWaveFile();
    virtual bool isAudioMuted();
    virtual bool isIntefererConsiderationEnabled();
    virtual bool isFrequencySyncAcquisitionFilterEnabled() const;
    virtual bool isSpectrumFlipped();
    virtual bool isAMAutoFrequencyAcquisitionEnabled();
    virtual bool isAMPLLEnabled();
    virtual bool GetAMSSPLLPhase(_REAL&);
    virtual int GetAMSSPercentageDataEntityGroupComplete();
    virtual char* GetAMSSDataEntityGroupStatus();
    virtual int GetAMSSCurrentBlock();
    virtual char* GetAMSSCurrentBlockBits();
    virtual bool GetAMSSBlock1Status();
    virtual EAMSSBlockLockStat GetAMSSLockStatus();

    virtual bool CanDecode(int);

    virtual ETypeIntTime GetTimeInterpolationAlgorithm() const;
    virtual ETypeIntFreq GetFrequencyInterpolationAlgorithm() const;
    virtual ETypeTiSyncTrac GetTimeSyncTrackingType();

    virtual CDataDecoder* GetDataDecoder();
    virtual bool GetReverbEffect();
    virtual EAcqStat GetAcquisitionState();
    virtual int GetFrequency() override;
    virtual void EnumerateInputs(std::vector<string>& names, std::vector<string>& descriptions, std::string& defaultInput);
    virtual void EnumerateOutputs(std::vector<string>& names, std::vector<string>& descriptions, std::string& defaultOutput);
    virtual CSettings* GetSettings() override;
    virtual CParameter*	GetParameters() override;

public slots:
    virtual void LoadSettings() override;
    virtual void SaveSettings() override;
    virtual void SetInputDevice(std::string) override;
    virtual void SetOutputDevice(std::string) override;
    virtual void SetInputDevice(QString);
    virtual void SetOutputDevice(QString);
    virtual void Start();
    virtual void Stop();
    virtual void Restart();
    virtual void SetSettings(CSettings* pNewSettings) override;
    virtual bool IsReceiver() const override { return true;}
    virtual bool IsTransmitter() const override { return false;}
    virtual void StartWriteWaveFile(std::string);
    virtual void StopWriteWaveFile();
    virtual void SetTimeInterpolationAlgorithm(ETypeIntTime);
    virtual void SetFrequencyInterpolationAlgorithm(ETypeIntFreq);
    virtual void SetTimeSyncTrackingType(ETypeTiSyncTrac);
    virtual void SetNumMSCMLCIterations(int);
    virtual void SetFlipSpectrum(bool);
    virtual void SetFrequencySyncAcquisitionFilter(bool);
    virtual void SetConsiderInterferer(bool);
    virtual void MuteAudio(bool);
    virtual void SetReverberationEffect(bool);
    virtual void SetReceiverMode(ERecMode);
    virtual void SetAMDemodulationType(EDemodType);
    virtual void SetAMFilterBW(int);
    virtual void SetAMAGCType(EAmAgcType);
    virtual void SetAMNoiseReductionType(ENoiRedType);
    virtual void SetAMNoiseReductionLevel(int);
    virtual void SetFrequency(int) override;
    virtual void SetAMDemodAcq(_REAL);
    virtual void EnableAMPLL(bool);
    virtual void EnableAutoFrequenctAcquisition(bool);
    virtual void onSoundInChannelChanged(int);
    virtual void onSoundOutChannelChanged(EOutChanSel);
    virtual void onSoundSampleRateChanged(int);
    virtual void SetSoundSignalUpscale(int) override;

private:
    CDRMReceiver& rx;
    ERunState eRunState;

signals:
    void inputSampleRateChanged(int);
    void outputSampleRateChanged(int);
    void soundUpscaleRatioChanged(int);
    void soundFileChanged(QString);
    void InputDeviceChanged(QString);
    void OutputDeviceChanged(QString);
    void AMBWParametersChanged(double rCenterFreq, double rBW);
    void AMNoiseReductionLevelChanged(int);
    void AMNoiseReductionTypeChanged(ENoiRedType);
    void AMDemodulationTypeChanged(EDemodType);
    void AMAGCTypeChanged(EAmAgcType);
    void AMFilterBWChanged(int);
    void AMMixerFrequencyOffsetChanged(double);
    void AMPLLPhaseChanged(_REAL);
    void ReceiverModeChanged(ERecMode);
    void InputChannelChanged(int);
    void OutputChannelChanged(int);
    void MSCMLInitNumIterationsChanged(int);
    void isInputRSCIChanged(bool);
    void isWriteWaveFileChanged(bool);
    void isAudioMutedChanged(bool);
    void isIntefererConsiderationEnabledChanged(bool);
    void isFrequencySyncAcquisitionFilterEnabledChanged(bool);
    void isSpectrumFlippedChanged(bool);
    void isAMAutoFrequencyAcquisitionEnabledChanged(bool);
    void isAMPLLEnabledChanged(bool);
    void AMSSPLLPhaseChanged(_REAL);
    void AMSSPercentageDataEntityGroupCompleteChanged(int);
    void AMSSDataEntityGroupStatusChanged(QString);
    void AMSSCurrentBlockChanged(int);
    void AMSSCurrentBlockBitsChanged(QString);
    void AMSSBlock1StatusChanged(bool);
    void AMSSLockStatusChanged(EAMSSBlockLockStat);
    void canDecodeChanged(bool);
    void TimeInterpolationAlgorithmChanged(ETypeIntTime);
    void FrequencyInterpolationAlgorithmChanged(ETypeIntFreq);
    void TimeSyncTrackingTypeChanged(ETypeTiSyncTrac);
    void ReverbEffectChanged(bool);
    void AcquisitionStateChanged(EAcqStat);
    void FrequencyChanged(int);
    void drmModeStarted();
    void amModeStarted();
    void fmModeStarted();
};

#endif // CRX_H
