/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2004
 *
 * Author(s):
 *	Volker Fischer
 *
 * Description:
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

#include "Hamlib.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#if defined(_WIN32)
# ifdef HAVE_SETUPAPI
#  ifndef INITGUID
#   define INITGUID 1
#  endif
#  include <windows.h>
#  include <setupapi.h>
# endif
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#endif
using namespace std;

/*
	This code is based on patches and example code from Tomi Manninen and
	Stephane Fillod (developer of hamlib)
*/
CHamlib::CHamlib():SpecDRMRigs(), CapsHamlibModels(),
pRig(nullptr), bSMeterIsSupported(false),
bModRigSettings(false), iHamlibModelID(0),
strHamlibConf(""), strSettings(""), iFreqOffset(0),
modes(), levels(), functions(), parameters(), config()
{
#ifdef RIG_MODEL_DWT
	/* Digital World Traveller */
	RigSpecialParameters(RIG_MODEL_DWT, "", 0, "");
#endif

#ifdef RIG_MODEL_G303
	/* Winradio G303 */
	RigSpecialParameters(RIG_MODEL_G303, "l_ATT=0,l_AGC=3", 0,
						 "l_ATT=0,l_AGC=3");
#endif

#ifdef RIG_MODEL_G313
	/* Winradio G313 */
	RigSpecialParameters(RIG_MODEL_G313, "l_ATT=0,l_AGC=3", 0,
						 "l_ATT=0,l_AGC=3");
#endif

#ifdef RIG_MODEL_AR7030
	/* AOR 7030 */
//  vecSpecDRMRigs.Add(CSpecDRMRig(RIG_MODEL_AR7030,
//      "m_CW=9500,l_IF=-4200,l_AGC=3", 5 /* kHz frequency offset */,
//      "l_AGC=3"));
	RigSpecialParameters(RIG_MODEL_AR7030, "m_AM=3,l_AGC=5",
						 0 /* kHz frequency offset */ ,
						 "m_AM=6,l_AGC=5");
#endif

#ifdef RIG_MODEL_ELEKTOR304
	/* Elektor 3/04 */
	RigSpecialParameters(RIG_MODEL_ELEKTOR304, "", 0, "");
#endif

#ifdef RIG_MODEL_ELEKTOR507
    /* Elektor 5/07 */
    RigSpecialParameters(RIG_MODEL_ELEKTOR507, "",
						-12 /* kHz frequency offset */ ,
						"");
#endif

#ifdef RIG_MODEL_NRD535
	/* JRC NRD 535 */
	RigSpecialParameters(RIG_MODEL_NRD535,
						 "l_CWPITCH=-5000,m_CW=12000,l_IF=-2000,l_AGC=3"
						 /* AGC=slow */ ,
						 3 /* kHz frequency offset */ ,
						 "l_AGC=3");
#endif

#ifdef RIG_MODEL_RX320
	/* TenTec RX320D */
	RigSpecialParameters(RIG_MODEL_RX320, "l_AF=1,l_AGC=3,m_AM=6000", 0,
						 "l_AGC=3");
#endif

#ifdef RIG_MODEL_RX340
	/* TenTec RX340D */
	RigSpecialParameters(RIG_MODEL_RX340,
						 "l_AF=1,m_USB=16000,l_AGC=3,l_IF=2000",
						 -12 /* kHz frequency offset */ ,
						 "l_AGC=3");
#endif

	/* Load all available front-end remotes in hamlib library */
	rig_load_all_backends();

	/* Get all models which are available.
	 * A call-back function is called to return the different rigs */
	rig_list_foreach(PrintHamlibModelList, this);
}

CHamlib::~CHamlib()
{
	if (pRig != nullptr)
	{
		/* close everything */
		rig_close(pRig);
		rig_cleanup(pRig);
	}
}

void
CHamlib::RigSpecialParameters(rig_model_t id, const string & sSet, int iFrOff,
							  const string & sModSet)
{
	CapsHamlibModels[id].bIsSpecRig = true;
	SpecDRMRigs[id] = CSpecDRMRig(sSet, iFrOff, sModSet);
}

void
CHamlib::GetRigList(map < rig_model_t, SDrRigCaps > &rigs)
{
	rigs = CapsHamlibModels;
}

