//*********************************************************************************************************************
// ReportWrapper.cpp
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
#include "ReportWrapper.h"

/********************************************************************************/

ReportWrapper::ReportWrapper(ReportName *reportName, int duration, DurationModifier *durationModifier, string reportSpecifierID, time_t createDateTime) :
	m_reportName(reportName),
	m_duration(duration),
	m_durationModifier(durationModifier),
	m_createDateTime(createDateTime)
{

	m_reportSpecifierID = (reportSpecifierID != "" ? reportSpecifierID : RandomHex::instance()->generateRandomHex());
}

/********************************************************************************/

ReportWrapper::~ReportWrapper()
{
	while (m_reportDescriptions.size() > 0)
	{
		oadrReportDescriptionType *reportDescription = m_reportDescriptions.begin()->second;
		m_reportDescriptions.erase(reportDescription->rID());

		delete (reportDescription);
	}
}

/********************************************************************************/

string ReportWrapper::reportSpecifierID()
{
	return m_reportSpecifierID;
}

/********************************************************************************/

void ReportWrapper::addDescription(string rID, string resourceID, ReportEnumeratedType::value reportType,
		ReadingTypeEnumeratedType::value readingType, string marketContext,
		int minSamplingPeriod, int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier,
		auto_ptr<oadrReportDescriptionType::itemBase_type> measuredValue)
{
	string reportTypeString = ReportEnumeratedTypeValue(reportType).toString();
	string readingTypeString = ReadingTypeEnumeratedTypeValue(readingType).toString();

	oadrReportDescriptionType *reportDescription = new oadrReportDescriptionType(rID, reportTypeString, readingTypeString);

	oadrReportDescriptionType::marketContext_type mc(marketContext);
	reportDescription->marketContext(mc);

	if (resourceID != "")
	{
		EiTargetType eitt;

		EiTargetType::resourceID_sequence rid_seq;

		rid_seq.push_back(resourceID);

		eitt.resourceID(rid_seq);

		reportDescription->reportDataSource(eitt);
	}

	oadrSamplingRateType samplingRate(Oadr2bHelper::generateDurationValueType(minSamplingPeriod, durationModifier),
			Oadr2bHelper::generateDurationValueType(maxSamplingPeriod, durationModifier),
			onChange);

	reportDescription->oadrSamplingRate(samplingRate);

	reportDescription->itemBase(measuredValue);

	m_reportDescriptions[rID] = reportDescription;
}

/********************************************************************************/

void ReportWrapper::addDescriptionResourceStatus(string rID, string resourceID, string marketContext,
		int minSamplingPeriod, int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier)
{
	auto_ptr<oadrReportDescriptionType::itemBase_type> i;

	addDescription(rID, resourceID, ReportEnumeratedType::x_resourceStatus, ReadingTypeEnumeratedType::x_notApplicable,
			marketContext, minSamplingPeriod, maxSamplingPeriod, onChange, durationModifier, i);
}

/********************************************************************************/

void ReportWrapper::addDescriptionEnergyItem(string rID, string resourceID, ReportEnumeratedType::value reportType,
		ReadingTypeEnumeratedType::value readingType, string marketContext, int minSamplingPeriod,
		int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier, eEnergyItemType energyItemType,
		string units, SiScaleCodeType::value siScaleCodeType, string description)
{
	auto_ptr<oadrReportDescriptionType::itemBase_type> base;

	EnergyItemType *measuredValue;

	switch (energyItemType)
	{
	case eEnergyApparent:
		measuredValue = new EnergyApparentType(description, units, siScaleCodeType);
		break;

	case eEnergyReactive:
		measuredValue = new EnergyReactiveType(description, units, siScaleCodeType);
		break;

	case eEnergyReal:
		measuredValue = new EnergyRealType(description, units, siScaleCodeType);
		break;

	default:
		measuredValue = new EnergyApparentType(description, units, siScaleCodeType);
		break;
	}

	base = auto_ptr<oadrReportDescriptionType::itemBase_type>(measuredValue);

	addDescription(rID, resourceID, reportType, readingType, marketContext, minSamplingPeriod, maxSamplingPeriod, onChange,
			durationModifier, base);
}

