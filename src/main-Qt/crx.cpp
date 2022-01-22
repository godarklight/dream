#include "crx.h"
#ifdef USE_CONSOLEIO
# include "../linux/ConsoleIO.h"
#endif

#include "../DrmReceiver.h"

CRx::CRx(CDRMReceiver& nRx, CTRx *trx): CTRx(trx), rx(nRx), eRunState(STOPPED)
{
}

CRx::~CRx()
{
}

void
CRx::run()
{
    qDebug("Working thread started");
    try
    {
        // set the frequency from the command line or ini file
        int iFreqkHz = rx.GetParameters()->GetFrequency();
        if (iFreqkHz != -1)
            rx.SetFrequency(iFreqkHz);

#ifdef USE_CONSOLEIO
        CConsoleIO::Enter(&rx);
#endif
        // emit any initial state needed by UI and not otherwise set
        emit InputChannelChanged(int(rx.GetReceiveData()->GetInChanSel()));
        emit OutputChannelChanged(int(rx.GetWriteData()->GetOutChanSel()));

        do
        {
            rx.InitReceiverMode();
            rx.SetInStartMode();

            switch(rx.GetReceiverMode()) {
            case RM_DRM: emit drmModeStarted(); break;
            case RM_AM: emit amModeStarted(); break;
            case RM_FM: emit fmModeStarted(); break;
            case RM_NONE:;
            }

            /* Set run flag so that the thread can work */
            eRunState = RUNNING;
            do
            {
                rx.updatePosition();
                rx.process();

#ifdef USE_CONSOLEIO
                eRunState = CConsoleIO::Update();
#endif
            }
            while (eRunState == RUNNING);

            /* Restore some parameter previously set by SetInput() */
        }
        while (eRunState == RESTART);

        rx.CloseSoundInterfaces();

#ifdef USE_CONSOLEIO
        CConsoleIO::Leave();
#endif

        eRunState = STOPPED;
    }
    catch (CGenErr GenErr)
    {
        qDebug("%s", GenErr.strError.c_str());
    }
    catch (string strError)
    {
        qDebug("%s", strError.c_str());
    }
    qDebug("Receiver working thread complete");
}

void CRx::LoadSettings()
{
    rx.LoadSettings();
    QString s = QString::fromStdString(rx.GetInputDevice());
    emit InputDeviceChanged(s);
    emit soundFileChanged(s); // TODO only send if it is a file!!!
    s = QString::fromStdString(rx.GetOutputDevice());
    emit OutputDeviceChanged(s);
    emit inputSampleRateChanged(rx.GetParameters()->GetSigSampleRate());
    emit outputSampleRateChanged(rx.GetParameters()->GetAudSampleRate());
    emit soundUpscaleRatioChanged(rx.GetParameters()->GetSigUpscaleRatio());
}

void CRx::SaveSettings()
{
    rx.SaveSettings();
}

void CRx::SetInputDevice(QString s)
{
    SetInputDevice(s.toStdString());
}

void CRx::SetInputDevice(string s)
{
    rx.SetInputDevice(s);
    eRunState = RESTART;
    QString id = QString::fromStdString(rx.GetInputDevice());
    emit InputDeviceChanged(id);
    emit soundFileChanged(id); // TODO only send if it is a file!!!
}

void CRx::SetOutputDevice(QString s)
{
    SetOutputDevice(s.toStdString());
}

void CRx::SetOutputDevice(string s)
{
    cerr << "CRx::SetOutputDevice " << s << endl;
    rx.SetOutputDevice(s);
    emit OutputDeviceChanged(QString::fromStdString(rx.GetOutputDevice()));
}

void CRx::EnumerateInputs(vector<string>& names, vector<string>& descriptions, string& defaultInput)
{
    rx.EnumerateInputs(names, descriptions, defaultInput);
}

void CRx::EnumerateOutputs(vector<string>& names, vector<string>& descriptions, string& defaultOutput)
{
    rx.EnumerateOutputs(names, descriptions, defaultOutput);
}

void CRx::Start()
{
    if (eRunState == RUNNING)
        eRunState = RESTART;
}

void CRx::Restart()
{
    Start();
}

void CRx::Stop()
{
    eRunState = STOP_REQUESTED;
}

CSettings* CRx::GetSettings()
{
    return rx.GetSettings();
}

void CRx::SetSettings(CSettings* s)
{
    rx.SetSettings(s);
}

CParameter*	CRx::GetParameters()
{
    return rx.GetParameters();
}

void    CRx::GetInputPSD(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
    rx.GetReceiveData()->GetInputPSD(vecrData, vecrScale);
}