void
CHamlib::GetPortList(map < string, string > &ports)
{
	ports.clear();
/* Port for use with 'NET rigctl' */
	ports["localhost:4532"] = "localhost:4532";
/* Config string for com-port selection is different in Windows and Linux */
#ifdef _WIN32
# ifdef HAVE_SETUPAPI
	GUID guid = GUID_DEVINTERFACE_COMPORT;
	HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, nullptr, nullptr,
											   DIGCF_PRESENT |
											   DIGCF_DEVICEINTERFACE);
	if (hDevInfoSet != INVALID_HANDLE_VALUE)
	{
		SP_DEVINFO_DATA devInfo;
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		for (int i = 0; SetupDiEnumDeviceInfo(hDevInfoSet, i, &devInfo); i++)
		{
			HKEY hDeviceKey =
				SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL,
									 0, DIREG_DEV, KEY_QUERY_VALUE);
			if (hDeviceKey)
			{
				char szPortName[256];
				DWORD dwSize = sizeof(szPortName);
				DWORD dwType = 0;
				if ((RegQueryValueExA
					 (hDeviceKey, "PortName", nullptr, &dwType,
					  reinterpret_cast < LPBYTE > (szPortName),
					  &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
				{
					char szFriendlyName[256];
					DWORD dwSize = sizeof(szFriendlyName);
					DWORD dwType = 0;
					if (SetupDiGetDeviceRegistryPropertyA
						(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType,
						 reinterpret_cast < PBYTE > (szFriendlyName), dwSize,
						 &dwSize) && (dwType == REG_SZ))
						ports[string(szFriendlyName) + " " + szPortName] = szPortName;
					else
						ports[szPortName] = szPortName;
				}

				RegCloseKey(hDeviceKey);
			}
		}

		SetupDiDestroyDeviceInfoList(hDevInfoSet);
	}
# endif
	if (ports.empty())
	{
		ports["COM1"] = "COM1";
		ports["COM2"] = "COM2";
		ports["COM3"] = "COM3 ";
		ports["COM4"] = "COM4 ";
		ports["COM5"] = "COM5 ";
	}
#elif defined(__APPLE__)
    io_iterator_t serialPortIterator;
    kern_return_t			kernResult;
    CFMutableDictionaryRef	classesToMatch;

    // Serial devices are instances of class IOSerialBSDClient
    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    if (classesToMatch == nullptr)
    {
        fprintf(stderr, "IOServiceMatching returned a nullptr dictionary.\n");
    }
    else
	{
        CFDictionarySetValue(classesToMatch,
                             CFSTR(kIOSerialBSDTypeKey),
                             CFSTR(kIOSerialBSDRS232Type));

	}
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, &serialPortIterator);
    if (KERN_SUCCESS != kernResult)
    {
        fprintf(stderr, "IOServiceGetMatchingServices returned %d\n", kernResult);
    }

    io_object_t		modemService;

    // Iterate across all modems found. In this example, we bail after finding the first modem.

    while ((modemService = IOIteratorNext(serialPortIterator)))
    {
        CFStringRef	bsdPathAsCFString;

		// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
		// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
		// incoming calls, e.g. a fax listener.

		bsdPathAsCFString = CFStringRef(IORegistryEntryCreateCFProperty(modemService,
                                                            CFSTR(kIOCalloutDeviceKey),
                                                            kCFAllocatorDefault,
                                                            0));
        if (bsdPathAsCFString)
        {
            Boolean result;
			char bsdPath[256];

            // Convert the path from a CFString to a C (NUL-terminated) string for use
			// with the POSIX open() call.

			result = CFStringGetCString(bsdPathAsCFString,
                                        bsdPath,
                                        sizeof(bsdPath),
                                        kCFStringEncodingUTF8);
            CFRelease(bsdPathAsCFString);

            if (result)
			{
				// make the name a bit more friendly for the menu
				string s,t=bsdPath;
				size_t p = t.find('.');
				if(p<string::npos)
					s = t.substr(p+1);
				else
					s = t;
				ports[s] = bsdPath;
            }
        }

        // Release the io_service_t now that we are done with it.

		(void) IOObjectRelease(modemService);
    }
#elif defined(__unix__)
	bool bOK = false;
	FILE *p = popen("hal-find-by-capability --capability serial", "r");
	if (p != nullptr)
	{
		while (!feof(p))
		{
			char buf[1024];
			buf[0] = 0;
			char* r = fgets(buf, sizeof(buf), p);
			if (strlen(buf) > 0)
			{
				string s =
					string("hal-get-property --key serial.device --udi ") +
					buf;
				FILE *p2 = popen(s.c_str(), "r");
				if (p2 != nullptr)
				{
					buf[0] = 0;
					r = fgets(buf, sizeof(buf), p2);
					size_t n = strlen(buf);
					if (n > 0)
					{
						if (buf[n - 1] == '\n')
							buf[n - 1] = 0;
						ports[buf] = buf;
						bOK = true;
					}
					pclose(p2);
				}
			}
			(void)r;
		}
		pclose(p);
	}
	if (!bOK)
	{
# ifdef __linux__
		ports["ttyS0"] = "/dev/ttyS0";
		ports["ttyS1"] = "/dev/ttyS1";
		ports["ttyUSB0"] = "/dev/ttyUSB0";
# endif
	}
#endif
}

