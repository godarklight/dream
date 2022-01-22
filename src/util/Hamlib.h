/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2004
 *
 * Author(s):
 *	Volker Fischer
 *
 * Description:
 *	Implements:
 *	- Hamlib interface
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

#ifndef __HAMLIB_H
#define __HAMLIB_H

#include "../GlobalDefinitions.h"
#include "Settings.h"
#include <hamlib/rig.h>

class CHamlib
{
public:
	enum ESMeterState {SS_VALID, SS_NOTVALID, SS_TIMEOUT};

	CHamlib();
	virtual ~CHamlib();

	struct SDrRigCaps
	{
		SDrRigCaps() : strManufacturer(""), strModelName(""),
			eRigStatus(RIG_STATUS_ALPHA),bIsSpecRig(false) {}
		SDrRigCaps(const std::string& strNMan, const std::string& strNModN, rig_status_e eNSt, bool bNsp) :
			strManufacturer(strNMan), strModelName(strNModN), eRigStatus(eNSt),
			bIsSpecRig(bNsp)
			{}
		SDrRigCaps(const SDrRigCaps& nSDRC) :
			strManufacturer(nSDRC.strManufacturer),
			strModelName(nSDRC.strModelName),
			eRigStatus(nSDRC.eRigStatus), bIsSpecRig(nSDRC.bIsSpecRig) {}

		inline SDrRigCaps& operator=(const SDrRigCaps& cNew)
		{
			strManufacturer = cNew.strManufacturer;
			strModelName = cNew.strModelName;
			eRigStatus = cNew.eRigStatus;
			bIsSpecRig = cNew.bIsSpecRig;
			return *this;
		}

		std::string			strManufacturer;
		std::string			strModelName;
		rig_status_e	eRigStatus;
		bool		bIsSpecRig;
	};

	bool		SetFrequency(const int iFreqkHz);
	ESMeterState	GetSMeter(_REAL& rCurSigStr);

	/* backend selection */
	void			GetRigList(std::map<rig_model_t,SDrRigCaps>&);
	void			SetHamlibModelID(const rig_model_t model);
	rig_model_t		GetHamlibModelID() const {return iHamlibModelID;}

	/* com port selection */
	void			GetPortList(std::map<std::string,std::string>&);
	void			SetComPort(const std::string&);
	std::string		GetComPort() const;

	void			SetEnableModRigSettings(const bool bNSM);
	bool			GetEnableModRigSettings() const {return bModRigSettings;}
	std::string		GetInfo() const;

	void			RigSpecialParameters(rig_model_t id, const std::string& sSet, int iFrOff, const std::string& sModSet);
	void			ConfigureRig(const std::string & strSet);
	void			LoadSettings(CSettings& s);
	void			SaveSettings(CSettings& s);

protected:
	class CSpecDRMRig
	{
	public:
		CSpecDRMRig() : strDRMSetMod(""), strDRMSetNoMod(""), iFreqOffs(0) {}
		CSpecDRMRig(const CSpecDRMRig& nSpec) :
			strDRMSetMod(nSpec.strDRMSetMod),
			strDRMSetNoMod(nSpec.strDRMSetNoMod), iFreqOffs(nSpec.iFreqOffs) {}
		CSpecDRMRig(std::string sSet, int iNFrOff, std::string sModSet) :
			strDRMSetMod(sModSet), strDRMSetNoMod(sSet), iFreqOffs(iNFrOff) {}

		std::string		strDRMSetMod; /* Special DRM settings (modified) */
		std::string		strDRMSetNoMod; /* Special DRM settings (not mod.) */
		int			iFreqOffs; /* Frequency offset */
	};

	std::map<rig_model_t,CSpecDRMRig>	SpecDRMRigs;
	std::map<rig_model_t,SDrRigCaps>		CapsHamlibModels;

	void EnableSMeter(const bool bStatus);

	static int			PrintHamlibModelList(const struct rig_caps* caps, void* data);
	void				SetRigModes();
	void				SetRigLevels();
	void				SetRigFuncs();
	void				SetRigParams();
	void				SetRigConfig();

	RIG*				pRig;
	bool				bSMeterIsSupported;
	bool				bModRigSettings;
	rig_model_t			iHamlibModelID;
	std::string			strHamlibConf;
	std::string			strSettings;
	int					iFreqOffset;
	std::map<std::string,std::string> modes;
	std::map<std::string,std::string> levels;
	std::map<std::string,std::string> functions;
	std::map<std::string,std::string> parameters;
	std::map<std::string,std::string> config;

};

#endif