void    CRx::GetPowDenSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
    rx.GetOFDMDemod()->GetPowDenSpec(vecrData, vecrScale);
}

void    CRx::GetInputSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
    rx.GetReceiveData()->GetInputSpec(vecrData, vecrScale);
}

void    CRx::GetAudioSpec(CVector<_REAL>& vecrData, CVector<_REAL>& vecrScale)
{
    rx.GetWriteData()->GetAudioSpec(vecrData, vecrScale);
}


ERecMode CRx::GetReceiverMode()
{
    return rx.GetReceiverMode();
}

int CRx::GetInChanSel()
{
    return int(rx.GetReceiveData()->GetInChanSel());
}

int CRx::GetMSCMLInitNumIterations()
{
    return rx.GetMSCMLC()->GetInitNumIterations();
}

CPlotManager* CRx::GetPlotManager()
{
    return rx.GetPlotManager();
}

_REAL CRx::ConvertFrequency(_REAL rFrequency, bool bInvert) const
{
    return rx.GetReceiveData()->ConvertFrequency(rFrequency, bInvert);
}

void CRx::GetMSCMLCVectorSpace(CVector<_COMPLEX>& v)
{
    rx.GetMSCMLC()->GetVectorSpace(v);
}

void CRx::GetSDCMLCVectorSpace(CVector<_COMPLEX>& v)
{
    rx.GetSDCMLC()->GetVectorSpace(v);
}

void CRx::GetFACMLCVectorSpace(CVector<_COMPLEX>& v)
{
    rx.GetFACMLC()->GetVectorSpace(v);
}

bool CRx::inputIsRSCI()
{
    return rx.GetRSIIn()->GetInEnabled();
}

bool CRx::isWriteWaveFile()
{
    return rx.GetWriteData()->GetIsWriteWaveFile();
}

bool CRx::isAudioMuted()
{
    return rx.GetWriteData()->GetMuteAudio();
}

bool CRx::isIntefererConsiderationEnabled()
{
    return rx.GetIntCons();
}

bool CRx::isFrequencySyncAcquisitionFilterEnabled() const
{
    return rx.GetFreqSyncAcq()->GetRecFilter();
}

bool CRx::isSpectrumFlipped()
{
    return rx.GetReceiveData()->GetFlippedSpectrum();
}

void CRx::GetAMBWParameters(double& rCenterFreq, double& rBW)
{
    rx.GetAMDemod()->GetBWParameters(rCenterFreq, rBW);
}

double CRx::GetAMMixerFrequencyOffset() const
{
    return rx.GetAMDemod()->GetCurMixFreqOffs();
}

int     CRx::GetAMNoiseReductionLevel()
{
    return rx.GetAMDemod()->GetNoiRedLevel();
}

ENoiRedType CRx::GetAMNoiseReductionType()
{
    return rx.GetAMDemod()->GetNoiRedType();
}

EDemodType  CRx::GetAMDemodulationType()
{
    return rx.GetAMDemod()->GetDemodType();
}

EAmAgcType CRx::GetAMAGCType()
{
    return rx.GetAMDemod()->GetAGCType();
}

int CRx::GetAMFilterBW()
{
    return rx.GetAMDemod()->GetFilterBW();
}

bool CRx::GetAMPLLPhase(_REAL& r)
{
    return rx.GetAMDemod()->GetPLLPhase(r);
}

bool CRx::isAMAutoFrequencyAcquisitionEnabled()
{
    return rx.GetAMDemod()->AutoFreqAcqEnabled();
}

bool CRx::isAMPLLEnabled()
{
    return rx.GetAMDemod()->PLLEnabled();
}

bool CRx::GetAMSSPLLPhase(_REAL& r)
{
    return rx.GetAMSSPhaseDemod()->GetPLLPhase(r);
}

int CRx::GetAMSSPercentageDataEntityGroupComplete()
{
    return rx.GetAMSSDecode()->GetPercentageDataEntityGroupComplete();
}

char* CRx::GetAMSSDataEntityGroupStatus()
{
    return rx.GetAMSSDecode()->GetDataEntityGroupStatus();
}

int CRx::GetAMSSCurrentBlock()
{
    return rx.GetAMSSDecode()->GetCurrentBlock();
}

char* CRx::GetAMSSCurrentBlockBits()
{
    return rx.GetAMSSDecode()->GetCurrentBlockBits();
}

bool CRx::GetAMSSBlock1Status()
{
    return rx.GetAMSSDecode()->GetBlock1Status();
}