void
CHamlib::SetComPort(const string & port)
{
	config["rig_pathname"] = port;
	SetHamlibModelID(iHamlibModelID);
}

string CHamlib::GetComPort() const
{
	map < string, string >::const_iterator m = config.find("rig_pathname");
	if (m == config.end())
		return "";
	return m->second;
}

int
CHamlib::PrintHamlibModelList(const struct rig_caps *caps, void *data)
{
	/* Access data members of class through pointer ((CHamlib*) data) */
	CHamlib & Hamlib = *((CHamlib *) data);

	/* Store new model in class. Use only relevant information */
	bool bIsSpec =
		Hamlib.SpecDRMRigs.find(caps->rig_model) != Hamlib.SpecDRMRigs.end();

	Hamlib.CapsHamlibModels[caps->rig_model] =
		SDrRigCaps(caps->mfg_name, caps->model_name, caps->status, bIsSpec);

	return 1;					/* !=0, we want them all! */
}

void
CHamlib::LoadSettings(CSettings & s)
{
	rig_model_t model = s.Get("Hamlib", "hamlib-model", 0);

	if (model != 0)
	{
		/* Hamlib configuration string */
		string strHamlibConf = s.Get("Hamlib", "hamlib-config");

		if (model == RIG_MODEL_G313)
		{
			string kwd, val;
#ifdef __linux__
			kwd = "if_path";
			val = "/tmp/g313";
			s.Put("Hamlib", kwd, val);
#endif
#ifdef _WIN32
			kwd = "wodeviceid";
			val = "-2";
#endif
			if(kwd!="")
			{
				if (strHamlibConf=="")
				{
					strHamlibConf = kwd + "=" + val;
				}
				else
				{
					// don't overwrite a saved value
					if(strHamlibConf.find_first_of(kwd)==string::npos)
					{
						strHamlibConf += "," + kwd + "=" + val;
					}
				}
			}
		}
		if (strHamlibConf != "")
		{
			istringstream params(strHamlibConf);
			while (!params.eof())
			{
				string name, value;
				getline(params, name, '=');
				getline(params, value, ',');
				config[name] = value;
			}
		}

		/* Enable DRM modified receiver flag */
		bModRigSettings = s.Get("Hamlib", "enmodrig", false);

		strSettings = s.Get("Hamlib", "settings");
		iFreqOffset = s.Get("Hamlib", "freqoffset", 0);

		if (strSettings != "" || iFreqOffset != 0)
		{
			if (bModRigSettings)
				RigSpecialParameters(model, "", iFreqOffset, strSettings);
			else
				RigSpecialParameters(model, strSettings, iFreqOffset, "");
		}

		/* Hamlib Model ID */
		SetHamlibModelID(model);
	}

    s.Put("Hamlib", "hamlib-model", (int)model);
	s.Put("Hamlib", "hamlib-config", strHamlibConf);
	s.Put("Hamlib", "settings", strSettings);
	s.Put("Hamlib", "freqoffset", iFreqOffset);
}

