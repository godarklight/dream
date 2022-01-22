#ifndef __FAAC_DLL_H
# define __FAAC_DLL_H

#ifdef _WIN32
# include <windows.h>
#else
# include <dlfcn.h>
#endif
#if defined(_WIN32) && !defined(__MINGW32__)
# ifndef FAACAPI
#  define FAACAPI __stdcall
# endif
#else
# ifndef FAACAPI
#  define FAACAPI
# endif
#endif
#pragma pack(push, 1)
typedef struct {
  void *ptr;
  char *name;
}
psymodellist_t;

typedef struct faacEncConfiguration
{
    /* config version */
    int version;

    /* library version */
    char *name;

    /* copyright std::string */
    char *copyright;

    /* MPEG version, 2 or 4 */
    unsigned int mpegVersion;

    /* AAC object type */
    unsigned int aacObjectType;

    /* Allow mid/side coding */
    unsigned int allowMidside;

    /* Use one of the channels as LFE channel */
    unsigned int useLfe;

    /* Use Temporal Noise Shaping */
    unsigned int useTns;

    /* bitrate / channel of AAC file */
    unsigned long bitRate;

    /* AAC file frequency bandwidth */
    unsigned int bandWidth;

    /* Quantizer quality */
    unsigned long quantqual;

    /* Bitstream output format (0 = Raw; 1 = ADTS) */
    unsigned int outputFormat;

    /* psychoacoustic model list */
    psymodellist_t *psymodellist;

    /* selected index in psymodellist */
    unsigned int psymodelidx;

    /*
		PCM Sample Input Format
		0	FAAC_INPUT_NULL			invalid, signifies a misconfigured config
		1	FAAC_INPUT_16BIT		native endian 16bit
		2	FAAC_INPUT_24BIT		native endian 24bit in 24 bits		(not implemented)
		3	FAAC_INPUT_32BIT		native endian 24bit in 32 bits		(DEFAULT)
		4	FAAC_INPUT_FLOAT		32bit floating point
    */
    unsigned int inputFormat;

    /* block type enforcing (SHORTCTL_NORMAL/SHORTCTL_NOSHORT/SHORTCTL_NOLONG) */
    int shortctl;

	/*
		Channel Remapping

		Default			0, 1, 2, 3 ... 63  (64 is MAX_CHANNELS in coder.h)

		WAVE 4.0		2, 0, 1, 3
		WAVE 5.0		2, 0, 1, 3, 4
		WAVE 5.1		2, 0, 1, 4, 5, 3
		AIFF 5.1		2, 0, 3, 1, 4, 5
	*/
	int channel_map[64];

} faacEncConfiguration, *faacEncConfigurationPtr;
#  pragma pack(pop)

/* MPEG ID's */
#define MPEG2 1
#define MPEG4 0

/* AAC object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4

/* Input Formats */
#define FAAC_INPUT_NULL    0
#define FAAC_INPUT_16BIT   1
#define FAAC_INPUT_24BIT   2
#define FAAC_INPUT_32BIT   3
#define FAAC_INPUT_FLOAT   4

typedef void *faacEncHandle;
typedef int (FAACAPI faacEncGetVersion_t)(char **, char **);


typedef faacEncConfigurationPtr
  (FAACAPI faacEncGetCurrentConfiguration_t)(faacEncHandle);


typedef int (FAACAPI faacEncSetConfiguration_t)(faacEncHandle,
				    faacEncConfigurationPtr);


typedef faacEncHandle (FAACAPI faacEncOpen_t)(unsigned long,
				  unsigned int,
				  unsigned long *,
				  unsigned long *);


typedef int (FAACAPI faacEncGetDecoderSpecificInfo_t)(faacEncHandle, unsigned char **,
					  unsigned long *);


typedef int (FAACAPI faacEncEncode_t)(faacEncHandle, int32_t *, unsigned int,
			 unsigned char *,
			 unsigned int);


typedef int (FAACAPI faacEncClose_t)(faacEncHandle);

#endif