/********************************************************************************/

void ReportWrapper::addDescriptionPowerItem(string rID, string resourceID, ReportEnumeratedType::value reportType,
		ReadingTypeEnumeratedType::value readingType, string marketContext,
		int minSamplingPeriod, int maxSamplingPeriod, bool onChange, DurationModifier *durationModifier,
		ePowerItemType powerItemType, string units, SiScaleCodeType::value siScaleCodeType, double hertz,
		double voltage, bool ac, string description)
{
	auto_ptr<oadrReportDescriptionType::itemBase_type> base;

	PowerItemType *measuredValue;

	PowerApparentType::powerAttributes_type attributes(hertz, voltage, ac);

	switch (powerItemType)
	{
	case ePowerApparent:
		measuredValue = new PowerApparentType(description, units, siScaleCodeType, attributes);
		break;

	case ePowerReactive:
		measuredValue = new PowerReactiveType(description, units, siScaleCodeType, attributes);
		break;

	case ePowerReal:
		measuredValue = new PowerRealType(description, units, siScaleCodeType, attributes);
		break;

	default:
		measuredValue = new PowerApparentType(description, units, siScaleCodeType, attributes);
		break;
	}

	base = auto_ptr<oadrReportDescriptionType::itemBase_type>(measuredValue);

	addDescription(rID, resourceID, reportType, readingType, marketContext, minSamplingPeriod, maxSamplingPeriod, onChange,
			durationModifier, base);
}

/********************************************************************************/

auto_ptr<oadrReportType> ReportWrapper::generateReportDescription()
{
	if (m_createDateTime == 0)
		m_createDateTime = time(NULL);

	auto_ptr<oadrReportType> reportType(new oadrReportType("0", m_reportSpecifierID, Oadr2bHelper::timetToiCalDateTime(m_createDateTime)));

	reportType->duration(Oadr2bHelper::generateDurationPropType(m_duration, m_durationModifier));

	//reportType->reportName(m_reportName->metaDataName());
	reportType->reportName(m_reportName->name());

	oadrReportType::oadrReportDescription_sequence sequence;

	map<string, oadrReportDescriptionType*>::iterator itr;

	for (itr = m_reportDescriptions.begin(); itr != m_reportDescriptions.end(); itr++)
	{
		sequence.push_back(*(itr->second));
	}

	reportType->oadrReportDescription(sequence);

	return reportType;
}

/********************************************************************************/

void ReportWrapper::addInterval(string rID, time_t dtstart, int duration, DurationModifier *durationModifier)
{
	if (m_rIDtoUID.find(rID) != m_rIDtoUID.end())
	{
		OadrException exception(OadrException::RID_EXISTS);
		throw exception;
	}

	int uid = m_rIDtoUID.size();
	m_rIDtoUID[rID] = uid;

	auto_ptr<IntervalType> intervalType(new IntervalType());

	stringstream s;

	s << uid;

	intervalType->uid(icalendar_2_0::uid(s.str()));
	intervalType->dtstart(Oadr2bHelper::timetTo_dtstart(dtstart));
	intervalType->duration(Oadr2bHelper::generateDurationPropType(duration, durationModifier));

	m_intervals.push_back(intervalType);
}

/********************************************************************************/

void ReportWrapper::addReportPayload(string rID, oadrReportPayloadType &reportPayload)
{
	if (m_rIDtoUID.find(rID) == m_rIDtoUID.end())
	{
		OadrException exception(OadrException::NO_INTERVAL);
		throw exception;
	}

	int uid = m_rIDtoUID[rID];

	if (m_uIDtoReportPayload.find(uid) == m_uIDtoReportPayload.end())
	{
		m_uIDtoReportPayload[uid] = vector<oadrReportPayloadType>();
	}

	m_uIDtoReportPayload[uid].push_back(reportPayload);
}