void
CHamlib::SaveSettings(CSettings & s)
{
	/* Hamlib Model ID */
    s.Put("Hamlib", "hamlib-model", (int)iHamlibModelID);

	/* Hamlib configuration string */
	stringstream ss;
	string sep = "";
	for (map < string, string >::iterator i = config.begin();
		 i != config.end(); i++)
	{
		ss << sep << i->first << "=" << i->second;
		sep = ",";
	}

	s.Put("Hamlib", "hamlib-config", ss.str());

	/* Enable DRM modified receiver flag */
	s.Put("Hamlib", "enmodrig", bModRigSettings);

	s.Put("Hamlib", "settings", strSettings);

	s.Put("Hamlib", "freqoffset", iFreqOffset);
}

bool
CHamlib::SetFrequency(const int iFreqkHz)
{
	bool bSucceeded = false;

	/* Check if rig was opend properly */
	if (pRig != nullptr)
	{
		/* Set frequency (consider frequency offset and conversion
		   from kHz to Hz by " * 1000 ") */
		if (rig_set_freq(pRig, RIG_VFO_CURR, (iFreqkHz + iFreqOffset) * 1000)
			== RIG_OK)
		{
			bSucceeded = true;
		}
	}

	return bSucceeded;
}

CHamlib::ESMeterState CHamlib::GetSMeter(_REAL & rCurSigStr)
{
	ESMeterState
		eRetVal = SS_NOTVALID;
	rCurSigStr = (_REAL) 0.0;

	if ((pRig != nullptr) && (bSMeterIsSupported))
	{
		value_t
			tVal;
		const int
			iHamlibRetVal =
			rig_get_level(pRig, RIG_VFO_CURR, RIG_LEVEL_STRENGTH, &tVal);

		if (!iHamlibRetVal)
		{
			rCurSigStr = (_REAL) tVal.i;
			eRetVal = SS_VALID;
		}

		/* If a time-out happened, do not update s-meter anymore (disable it) */
		if (iHamlibRetVal == -RIG_ETIMEOUT)
		{
			bSMeterIsSupported = false;
			eRetVal = SS_TIMEOUT;
		}
	}

	return eRetVal;
}

void
CHamlib::ConfigureRig(const string & strSet)
{
	/* Parse special settings */
	istringstream params(strSet);
	while (!params.eof())
	{
		string p, name, value;

		getline(params, p, '_');
		getline(params, name, '=');
		getline(params, value, ',');
		if (p == "" || p.length() != 1 || name == "" || value == "")
		{
			/* Malformatted config string */
			rig_cleanup(pRig);
			pRig = nullptr;

			throw CGenErr(string("Malformatted config string: ") + strSet);
		}
		switch (p[0])
		{
		case 'm':
			modes[name] = value;
			break;
		case 'l':
			levels[name] = value;
			break;
		case 'f':
			functions[name] = value;
			break;
		case 'p':
			parameters[name] = value;
			break;
		default:
			cerr << "Rig unknown setting: " << p << "_" << name <<
				"=" << value << endl;
		}
	}
}

void
CHamlib::SetRigModes()
{
	for (map < string, string >::const_iterator i = modes.begin();
		 i != modes.end(); i++)
	{
		rmode_t mode = rig_parse_mode(i->first.c_str());
		if (mode != RIG_MODE_NONE)
		{
			int ret =
				rig_set_mode(pRig, RIG_VFO_CURR, mode,
							 atoi(i->second.c_str()));
			if (ret != RIG_OK)
				cerr << "Rig set mode failed: " << rigerror(ret) << endl;
		}
	}
}

void
CHamlib::SetRigLevels()
{
	for (map < string, string >::const_iterator i = levels.begin();
		 i != levels.end(); i++)
	{
		setting_t setting = rig_parse_level(i->first.c_str());
		if (setting != RIG_LEVEL_NONE)
		{
			value_t val;
			if (RIG_LEVEL_IS_FLOAT(setting))
				val.f = atof(i->second.c_str());
			else
				val.i = atoi(i->second.c_str());

			int ret = rig_set_level(pRig, RIG_VFO_CURR, setting, val);
			if (ret != RIG_OK)
				cerr << "Rig set level failed: " << rigerror(ret) << endl;
		}
	}
}

