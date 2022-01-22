#include "soundin.h"
#include <SLES/OpenSLES.h>

extern SLObjectItf engineObject;

const int MAX_NUMBER_INPUT_DEVICES = 10;

static const char* SLESErrorToString(SLresult result)
{
    switch (result)
    {
    case SL_RESULT_SUCCESS: return "SUCCESS";
    case SL_RESULT_PRECONDITIONS_VIOLATED: return
    "SL_RESULT_PRECONDITIONS_VIOLATED";
    case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
    case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
    case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
    case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
    case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";
    case SL_RESULT_BUFFER_INSUFFICIENT: return "SL_RESULT_BUFFER_INSUFFICIENT";
    case SL_RESULT_CONTENT_CORRUPTED: return "SL_RESULT_CONTENT_CORRUPTED";
    case SL_RESULT_CONTENT_UNSUPPORTED: return "SL_RESULT_CONTENT_UNSUPPORTED";
    case SL_RESULT_CONTENT_NOT_FOUND: return "SL_RESULT_CONTENT_NOT_FOUND";
    case SL_RESULT_PERMISSION_DENIED: return "SL_RESULT_PERMISSION_DENIED";
    case SL_RESULT_FEATURE_UNSUPPORTED: return
    "SL_RESULT_FEATURE_UNSUPPORTED";
    case SL_RESULT_INTERNAL_ERROR: return "SL_RESULT_INTERNAL_ERROR";
    case SL_RESULT_UNKNOWN_ERROR: return "SL_RESULT_UNKNOWN_ERROR";
    case SL_RESULT_OPERATION_ABORTED: return "SL_RESULT_OPERATION_ABORTED";
    case SL_RESULT_CONTROL_LOST: return "SL_RESULT_CONTROL_LOST";
    default: return "Unknown";
    }
}



COpenSLESIn::COpenSLESIn():currentDevice("")
{
}

COpenSLESIn::~COpenSLESIn()
{

}

void COpenSLESIn::Enumerate(vector<string>& names, vector<string>& descriptions)
{
    SLuint32 InputDeviceIDs[MAX_NUMBER_INPUT_DEVICES];
    SLint32 numInputs = 0;
    //(*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    /* Get the Audio IO DEVICE CAPABILITIES interface */
    SLAudioIODeviceCapabilitiesItf AudioIODeviceCapabilitiesItf;
    SLresult res = (*engineObject)->GetInterface(engineObject, SL_IID_AUDIOIODEVICECAPABILITIES, (void*)&AudioIODeviceCapabilitiesItf);
    if(res!=SL_RESULT_SUCCESS) {
        string em = "Can't get audio input device capabilities ";
        em += SLESErrorToString(res);
        qDebug(em.c_str());
        return;
    }
    numInputs = MAX_NUMBER_INPUT_DEVICES;
    res = (*AudioIODeviceCapabilitiesItf)->GetAvailableAudioInputs( AudioIODeviceCapabilitiesItf, &numInputs, InputDeviceIDs);
    if(res!=SL_RESULT_SUCCESS) {
        string em = "Can't enumerate audio input devices ";
        em += SLESErrorToString(res);
        qDebug(em.c_str());
        return;
    }
    for (SLint32 i=0;i<numInputs; i++)
    {
        SLAudioInputDescriptor AudioInputDescriptor;
        res = (*AudioIODeviceCapabilitiesItf)->QueryAudioInputCapabilities(
                    AudioIODeviceCapabilitiesItf, InputDeviceIDs[i], &AudioInputDescriptor
              );
        //CheckErr(res);
        names.push_back((char*)AudioInputDescriptor.deviceName);
        descriptions.push_back("(tbd)");
    }

}

void COpenSLESIn::SetDev(string sNewDevice)
{
    currentDevice = sNewDevice;

}

string COpenSLESIn::GetDev()
{
    return currentDevice;
}

int COpenSLESIn::GetSampleRate()
{
    return 48000;
}

bool COpenSLESIn::Init(int iNewSampleRate, int iNewBufferSize, bool bNewBlocking)
{
    (void)iNewSampleRate;
    (void)iNewBufferSize;
    (void)bNewBlocking;
    return false;
}

bool COpenSLESIn::Read(CVector<short>& psData)
{
    (void)psData;
    return false;
}

