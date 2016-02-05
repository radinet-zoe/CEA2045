//*********************************************************************************************************************
// ReportWrapper.h
//
// Copyright (c) 2013 ELECTRIC POWER RESEARCH INSTITUTE, INC. All rights reserved.
//
// OpenADR ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
// following conditions are met:
//
// *    Redistributions of source code must retain the above copyright  notice, this list of conditions and
//      the following disclaimer.
//
// *    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
//      the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// *    Neither the name of the Electric Power Research Institute, Inc. ("EPRI") nor the names of its contributors
//      may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL EPRI BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
#ifndef REPORTWRAPPER_H_
#define REPORTWRAPPER_H_

#include <oadrsd/2b/oadrReportDescriptionType.hxx>
#include <oadrsd/2b/MarketContextType.hxx>
#include <oadrsd/2b/EiTargetType.hxx>
#include <oadrsd/2b/oadrSamplingRateType.hxx>
#include <oadrsd/2b/DurationValueType.hxx>
#include <oadrsd/2b/ItemBaseType.hxx>
#include <oadrsd/2b/StreamBaseType.hxx>

#include <oadrsd/2b/SiScaleCodeType.hxx>
#include <oadrsd/2b/EnergyReactiveType.hxx>
#include <oadrsd/2b/EnergyApparentType.hxx>
#include <oadrsd/2b/EnergyRealType.hxx>

#include <oadrsd/2b/PowerReactiveType.hxx>
#include <oadrsd/2b/PowerApparentType.hxx>
#include <oadrsd/2b/PowerRealType.hxx>
#include <oadrsd/2b/PowerAttributesType.hxx>

#include <oadrsd/2b/oadrReportType.hxx>

#include <oadrsd/2b/DurationPropType.hxx>

#include <oadrsd/2b/reportNameType.hxx>

#include <oadrsd/2b/oadrReportDescriptionType.hxx>
#include <oadrsd/2b/IntervalType.hxx>
#include <oadrsd/2b/intervals.hxx>
#include <oadrsd/2b/oadrDataQualityType.hxx>
#include <oadrsd/2b/oadrDataQualityTypeType.hxx>
#include <oadrsd/2b/ReportPayloadType.hxx>
#include <oadrsd/2b/oadrReportPayloadType.hxx>
#include <oadrsd/2b/ConfidenceType.hxx>
#include <oadrsd/2b/AccuracyType.hxx>
#include <oadrsd/2b/oadrPayloadResourceStatusType.hxx>
#include <oadrsd/2b/PayloadFloatType.hxx>

#include <oadrsd/2b/StreamPayloadBaseType.hxx>

#include <oadrsd/2b/oadrLoadControlStateType.hxx>

#include <oadrsd/2b/uid.hxx>

#include "ReportEnumeratedTypeValue.h"
#include "ReadingTypeEnumeratedTypeValue.h"

#include <string>
#include <map>
#include <vector>

#include "../Oadr2bHelper.h"

#include "ReportName.h"
#include "../DurationModifier.h"

#include "../../helper/RandomHex.h"

#include "../OadrException.h"

using namespace std;
using namespace oadr2b::oadr;
using namespace oadr2b::ei;
using namespace power;
using namespace siscale;
using namespace stream;

class ReportWrapper
{
private:
	ReportName *m_reportName;
	int m_duration;
	DurationModifier *m_durationModifier;

	string m_reportSpecifierID;

	map<string, oadrReportDescriptionType*> m_reportDescriptions;

	map<string, int> m_rIDtoUID;

	map<int, vector<oadrReportPayloadType> > m_uIDtoReportPayload;

	intervals::interval_sequence m_intervals;

	time_t m_createDateTime;

	void addReportPayload(string rID, oadrReportPayloadType &reportPayload);

public:
	enum eEnergyItemType
	{
		eEnergyApparent,
		eEnergyReactive,
		eEnergyReal
	};

	enum ePowerItemType
	{
		ePowerApparent,
		ePowerReactive,
		ePowerReal
	};

	ReportWrapper(ReportName *reportName, int duration, DurationModifier *durationModifier, string reportSpecifierID = "", time_t createDateTime = 0);
	virtual ~ReportWrapper();

	string reportSpecifierID();

	void addDescription(string rID, string resourceID, ReportEnumeratedType::value reportType,
			ReadingTypeEnumeratedType::value readingType, string marketContext,
			int minSamplingPeriod, int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier,
			auto_ptr<oadrReportDescriptionType::itemBase_type> measuredValue);

	void addDescriptionResourceStatus(string rID, string resourceID, string marketContext,
			int minSamplingPeriod, int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier);


	void addDescriptionEnergyItem(string rID, string resourceID, ReportEnumeratedType::value reportType,
			ReadingTypeEnumeratedType::value readingType, string marketContext, int minSamplingPeriod,
			int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier, eEnergyItemType energyItemType,
			string units, SiScaleCodeType::value siScaleCodeType, string description = "");

	void addDescriptionPowerItem(string rID, string resourceID, ReportEnumeratedType::value reportType,
			ReadingTypeEnumeratedType::value readingType, string marketContext,
			int minSamplingPeriod, int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier,
			ePowerItemType powerItemType, string units, SiScaleCodeType::value siScaleCodeType, double hertz,
			double voltage, bool ac, string description = "");

	auto_ptr<oadrReportType> generateReportDescription();

	void addInterval(string rID, time_t dtstart, int duration, DurationModifier *durationModifier);

	void addOadrReportPayloadFloat(string rID, float value, oadrDataQualityType::value dataQuality = oadrDataQualityType::Quality_Good___Non_Specific, unsigned int confidence = 100, unsigned int accuracy = 100);

	void addOadrReportPayloadResourceStatus(string rID, bool online, bool manualOverride, oadr2b::oadr::oadrLoadControlStateType loadControlState, oadrDataQualityType::value dataQuality = oadrDataQualityType::Quality_Good___Non_Specific, unsigned int confidence = 100, unsigned int accuracy = 100);

	auto_ptr<oadrReportType> generateReport(time_t dtstart, string reportRequestID, time_t createdDateTime = 0);
};

#endif /* REPORTWRAPPER_H_ */
