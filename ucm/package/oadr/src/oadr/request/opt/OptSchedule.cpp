//*********************************************************************************************************************
// OptSchedule.cpp
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
#include "OptSchedule.h"

OptSchedule::OptSchedule(EiOptType::optType_type optType, OptReasonValue optReasonValue, string marketContext, string resourceID, string optID) :
	m_optType(optType),
	m_optReasonValue(optReasonValue),
	m_marketContext(marketContext),
	m_resourceID(resourceID)
{
	m_optID = (optID != "" ? optID : RandomHex::instance()->generateRandomHex());
}

/********************************************************************************/

OptSchedule::~OptSchedule()
{
}

/********************************************************************************/

void OptSchedule::addAvailable(int year, unsigned short month, unsigned short day,
        unsigned short hours, unsigned short minutes,
        double seconds, int duration, DurationModifier *durationModifier)
{
	tavailable available;

	available.year = year;
	available.month = month;
	available.day = day;
	available.hours = hours;
	available.minutes = minutes;
	available.seconds = seconds;

	available.duration = duration;
	available.durationModifier = durationModifier;

	m_vavailability.push_back(available);
}

/********************************************************************************/

void OptSchedule::addAvailable(time_t dtstart, int duration, DurationModifier *durationModifier)
{
	tm tm_utc;

	gmtime_r(&dtstart, &tm_utc);

	addAvailable(tm_utc.tm_year + 1900, tm_utc.tm_mon + 1, tm_utc.tm_mday, tm_utc.tm_hour, tm_utc.tm_min,
			tm_utc.tm_sec, duration, durationModifier);
}

/********************************************************************************/

void OptSchedule::addAvailable(tm dtstartUTC, int duration, DurationModifier *durationModifier)
{
	addAvailable(dtstartUTC.tm_year + 1900, dtstartUTC.tm_mon + 1, dtstartUTC.tm_mday, dtstartUTC.tm_hour, dtstartUTC.tm_min,
			dtstartUTC.tm_sec, duration, durationModifier);
}

/********************************************************************************/

VavailabilityType OptSchedule::optScheduleToVavail()
{

	VavailabilityType::components_type components;

	ArrayOfVavailabilityContainedComponents::available_sequence asequence;

	vector<tavailable>::iterator itr;

	for (itr = m_vavailability.begin(); itr != m_vavailability.end(); itr++)
	{
		AvailableType::properties_type properties((*itr).get_dtstart(), (*itr).get_duration());

		ArrayOfVavailabilityContainedComponents::available_type available(properties);

		asequence.push_back(available);
	}

	components.available(asequence);

	VavailabilityType vavail(components);

	return vavail;
}

/********************************************************************************/

string OptSchedule::optID()
{
	return m_optID;
}

/********************************************************************************/

string OptSchedule::marketContext()
{
	return m_marketContext;
}

/********************************************************************************/

EiOptType::optType_type OptSchedule::optType()
{
	return m_optType;
}

/********************************************************************************/

OptReasonValue OptSchedule::optReasonValue()
{
	return m_optReasonValue;
}

/********************************************************************************/

string OptSchedule::resourceID()
{
	return m_resourceID;
}

/********************************************************************************/

void OptSchedule::clear()
{
	m_vavailability.clear();
}