EAMSSBlockLockStat CRx::GetAMSSLockStatus()
{
    return rx.GetAMSSDecode()->GetLockStatus();
}

bool CRx::CanDecode(int n)
{
    return rx.GetAudSorceDec()->CanDecode(CAudioParam::EAudCod(n));
}

ETypeIntTime CRx::GetTimeInterpolationAlgorithm() const
{
    return rx.GetTimeInterpolationAlgorithm();
}

ETypeIntFreq CRx::GetFrequencyInterpolationAlgorithm() const
{
    return rx.GetFrequencyInterpolationAlgorithm();
}

ETypeTiSyncTrac CRx::GetTimeSyncTrackingType()
{
    return rx.GetTiSyncTracType();
}

CDataDecoder* CRx::GetDataDecoder()
{
    return rx.GetDataDecoder();
}

bool CRx::GetReverbEffect()
{
    return rx.GetAudSorceDec()->GetReverbEffect();
}

EAcqStat CRx::GetAcquisitionState()
{
    return rx.GetAcquiState();
}

int CRx::GetFrequency()
{
    return rx.GetFrequency();
}

void CRx::StartWriteWaveFile(string s)
{
    rx.GetWriteData()->StartWriteWaveFile(s);
}

void CRx::StopWriteWaveFile()
{
    rx.GetWriteData()->StopWriteWaveFile();
}

void CRx::SetTimeInterpolationAlgorithm(ETypeIntTime e)
{
    rx.SetTimeInt(e);
}

void CRx::SetFrequencyInterpolationAlgorithm(ETypeIntFreq e)
{
    rx.SetFreqInt(e);
}

void CRx::SetTimeSyncTrackingType(ETypeTiSyncTrac e)
{
    rx.SetTiSyncTracType(e);
}

void CRx::SetNumMSCMLCIterations(int n)
{
    rx.GetMSCMLC()->SetNumIterations(n);
}

void CRx::SetFlipSpectrum(bool b)
{
    rx.GetReceiveData()->SetFlippedSpectrum(b);
}

void CRx::SetFrequencySyncAcquisitionFilter(bool b)
{
    rx.GetFreqSyncAcq()->SetRecFilter(b);
    eRunState = RESTART;
}

void CRx::SetConsiderInterferer(bool b)
{
    rx.SetIntCons(b);
}

void CRx::MuteAudio(bool b)
{
    rx.GetWriteData()->MuteAudio(b);
}

void CRx::SetReverberationEffect(bool b)
{
    return rx.GetAudSorceDec()->SetReverbEffect(b);
}

void CRx::SetReceiverMode(ERecMode e)
{
    rx.SetReceiverMode(e);
    eRunState = RESTART;
}

void CRx::SetAMDemodulationType(EDemodType e)
{
    rx.GetAMDemod()->SetDemodType(e);
}

void CRx::SetAMFilterBW(int n)
{
    rx.GetAMDemod()->SetFilterBW(n);
}

void CRx::SetAMAGCType(EAmAgcType e)
{
    rx.GetAMDemod()->SetAGCType(e);
}

void CRx::SetAMNoiseReductionType(ENoiRedType e)
{
    rx.GetAMDemod()->SetNoiRedType(e);
}

void CRx::SetAMNoiseReductionLevel(int n)
{
    rx.GetAMDemod()->SetNoiRedLevel(n);
}

void CRx::SetFrequency(int n)
{
    rx.SetFrequency(n);
}

void CRx::SetAMDemodAcq(_REAL r)
{
    rx.SetAMDemodAcq(r);
}

void CRx::EnableAMPLL(bool b)
{
    rx.GetAMDemod()->EnablePLL(b);
}

void CRx::EnableAutoFrequenctAcquisition(bool b)
{
    rx.GetAMDemod()->EnableAutoFreqAcq(b);
}

void CRx::onSoundInChannelChanged(int e)
{
    rx.GetReceiveData()->SetInChanSel(EInChanSel(e));
    emit InputChannelChanged(e);
}

void CRx::onSoundOutChannelChanged(EOutChanSel e)
{
    rx.GetWriteData()->SetOutChanSel(e);
    emit OutputChannelChanged(int(e));
}

void  CRx::onSoundSampleRateChanged(int n)
{
    rx.GetParameters()->SetNewAudSampleRate(n);
    Restart();
    emit outputSampleRateChanged(n);
}

void CRx::SetSoundSignalUpscale(int n)
{
    rx.GetParameters()->SetNewSigUpscaleRatio(n);
    Restart();
    emit soundUpscaleRatioChanged(n);
}