void COpenSLESIn::Close()
{

}
/*

Grokbase › Groups › Android › android-ndk › August 2012FAQ
Groups
 Users
 Badges

[android-ndk] Errors with OpenSLES recording and the emulator
  Brendon Costa
Aug 25, 2012 at 2:12 pm

Hi all,

I have been playing with OpenSLES on Android and all seems to be fine
except that I have a problem where the emulator fails to record audio. I
have a small test app based on the native-audio sample (I have cut down to
relevant portions and pasted below) which works fine on my Android device
(X10 Mini Pro rooted and re-rom'ed with Cyanogen) but no matter what I do I
cant get it to work on an emulator.

I have tried creating emulators of different versions, and explicitly
enabling the audio recording option. Basically when I run the app on the
emulator I get the following errors from a logcat:

W/libOpenSLES( 459): class Engine interface 0 requested but unavailable
MPH=9
E/AudioRecord( 459): Could not get audio input for record source 1
E/libOpenSLES( 459): android_audioRecorder_realize(0x52438) error creating
AudioRecord object

I am not using JNI for the moment (though did write a small JNI app that
used this code to test if there was some permissions thing requiring a
manifest but had the same problem). Instead I am basically executing the
binary directly under a root shell on the device using adb.

When running the app I get the following output (error happens when
realizing the recorder object):

Running: ./clienttest
result: 9 : SL_RESULT_CONTENT_UNSUPPORTED
assertion "SL_RESULT_SUCCESS == result" failed: file
"jni/../../../../AxonCore/make/android/jni/../../../../AxonCore/src/clienttest.cpp",
line 122, function "int main()"
[1] Segmentation fault ./clienttest

I figures the problem was an unsupported sample rate or format, but I have
tried all defined sample rates, and a bunch of other format specifiers for
the buffer queue and nothing seems to fix the problem.

Any thoughts?



// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <assert.h>
#include <iostream>
#include <math.h>
#include <limits>
#include <vector>

static const int CHANNELS = 1; // If not 1 need to set the SLDataFormat_PCM
channel mask correctly
static const int SAMPLE_RATE = SL_SAMPLINGRATE_16 / 1000;
static const int TOTAL_BUFFERS = 2;
static const unsigned int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * 20 / 1000;

std::vector<std::vector<short> > record_buffers;
unsigned int next_record_buffer_index = 0;

void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
std::vector<short>& buffer(record_buffers[next_record_buffer_index %
TOTAL_BUFFERS]);
++next_record_buffer_index;


SLresult result;
result = (*bq)->Enqueue(bq, &buffer[0], buffer.size()*sizeof(short));
if (SL_RESULT_SUCCESS != result)
{
std::cerr << "Failed to enqueue record buffer: " <<
next_record_buffer_index << std::endl;
}
}

static const char* SLESErrorToString(SLresult result)
{
switch (result)
{
case SL_RESULT_SUCCESS: return "SUCCESS";
case SL_RESULT_PRECONDITIONS_VIOLATED: return
"SL_RESULT_PRECONDITIONS_VIOLATED";
case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";
case SL_RESULT_BUFFER_INSUFFICIENT: return "SL_RESULT_BUFFER_INSUFFICIENT";
case SL_RESULT_CONTENT_CORRUPTED: return "SL_RESULT_CONTENT_CORRUPTED";
case SL_RESULT_CONTENT_UNSUPPORTED: return "SL_RESULT_CONTENT_UNSUPPORTED";
case SL_RESULT_CONTENT_NOT_FOUND: return "SL_RESULT_CONTENT_NOT_FOUND";
case SL_RESULT_PERMISSION_DENIED: return "SL_RESULT_PERMISSION_DENIED";
case SL_RESULT_FEATURE_UNSUPPORTED: return "SL_RESULT_FEATURE_UNSUPPORTED";
case SL_RESULT_INTERNAL_ERROR: return "SL_RESULT_INTERNAL_ERROR";
case SL_RESULT_UNKNOWN_ERROR: return "SL_RESULT_UNKNOWN_ERROR";
case SL_RESULT_OPERATION_ABORTED: return "SL_RESULT_OPERATION_ABORTED";
case SL_RESULT_CONTROL_LOST: return "SL_RESULT_CONTROL_LOST";
default: return "Unknown";
}
}

int main()
{
record_buffers.resize(TOTAL_BUFFERS);
for (size_t i = 0; i < record_buffers.size(); ++i)
{
record_buffers[i].resize(BUFFER_SIZE, 0);
}
next_record_buffer_index = 0;


SLresult result;

const SLInterfaceID engine_ids[] = {SL_IID_AUDIOIODEVICECAPABILITIES};
const SLboolean engine_req[] = {SLbool_false};
// create engine
SLObjectItf engineObject = nullptr;
result = slCreateEngine(&engineObject, 0, nullptr, 1, engine_ids, engine_req);
assert(SL_RESULT_SUCCESS == result);

// realize the engine
result = (*engineObject)->Realize(engineObject, SLbool_false);
assert(SL_RESULT_SUCCESS == result);

// get the engine interface, which is needed in order to create other
objects
SLEngineItf engineEngine;
result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
&engineEngine);
assert(SL_RESULT_SUCCESS == result);

// recorder interfaces
SLObjectItf recorderObject = nullptr;
SLRecordItf recorderRecord;
SLAndroidSimpleBufferQueueItf recorderBufferQueue;
SLDataFormat_PCM rec_format_pcm =
{
SL_DATAFORMAT_PCM // Format type
, CHANNELS // Channels
, SL_SAMPLINGRATE_16 // (in millihertz apparently)
, SL_PCMSAMPLEFORMAT_FIXED_16 // bits per sample
, SL_PCMSAMPLEFORMAT_FIXED_16 // container size
, SL_SPEAKER_FRONT_CENTER // channel mask
, SL_BYTEORDER_LITTLEENDIAN // endianess
};

// configure audio source
SLDataLocator_IODevice rec_loc_dev = {SL_DATALOCATOR_IODEVICE,
SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr};
SLDataSource rec_audioSrc = {&rec_loc_dev, nullptr};

// configure audio sink
SLDataLocator_AndroidSimpleBufferQueue rec_loc_bq =
{SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, TOTAL_BUFFERS};
SLDataSink rec_audioSnk = {&rec_loc_bq, &rec_format_pcm};

// create audio recorder
// (requires the RECORD_AUDIO permission)
const SLInterfaceID rec_id[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
const SLboolean rec_req[] = {SLbool_true};
result = (*engineEngine)->CreateAudioRecorder(engineEngine,
&recorderObject, &rec_audioSrc, &rec_audioSnk, 1, rec_id, rec_req);
assert(SL_RESULT_SUCCESS == result);

// realize the audio recorder
result = (*recorderObject)->Realize(recorderObject, SLbool_false);
std::cerr << "result: " << result << " : " << SLESErrorToString(result) <<
std::endl;
assert(SL_RESULT_SUCCESS == result);

// get the record interface
result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD,
&recorderRecord);
assert(SL_RESULT_SUCCESS == result);

// get the buffer queue interface
result = (*recorderObject)->GetInterface(recorderObject,
SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);

// register callback on the buffer queue
result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,
bqRecorderCallback, nullptr);
assert(SL_RESULT_SUCCESS == result);
// in case already recording, stop recording and clear buffer queue
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_STOPPED);
assert(SL_RESULT_SUCCESS == result);
result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);


for (size_t i = 0; i < TOTAL_BUFFERS; ++i)
{
// @todo bcost : Enqueue should have size * sizeof(short) BUT DOES NOT
//result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
recorderBuffer, RECORDER_FRAMES * sizeof(short));
result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
&record_buffers[i][0], record_buffers[i].size()*sizeof(short));
assert(SL_RESULT_SUCCESS == result);
}

// start recording
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_RECORDING);
assert(SL_RESULT_SUCCESS == result);
// Start main loop
std::cerr << "Starting main loop" << std::endl;
while (true)
{
usleep(100000);
}
// @todo : Test app need to add cleanup and proper exit
return 0;
}


--
You received this message because you are subscribed to the Google Groups "android-ndk" group.
To post to this group, send email to android-ndk@googlegroups.com.
To unsubscribe from this group, send email to android-ndk+unsubscribe@googlegroups.com.
For more options, visit this group at http://groups.google.com/group/android-ndk?hl=en.
reply


Search Discussions

 All Groups  Android  android-ndk
5 responsesNested Oldest
  Doug Emulators have never done well with recording audio. Best stick with your device. Or mock your audio recording source if you want to use the emulator. Doug -- You received this message because you are subscribed to the Google Groups "android-ndk" group. To view this discussion on the web visit https://groups.google.com/d/msg/android-ndk/-/OrFmZ47xNAwJ. To post to this group, send email to android-ndk@googlegroups.com. To unsubscribe from this group, send email to
  Doug  at Aug 26, 2012 at 5:15 pm  ⇧

Emulators have never done well with recording audio. Best stick with your
device. Or mock your audio recording source if you want to use the
emulator.

Doug

On Friday, August 24, 2012 9:09:45 PM UTC-7, Brendon Costa wrote:

Hi all,

I have been playing with OpenSLES on Android and all seems to be fine
except that I have a problem where the emulator fails to record audio. I
have a small test app based on the native-audio sample (I have cut down to
relevant portions and pasted below) which works fine on my Android device
(X10 Mini Pro rooted and re-rom'ed with Cyanogen) but no matter what I do I
cant get it to work on an emulator.

I have tried creating emulators of different versions, and explicitly
enabling the audio recording option. Basically when I run the app on the
emulator I get the following errors from a logcat:

W/libOpenSLES( 459): class Engine interface 0 requested but unavailable
MPH=9
E/AudioRecord( 459): Could not get audio input for record source 1
E/libOpenSLES( 459): android_audioRecorder_realize(0x52438) error
creating AudioRecord object

I am not using JNI for the moment (though did write a small JNI app that
used this code to test if there was some permissions thing requiring a
manifest but had the same problem). Instead I am basically executing the
binary directly under a root shell on the device using adb.

When running the app I get the following output (error happens when
realizing the recorder object):

Running: ./clienttest
result: 9 : SL_RESULT_CONTENT_UNSUPPORTED
assertion "SL_RESULT_SUCCESS == result" failed: file
"jni/../../../../AxonCore/make/android/jni/../../../../AxonCore/src/clienttest.cpp",
line 122, function "int main()"
[1] Segmentation fault ./clienttest

I figures the problem was an unsupported sample rate or format, but I have
tried all defined sample rates, and a bunch of other format specifiers for
the buffer queue and nothing seems to fix the problem.

Any thoughts?



// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <assert.h>
#include <iostream>
#include <math.h>
#include <limits>
#include <vector>

static const int CHANNELS = 1; // If not 1 need to set the
SLDataFormat_PCM channel mask correctly
static const int SAMPLE_RATE = SL_SAMPLINGRATE_16 / 1000;
static const int TOTAL_BUFFERS = 2;
static const unsigned int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * 20 / 1000;

std::vector<std::vector<short> > record_buffers;
unsigned int next_record_buffer_index = 0;

void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
std::vector<short>& buffer(record_buffers[next_record_buffer_index %
TOTAL_BUFFERS]);
++next_record_buffer_index;


SLresult result;
result = (*bq)->Enqueue(bq, &buffer[0], buffer.size()*sizeof(short));
if (SL_RESULT_SUCCESS != result)
{
std::cerr << "Failed to enqueue record buffer: " <<
next_record_buffer_index << std::endl;
}
}

static const char* SLESErrorToString(SLresult result)
{
switch (result)
{
case SL_RESULT_SUCCESS: return "SUCCESS";
case SL_RESULT_PRECONDITIONS_VIOLATED: return
"SL_RESULT_PRECONDITIONS_VIOLATED";
case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";
case SL_RESULT_BUFFER_INSUFFICIENT: return "SL_RESULT_BUFFER_INSUFFICIENT";
case SL_RESULT_CONTENT_CORRUPTED: return "SL_RESULT_CONTENT_CORRUPTED";
case SL_RESULT_CONTENT_UNSUPPORTED: return "SL_RESULT_CONTENT_UNSUPPORTED";
case SL_RESULT_CONTENT_NOT_FOUND: return "SL_RESULT_CONTENT_NOT_FOUND";
case SL_RESULT_PERMISSION_DENIED: return "SL_RESULT_PERMISSION_DENIED";
case SL_RESULT_FEATURE_UNSUPPORTED: return
"SL_RESULT_FEATURE_UNSUPPORTED";
case SL_RESULT_INTERNAL_ERROR: return "SL_RESULT_INTERNAL_ERROR";
case SL_RESULT_UNKNOWN_ERROR: return "SL_RESULT_UNKNOWN_ERROR";
case SL_RESULT_OPERATION_ABORTED: return "SL_RESULT_OPERATION_ABORTED";
case SL_RESULT_CONTROL_LOST: return "SL_RESULT_CONTROL_LOST";
default: return "Unknown";
}
}

int main()
{
record_buffers.resize(TOTAL_BUFFERS);
for (size_t i = 0; i < record_buffers.size(); ++i)
{
record_buffers[i].resize(BUFFER_SIZE, 0);
}
next_record_buffer_index = 0;


SLresult result;

const SLInterfaceID engine_ids[] = {SL_IID_AUDIOIODEVICECAPABILITIES};
const SLboolean engine_req[] = {SLbool_false};
// create engine
SLObjectItf engineObject = nullptr;
result = slCreateEngine(&engineObject, 0, nullptr, 1, engine_ids,
engine_req);
assert(SL_RESULT_SUCCESS == result);

// realize the engine
result = (*engineObject)->Realize(engineObject, SLbool_false);
assert(SL_RESULT_SUCCESS == result);

// get the engine interface, which is needed in order to create other
objects
SLEngineItf engineEngine;
result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
&engineEngine);
assert(SL_RESULT_SUCCESS == result);

// recorder interfaces
SLObjectItf recorderObject = nullptr;
SLRecordItf recorderRecord;
SLAndroidSimpleBufferQueueItf recorderBufferQueue;
SLDataFormat_PCM rec_format_pcm =
{
SL_DATAFORMAT_PCM // Format type
, CHANNELS // Channels
, SL_SAMPLINGRATE_16 // (in millihertz apparently)
, SL_PCMSAMPLEFORMAT_FIXED_16 // bits per sample
, SL_PCMSAMPLEFORMAT_FIXED_16 // container size
, SL_SPEAKER_FRONT_CENTER // channel mask
, SL_BYTEORDER_LITTLEENDIAN // endianess
};

// configure audio source
SLDataLocator_IODevice rec_loc_dev = {SL_DATALOCATOR_IODEVICE,
SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr};
SLDataSource rec_audioSrc = {&rec_loc_dev, nullptr};

// configure audio sink
SLDataLocator_AndroidSimpleBufferQueue rec_loc_bq =
{SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, TOTAL_BUFFERS};
SLDataSink rec_audioSnk = {&rec_loc_bq, &rec_format_pcm};

// create audio recorder
// (requires the RECORD_AUDIO permission)
const SLInterfaceID rec_id[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
const SLboolean rec_req[] = {SLbool_true};
result = (*engineEngine)->CreateAudioRecorder(engineEngine,
&recorderObject, &rec_audioSrc, &rec_audioSnk, 1, rec_id, rec_req);
assert(SL_RESULT_SUCCESS == result);

// realize the audio recorder
result = (*recorderObject)->Realize(recorderObject, SLbool_false);
std::cerr << "result: " << result << " : " << SLESErrorToString(result) <<
std::endl;
assert(SL_RESULT_SUCCESS == result);

// get the record interface
result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD,
&recorderRecord);
assert(SL_RESULT_SUCCESS == result);

// get the buffer queue interface
result = (*recorderObject)->GetInterface(recorderObject,
SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);

// register callback on the buffer queue
result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,
bqRecorderCallback, nullptr);
assert(SL_RESULT_SUCCESS == result);
// in case already recording, stop recording and clear buffer queue
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_STOPPED);
assert(SL_RESULT_SUCCESS == result);
result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);


for (size_t i = 0; i < TOTAL_BUFFERS; ++i)
{
// @todo bcost : Enqueue should have size * sizeof(short) BUT DOES NOT
//result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
recorderBuffer, RECORDER_FRAMES * sizeof(short));
result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
&record_buffers[i][0], record_buffers[i].size()*sizeof(short));
assert(SL_RESULT_SUCCESS == result);
}

// start recording
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_RECORDING);
assert(SL_RESULT_SUCCESS == result);
// Start main loop
std::cerr << "Starting main loop" << std::endl;
while (true)
{
usleep(100000);
}
// @todo : Test app need to add cleanup and proper exit
return 0;
}



--
You received this message because you are subscribed to the Google Groups "android-ndk" group.
To view this discussion on the web visit https://groups.google.com/d/msg/android-ndk/-/OrFmZ47xNAwJ.
To post to this group, send email to android-ndk@googlegroups.com.
To unsubscribe from this group, send email to android-ndk+unsubscribe@googlegroups.com.
For more options, visit this group at http://groups.google.com/group/android-ndk?hl=en.
reply | permalink
  Saeed Abbassi Hi Brendon I've tested this example on OpenSL-ES and it works fine, Android audio streaming with OpenSL ES and the NDK Pay attention to getting audio permission before recording, good luck saeed Le samedi 25 août 2012 08:39:45 UTC+4:30, Brendon Costa a écrit : -- You received this message because you are subscribed to the Google Groups "android-ndk" group. To view this discussion on the web visit https://groups.google.com/d/msg/android-ndk/-/svHQumNGJ04J. To post to this group, send email to
  Saeed Abbassi  at Sep 6, 2012 at 6:35 am  ⇧

Hi Brendon

I've tested this example on OpenSL-ES and it works fine,

Android audio streaming with OpenSL ES and the NDK

Pay attention to getting audio permission before recording,

good luck
saeed


Le samedi 25 août 2012 08:39:45 UTC+4:30, Brendon Costa a écrit :

Hi all,

I have been playing with OpenSLES on Android and all seems to be fine
except that I have a problem where the emulator fails to record audio. I
have a small test app based on the native-audio sample (I have cut down to
relevant portions and pasted below) which works fine on my Android device
(X10 Mini Pro rooted and re-rom'ed with Cyanogen) but no matter what I do I
cant get it to work on an emulator.

I have tried creating emulators of different versions, and explicitly
enabling the audio recording option. Basically when I run the app on the
emulator I get the following errors from a logcat:

W/libOpenSLES( 459): class Engine interface 0 requested but unavailable
MPH=9
E/AudioRecord( 459): Could not get audio input for record source 1
E/libOpenSLES( 459): android_audioRecorder_realize(0x52438) error
creating AudioRecord object

I am not using JNI for the moment (though did write a small JNI app that
used this code to test if there was some permissions thing requiring a
manifest but had the same problem). Instead I am basically executing the
binary directly under a root shell on the device using adb.

When running the app I get the following output (error happens when
realizing the recorder object):

Running: ./clienttest
result: 9 : SL_RESULT_CONTENT_UNSUPPORTED
assertion "SL_RESULT_SUCCESS == result" failed: file
"jni/../../../../AxonCore/make/android/jni/../../../../AxonCore/src/clienttest.cpp",
line 122, function "int main()"
[1] Segmentation fault ./clienttest

I figures the problem was an unsupported sample rate or format, but I have
tried all defined sample rates, and a bunch of other format specifiers for
the buffer queue and nothing seems to fix the problem.

Any thoughts?



// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <assert.h>
#include <iostream>
#include <math.h>
#include <limits>
#include <vector>

static const int CHANNELS = 1; // If not 1 need to set the
SLDataFormat_PCM channel mask correctly
static const int SAMPLE_RATE = SL_SAMPLINGRATE_16 / 1000;
static const int TOTAL_BUFFERS = 2;
static const unsigned int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * 20 / 1000;

std::vector<std::vector<short> > record_buffers;
unsigned int next_record_buffer_index = 0;

void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
std::vector<short>& buffer(record_buffers[next_record_buffer_index %
TOTAL_BUFFERS]);
++next_record_buffer_index;


SLresult result;
result = (*bq)->Enqueue(bq, &buffer[0], buffer.size()*sizeof(short));
if (SL_RESULT_SUCCESS != result)
{
std::cerr << "Failed to enqueue record buffer: " <<
next_record_buffer_index << std::endl;
}
}

int main()
{
record_buffers.resize(TOTAL_BUFFERS);
for (size_t i = 0; i < record_buffers.size(); ++i)
{
record_buffers[i].resize(BUFFER_SIZE, 0);
}
next_record_buffer_index = 0;


SLresult result;

const SLInterfaceID engine_ids[] = {SL_IID_AUDIOIODEVICECAPABILITIES};
const SLboolean engine_req[] = {SLbool_false};
// create engine
SLObjectItf engineObject = nullptr;
result = slCreateEngine(&engineObject, 0, nullptr, 1, engine_ids,
engine_req);
assert(SL_RESULT_SUCCESS == result);

// realize the engine
result = (*engineObject)->Realize(engineObject, SLbool_false);
assert(SL_RESULT_SUCCESS == result);

// get the engine interface, which is needed in order to create other
objects
SLEngineItf engineEngine;
result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
&engineEngine);
assert(SL_RESULT_SUCCESS == result);

// recorder interfaces
SLObjectItf recorderObject = nullptr;
SLRecordItf recorderRecord;
SLAndroidSimpleBufferQueueItf recorderBufferQueue;
SLDataFormat_PCM rec_format_pcm =
{
SL_DATAFORMAT_PCM // Format type
, CHANNELS // Channels
, SL_SAMPLINGRATE_16 // (in millihertz apparently)
, SL_PCMSAMPLEFORMAT_FIXED_16 // bits per sample
, SL_PCMSAMPLEFORMAT_FIXED_16 // container size
, SL_SPEAKER_FRONT_CENTER // channel mask
, SL_BYTEORDER_LITTLEENDIAN // endianess
};

// configure audio source
SLDataLocator_IODevice rec_loc_dev = {SL_DATALOCATOR_IODEVICE,
SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr};
SLDataSource rec_audioSrc = {&rec_loc_dev, nullptr};

// configure audio sink
SLDataLocator_AndroidSimpleBufferQueue rec_loc_bq =
{SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, TOTAL_BUFFERS};
SLDataSink rec_audioSnk = {&rec_loc_bq, &rec_format_pcm};

// create audio recorder
// (requires the RECORD_AUDIO permission)
const SLInterfaceID rec_id[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
const SLboolean rec_req[] = {SLbool_true};
result = (*engineEngine)->CreateAudioRecorder(engineEngine,
&recorderObject, &rec_audioSrc, &rec_audioSnk, 1, rec_id, rec_req);
assert(SL_RESULT_SUCCESS == result);

// realize the audio recorder
result = (*recorderObject)->Realize(recorderObject, SLbool_false);
std::cerr << "result: " << result << " : " << SLESErrorToString(result) <<
std::endl;
assert(SL_RESULT_SUCCESS == result);

// get the record interface
result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD,
&recorderRecord);
assert(SL_RESULT_SUCCESS == result);

// get the buffer queue interface
result = (*recorderObject)->GetInterface(recorderObject,
SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);

// register callback on the buffer queue
result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,
bqRecorderCallback, nullptr);
assert(SL_RESULT_SUCCESS == result);
// in case already recording, stop recording and clear buffer queue
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_STOPPED);
assert(SL_RESULT_SUCCESS == result);
result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);


for (size_t i = 0; i < TOTAL_BUFFERS; ++i)
{
// @todo bcost : Enqueue should have size * sizeof(short) BUT DOES NOT
//result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
recorderBuffer, RECORDER_FRAMES * sizeof(short));
result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
&record_buffers[i][0], record_buffers[i].size()*sizeof(short));
assert(SL_RESULT_SUCCESS == result);
}

// start recording
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_RECORDING);
assert(SL_RESULT_SUCCESS == result);
// Start main loop
std::cerr << "Starting main loop" << std::endl;
while (true)
{
usleep(100000);
}
// @todo : Test app need to add cleanup and proper exit
return 0;
}



--
You received this message because you are subscribed to the Google Groups "android-ndk" group.
To view this discussion on the web visit https://groups.google.com/d/msg/android-ndk/-/svHQumNGJ04J.
To post to this group, send email to android-ndk@googlegroups.com.
To unsubscribe from this group, send email to android-ndk+unsubscribe@googlegroups.com.
For more options, visit this group at http://groups.google.com/group/android-ndk?hl=en.
reply | permalink
  Saeed Abbassi When running the executable on android, debugging using gdbserver could help a lot to back trace your program and debug give it a try 2012/9/5 Saeed Abbassi < [email protected] > -- You received this message because you are subscribed to the Google Groups "android-ndk" group. To post to this group, send email to android-ndk@googlegroups.com. To unsubscribe from this group, send email to android-ndk+unsubscribe@googlegroups.com. For more options, visit this group at
  Saeed Abbassi  at Sep 7, 2012 at 1:59 am  ⇧

When running the executable on android,
debugging using gdbserver could help a lot to back trace your program and
debug

give it a try

2012/9/5 Saeed Abbassi <sae...@...com>

When running the a
--
You received this message because you are subscribed to the Google Groups "android-ndk" group.
To post to this group, send email to android-ndk@googlegroups.com.
To unsubscribe from this group, send email to android-ndk+unsubscribe@googlegroups.com.
For more options, visit this group at http://groups.google.com/group/android-ndk?hl=en.
reply | permalink
  Brendon Costa Hi Saeed, Thanks for the response. You mentioned permissions. I did try to create a JNI layer that wrapped the code above and call it from a Java app in which I gave the recording permissions in the manifest and it still didn't work for me. I used: "" Unless there is a way to request the permissions from within the NDK, I am not sure what else I am missing. Would you be willing to provide a tarball or something of the project you got this working in? So you got this to work with the emulator?
  Brendon Costa  at Sep 7, 2012 at 2:00 am  ⇧

Hi Saeed,

Thanks for the response.

You mentioned permissions. I did try to create a JNI layer that wrapped the
code above and call it from a Java app in which I gave the recording
permissions in the manifest and it still didn't work for me. I used:
"<uses-permission
android:name="android.permission.RECORD_AUDIO"></uses-permission>"

Unless there is a way to request the permissions from within the NDK, I am
not sure what else I am missing.

Would you be willing to provide a tarball or something of the project you
got this working in?

So you got this to work with the emulator? What platform was the emulator
running on and how was it configured, maybe that has an effect (I am using
windows with a emulator at API Level 10, Audio recording support set to
yes)?

Thanks,
Brendon.

On Wednesday, 5 September 2012, Saeed Abbassi wrote:

Hi Brendon

I've tested this example on OpenSL-ES and it works fine,

Android audio streaming with OpenSL ES and the NDK

Pay attention to getting audio permission before recording,

good luck
saeed


Le samedi 25 août 2012 08:39:45 UTC+4:30, Brendon Costa a écrit :

Hi all,

I have been playing with OpenSLES on Android and all seems to be fine
except that I have a problem where the emulator fails to record audio. I
have a small test app based on the native-audio sample (I have cut down to
relevant portions and pasted below) which works fine on my Android device
(X10 Mini Pro rooted and re-rom'ed with Cyanogen) but no matter what I do I
cant get it to work on an emulator.

I have tried creating emulators of different versions, and explicitly
enabling the audio recording option. Basically when I run the app on the
emulator I get the following errors from a logcat:

W/libOpenSLES( 459): class Engine interface 0 requested but unavailable
MPH=9
E/AudioRecord( 459): Could not get audio input for record source 1
E/libOpenSLES( 459): android_audioRecorder_realize(**0x52438) error
creating AudioRecord object

I am not using JNI for the moment (though did write a small JNI app that
used this code to test if there was some permissions thing requiring a
manifest but had the same problem). Instead I am basically executing the
binary directly under a root shell on the device using adb.

When running the app I get the following output (error happens when
realizing the recorder object):

Running: ./clienttest
result: 9 : SL_RESULT_CONTENT_UNSUPPORTED
assertion "SL_RESULT_SUCCESS == result" failed: file
line 122, function "int main()"
[1] Segmentation fault ./clienttest

I figures the problem was an unsupported sample rate or format, but I have
tried all defined sample rates, and a bunch of other format specifiers for
the buffer queue and nothing seems to fix the problem.

Any thoughts?



// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <assert.h>
#include <iostream>
#include <math.h>
#include <limits>
#include <vector>

static const int CHANNELS = 1; // If not 1 need to set the
SLDataFormat_PCM channel mask correctly
static const int SAMPLE_RATE = SL_SAMPLINGRATE_16 / 1000;
static const int TOTAL_BUFFERS = 2;
static const unsigned int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * 20 / 1000;

std::vector<std::vector<short> > record_buffers;
unsigned int next_record_buffer_index = 0;

void bqRecorderCallback(**SLAndroidSimpleBufferQueueItf bq, void *context)
{
std::vector<short>& buffer(record_buffers[next_**record_buffer_index %
TOTAL_BUFFERS]);
++next_record_buffer_index;


SLresult result;
result = (*bq)->Enqueue(bq, &buffer[0], buffer.size()*sizeof(short));
if (SL_RESULT_SUCCESS != result)
{
std::cerr << "Failed to enqueue record buffer: " <<
next_record_buffer_index << std::endl;
}
}

static const char* SLESErrorToString(SLresult result)
{
switch (result)
{
case SL_RESULT_SUCCESS: return "SUCCESS";
case SL_RESULT_PRECONDITIONS_**VIOLATED: return "SL_RESULT_PRECONDITIONS_*
*VIOLATED";
case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";

--
You received this message because you are subscribed to the Google Groups
"android-ndk" group.
To view this discussion on the web visit
https://groups.google.com/d/msg/android-ndk/-/svHQumNGJ04J.
To post to this group, send email to android-ndk@googlegroups.com<javascript:_e({}, 'cvml', 'android-ndk@googlegroups.com');>
.
To unsubscribe from this group, send email to
android-ndk+unsubscribe@googlegroups.com <javascript:_e({}, 'cvml',
'android-ndk%2Bunsubscribe@googlegroups.com');>.
For more options, visit this group at
http://groups.google.com/group/android-ndk?hl=en.
--
You received this message because you are subscribed to the Google Groups "android-ndk" group.
To post to this group, send email to android-ndk@googlegroups.com.
To unsubscribe from this group, send email to android-ndk+unsubscribe@googlegroups.com.
For more options, visit this group at http://groups.google.com/group/android-ndk?hl=en.
reply | permalink
  Sylvain Hi Brendon, I thought I made a reply with a suggestion to your issue but apparently I did not. What I experienced is that you can create a recorder in the AVD, but it must be using a 8kHz sample rate. And the tricky part is that you should not attempt to initialize it with another sample rate before, otherwise you will never been able to initialize it successfully. So the way I ended up implementing it is by using __system_property_get to detect whether the kernel is the one used by qemu, and
  Sylvain  at Sep 7, 2012 at 1:26 pm  ⇧

Hi Brendon,

I thought I made a reply with a suggestion to your issue but apparently I
did not.

What I experienced is that you can create a recorder in the AVD, but it
must be using a 8kHz sample rate. And the tricky part is that you should
not attempt to initialize it with another sample rate before, otherwise you
will never been able to initialize it successfully. So the way I ended up
implementing it is by using __system_property_get to detect whether the
kernel is the one used by qemu, and if yes, I use 8kHz:

#include <sys/system_properties.h>
...
char outProperty[PROP_VALUE_MAX];
__system_property_get("ro.hardware", outProperty);
SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_16,
SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
if (strcmp(outProperty,"goldfish") == 0)
{
format_pcm.samplesPerSec = SL_SAMPLINGRATE_8;
}

I hope that helps

On Saturday, August 25, 2012 12:09:45 PM UTC+8, Brendon Costa wrote:

Hi all,

I have been playing with OpenSLES on Android and all seems to be fine
except that I have a problem where the emulator fails to record audio. I
have a small test app based on the native-audio sample (I have cut down to
relevant portions and pasted below) which works fine on my Android device
(X10 Mini Pro rooted and re-rom'ed with Cyanogen) but no matter what I do I
cant get it to work on an emulator.

I have tried creating emulators of different versions, and explicitly
enabling the audio recording option. Basically when I run the app on the
emulator I get the following errors from a logcat:

W/libOpenSLES( 459): class Engine interface 0 requested but unavailable
MPH=9
E/AudioRecord( 459): Could not get audio input for record source 1
E/libOpenSLES( 459): android_audioRecorder_realize(0x52438) error
creating AudioRecord object

I am not using JNI for the moment (though did write a small JNI app that
used this code to test if there was some permissions thing requiring a
manifest but had the same problem). Instead I am basically executing the
binary directly under a root shell on the device using adb.

When running the app I get the following output (error happens when
realizing the recorder object):

Running: ./clienttest
result: 9 : SL_RESULT_CONTENT_UNSUPPORTED
assertion "SL_RESULT_SUCCESS == result" failed: file
"jni/../../../../AxonCore/make/android/jni/../../../../AxonCore/src/clienttest.cpp",
line 122, function "int main()"
[1] Segmentation fault ./clienttest

I figures the problem was an unsupported sample rate or format, but I have
tried all defined sample rates, and a bunch of other format specifiers for
the buffer queue and nothing seems to fix the problem.

Any thoughts?



// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <assert.h>
#include <iostream>
#include <math.h>
#include <limits>
#include <vector>

static const int CHANNELS = 1; // If not 1 need to set the
SLDataFormat_PCM channel mask correctly
static const int SAMPLE_RATE = SL_SAMPLINGRATE_16 / 1000;
static const int TOTAL_BUFFERS = 2;
static const unsigned int BUFFER_SIZE = SAMPLE_RATE * CHANNELS * 20 / 1000;

std::vector<std::vector<short> > record_buffers;
unsigned int next_record_buffer_index = 0;

void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
std::vector<short>& buffer(record_buffers[next_record_buffer_index %
TOTAL_BUFFERS]);
++next_record_buffer_index;


SLresult result;
result = (*bq)->Enqueue(bq, &buffer[0], buffer.size()*sizeof(short));
if (SL_RESULT_SUCCESS != result)
{
std::cerr << "Failed to enqueue record buffer: " <<
next_record_buffer_index << std::endl;
}
}

static const char* SLESErrorToString(SLresult result)
{
switch (result)
{
case SL_RESULT_SUCCESS: return "SUCCESS";
case SL_RESULT_PRECONDITIONS_VIOLATED: return
"SL_RESULT_PRECONDITIONS_VIOLATED";
case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";
case SL_RESULT_BUFFER_INSUFFICIENT: return "SL_RESULT_BUFFER_INSUFFICIENT";
case SL_RESULT_CONTENT_CORRUPTED: return "SL_RESULT_CONTENT_CORRUPTED";
case SL_RESULT_CONTENT_UNSUPPORTED: return "SL_RESULT_CONTENT_UNSUPPORTED";
case SL_RESULT_CONTENT_NOT_FOUND: return "SL_RESULT_CONTENT_NOT_FOUND";
case SL_RESULT_PERMISSION_DENIED: return "SL_RESULT_PERMISSION_DENIED";
case SL_RESULT_FEATURE_UNSUPPORTED: return
"SL_RESULT_FEATURE_UNSUPPORTED";
case SL_RESULT_INTERNAL_ERROR: return "SL_RESULT_INTERNAL_ERROR";
case SL_RESULT_UNKNOWN_ERROR: return "SL_RESULT_UNKNOWN_ERROR";
case SL_RESULT_OPERATION_ABORTED: return "SL_RESULT_OPERATION_ABORTED";
case SL_RESULT_CONTROL_LOST: return "SL_RESULT_CONTROL_LOST";
default: return "Unknown";
}
}

int main()
{
record_buffers.resize(TOTAL_BUFFERS);
for (size_t i = 0; i < record_buffers.size(); ++i)
{
record_buffers[i].resize(BUFFER_SIZE, 0);
}
next_record_buffer_index = 0;


SLresult result;

const SLInterfaceID engine_ids[] = {SL_IID_AUDIOIODEVICECAPABILITIES};
const SLboolean engine_req[] = {SLbool_false};
// create engine
SLObjectItf engineObject = nullptr;
result = slCreateEngine(&engineObject, 0, nullptr, 1, engine_ids,
engine_req);
assert(SL_RESULT_SUCCESS == result);

// realize the engine
result = (*engineObject)->Realize(engineObject, SLbool_false);
assert(SL_RESULT_SUCCESS == result);

// get the engine interface, which is needed in order to create other
objects
SLEngineItf engineEngine;
result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
&engineEngine);
assert(SL_RESULT_SUCCESS == result);

// recorder interfaces
SLObjectItf recorderObject = nullptr;
SLRecordItf recorderRecord;
SLAndroidSimpleBufferQueueItf recorderBufferQueue;
SLDataFormat_PCM rec_format_pcm =
{
SL_DATAFORMAT_PCM // Format type
, CHANNELS // Channels
, SL_SAMPLINGRATE_16 // (in millihertz apparently)
, SL_PCMSAMPLEFORMAT_FIXED_16 // bits per sample
, SL_PCMSAMPLEFORMAT_FIXED_16 // container size
, SL_SPEAKER_FRONT_CENTER // channel mask
, SL_BYTEORDER_LITTLEENDIAN // endianess
};

// configure audio source
SLDataLocator_IODevice rec_loc_dev = {SL_DATALOCATOR_IODEVICE,
SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr};
SLDataSource rec_audioSrc = {&rec_loc_dev, nullptr};

// configure audio sink
SLDataLocator_AndroidSimpleBufferQueue rec_loc_bq =
{SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, TOTAL_BUFFERS};
SLDataSink rec_audioSnk = {&rec_loc_bq, &rec_format_pcm};

// create audio recorder
// (requires the RECORD_AUDIO permission)
const SLInterfaceID rec_id[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
const SLboolean rec_req[] = {SLbool_true};
result = (*engineEngine)->CreateAudioRecorder(engineEngine,
&recorderObject, &rec_audioSrc, &rec_audioSnk, 1, rec_id, rec_req);
assert(SL_RESULT_SUCCESS == result);

// realize the audio recorder
result = (*recorderObject)->Realize(recorderObject, SLbool_false);
std::cerr << "result: " << result << " : " << SLESErrorToString(result) <<
std::endl;
assert(SL_RESULT_SUCCESS == result);

// get the record interface
result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD,
&recorderRecord);
assert(SL_RESULT_SUCCESS == result);

// get the buffer queue interface
result = (*recorderObject)->GetInterface(recorderObject,
SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);

// register callback on the buffer queue
result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,
bqRecorderCallback, nullptr);
assert(SL_RESULT_SUCCESS == result);
// in case already recording, stop recording and clear buffer queue
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_STOPPED);
assert(SL_RESULT_SUCCESS == result);
result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
assert(SL_RESULT_SUCCESS == result);


for (size_t i = 0; i < TOTAL_BUFFERS; ++i)
{
// @todo bcost : Enqueue should have size * sizeof(short) BUT DOES NOT
//result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
recorderBuffer, RECORDER_FRAMES * sizeof(short));
result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue,
&record_buffers[i][0], record_buffers[i].size()*sizeof(short));
assert(SL_RESULT_SUCCESS == result);
}

// start recording
result = (*recorderRecord)->SetRecordState(recorderRecord,
SL_RECORDSTATE_RECORDING);
assert(SL_RESULT_SUCCESS == result);
// Start main loop
std::cerr << "Starting main loop" << std::endl;
while (true)
{
usleep(100000);
}
// @todo : Test app need to add cleanup and proper exit
return 0;
}

 */