/********************************************************************************/

void ReportWrapper::addOadrReportPayloadFloat(string rID, float value, oadrDataQualityType::value dataQuality, unsigned int confidence, unsigned int accuracy)
{
	auto_ptr<PayloadBaseType> payload(new PayloadFloatType(value));

	auto_ptr<ReportPayloadType::rID_type> rID_ptr(new ReportPayloadType::rID_type(rID));

	oadrReportPayloadType reportPayload(rID_ptr, payload);

	reportPayload.confidence(confidence);
	reportPayload.accuracy(accuracy);

	// TODO: does this need to be changed to oadrDataQualityTypeType
	// if it doesn't, there's on need for the "Value" classes I've been creating
	reportPayload.oadrDataQuality(oadrDataQualityType(dataQuality));

	addReportPayload(rID, reportPayload);
}

/********************************************************************************/

void ReportWrapper::addOadrReportPayloadResourceStatus(string rID, bool online, bool manualOverride, oadr2b::oadr::oadrLoadControlStateType loadControlState, oadrDataQualityType::value dataQuality, unsigned int confidence, unsigned int accuracy)
{
	// TODO: refactor: move common code between this func and the previous to addReportPayload()
	// add auto_ptr<PayloadBaseType> to func
	auto_ptr<oadrPayloadResourceStatusType> payload(new oadrPayloadResourceStatusType(online, manualOverride));

	// add blank load control values for now; these values will need to be exposed at some point
	// or pass the loadControl object to this function
	//payload->oadrLoadControlState(Oadr2bHelper::generateEmptyLoadControlState());
	payload->oadrLoadControlState(loadControlState);
	// everything below should be moved to addReportPayload
	auto_ptr<PayloadBaseType> payloadBase(payload);

	auto_ptr<ReportPayloadType::rID_type> rID_ptr(new ReportPayloadType::rID_type(rID));

	oadrReportPayloadType reportPayload(rID_ptr, payloadBase);

	reportPayload.confidence(confidence);
	reportPayload.accuracy(accuracy);

	// TODO: does this need to be changed to oadrDataQualityTypeType
	// if it doesn't, there's on need for the "Value" classes I've been creating
	reportPayload.oadrDataQuality(oadrDataQualityType(dataQuality));

	addReportPayload(rID, reportPayload);
}

/********************************************************************************/

auto_ptr<oadrReportType> ReportWrapper::generateReport(time_t dtstart, string reportRequestID, time_t createdDateTime)
{
	auto_ptr<oadrReportType> reportType(new oadrReportType(reportRequestID, m_reportSpecifierID, Oadr2bHelper::timetToiCalDateTime(dtstart)));

	if (createdDateTime == 0)
		createdDateTime = time(NULL);

	reportType->createdDateTime(Oadr2bHelper::timetToiCalDateTime(createdDateTime));

	reportType->reportName(m_reportName->name());

	intervals::interval_sequence::iterator interval_itr;

	int uid = 0;
	for (interval_itr = m_intervals.begin(); interval_itr != m_intervals.end(); interval_itr++)
	{
		IntervalType::streamPayloadBase_sequence sequence;

		// TODO: convert UID to int and look associated ReportPayload in the map
		// below.  consider converting m_intervals to a vector to ensure the
		// intervals are iterated in the order they're added
		//string uid = (*interval_itr).uid()->text();

		vector<oadrReportPayloadType> *payloads = &m_uIDtoReportPayload[uid];

		vector<oadrReportPayloadType>::iterator payload_itr;

		for (payload_itr = payloads->begin(); payload_itr != payloads->end(); payload_itr++)
		{
			sequence.push_back(*payload_itr);
		}

		(*interval_itr).streamPayloadBase(sequence);

		uid++;
	}

	stream::intervals i;

	i.interval(m_intervals);

	reportType->intervals(i);

	m_intervals.clear();
	m_uIDtoReportPayload.clear();

	return reportType;
}