void
CHamlib::SetRigFuncs()
{
	for (map < string, string >::const_iterator i = functions.begin();
		 i != functions.end(); i++)
	{
		setting_t setting = rig_parse_func(i->first.c_str());
		if (setting != RIG_FUNC_NONE)
		{
			int ret =
				rig_set_func(pRig, RIG_VFO_CURR, setting,
							 atoi(i->second.c_str()));
			if (ret != RIG_OK)
				cerr << "Rig set func failed: " << rigerror(ret) << endl;
		}
	}
}

void
CHamlib::SetRigParams()
{
	for (map < string, string >::const_iterator i = parameters.begin();
		 i != parameters.end(); i++)
	{
		setting_t setting = rig_parse_parm(i->first.c_str());
		if (setting != RIG_PARM_NONE)
		{
			value_t val;
			if (RIG_PARM_IS_FLOAT(setting))
				val.f = atof(i->second.c_str());
			else
				val.i = atoi(i->second.c_str());
			int ret = rig_set_parm(pRig, setting, val);
			if (ret != RIG_OK)
				cerr << "Rig set parm failed: " << rigerror(ret) << endl;
		}
	}
}

void
CHamlib::SetRigConfig()
{
	for (map < string, string >::const_iterator i = config.begin();
		 i != config.end(); i++)
	{
	    cerr << i->first << ":" << i->second << endl;
		int ret =
			rig_set_conf(pRig, rig_token_lookup(pRig, i->first.c_str()),
						 i->second.c_str());
		if (ret != RIG_OK)
		{
			rig_cleanup(pRig);
			pRig = nullptr;
			throw CGenErr("Rig set conf failed.");
		}
	}
}

void
CHamlib::SetEnableModRigSettings(const bool bNSM)
{
	if (bModRigSettings != bNSM)
	{
		/* Set internal parameter */
		bModRigSettings = bNSM;

		/* Hamlib must be re-initialized with new parameter */
		SetHamlibModelID(iHamlibModelID);
	}
}

void
CHamlib::SetHamlibModelID(const rig_model_t model)
{
	int ret;

	/* Set value for current selected model ID */
	iHamlibModelID = model;

	/* Init frequency offset */
	iFreqOffset = 0;

	try
	{
		/* If rig was already open, close it first */
		if (pRig != nullptr)
		{
			/* Close everything */
			rig_close(pRig);
			rig_cleanup(pRig);
			pRig = nullptr;
		}

		if (iHamlibModelID == 0)
			throw CGenErr("No rig model ID selected.");

		/* Check for special DRM front-end selection */
		map < rig_model_t, CSpecDRMRig >::const_iterator s =
			SpecDRMRigs.find(iHamlibModelID);
		if (s != SpecDRMRigs.end())
		{
			/* Get correct parameter string */
			if (bModRigSettings)
				strSettings = s->second.strDRMSetMod;
			else
			{
				strSettings = s->second.strDRMSetNoMod;

				/* Additionally, set frequency offset for this special rig */
				iFreqOffset = s->second.iFreqOffs;
			}
			if (strSettings != "")
			{
				ConfigureRig(strSettings);
			}
		}

		/* Init rig */
		pRig = rig_init(iHamlibModelID);
		if (pRig == nullptr)
			throw CGenErr("Initialization of hamlib failed.");

		SetRigConfig();

		/* Open rig */
		ret = rig_open(pRig);
		if (ret != RIG_OK)
		{
			/* Fail! */
			rig_cleanup(pRig);
			pRig = nullptr;

			throw CGenErr("Rig open failed.");
		}

		/* Ignore result, some rigs don't have support for this */
		rig_set_powerstat(pRig, RIG_POWER_ON);

		SetRigModes();
		SetRigLevels();
		SetRigFuncs();
		SetRigParams();

		/* Check if s-meter capabilities are available */
		if (pRig != nullptr)
		{
			/* Check if s-meter can be used. Disable GUI control if not */
			if (rig_has_get_level(pRig, RIG_LEVEL_STRENGTH))
				bSMeterIsSupported = true;
			else
				bSMeterIsSupported = false;
		}
	}

	catch(CGenErr GenErr)
	{
		/* Print error message */
		cerr << GenErr.strError << endl;

		/* Disable s-meter */
		bSMeterIsSupported = false;
	}
}
