//*********************************************************************************************************************
// OptSchedule.h
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
#ifndef OPTSCHEDULE_H_
#define OPTSCHEDULE_H_

#include <vector>

#include <oadrsd/2b/dtstart.hxx>
#include <oadrsd/2b/DateTimeType.hxx>
#include <oadrsd/2b/properties.hxx>
#include <oadrsd/2b/DurationPropType.hxx>
#include <oadrsd/2b/DurationValueType.hxx>

#include <oadrsd/2b/VavailabilityType.hxx>
#include <oadrsd/2b/DateTimeType.hxx>
#include <oadrsd/2b/ArrayOfVavailabilityContainedComponents.hxx>
#include <oadrsd/2b/AvailableType.hxx>

#include <oadrsd/2b/EiOptType.hxx>

#include <oadrsd/2b/OptTypeType.hxx>

#include "OptReasonValue.h"
#include "../DurationModifier.h"

#include "../../helper/RandomHex.h"

using namespace icalendar_2_0;
using namespace std;
using namespace oadr2b::ei;

struct tavailable
{
	int year;
	unsigned short month;
	unsigned short day;
	unsigned short hours;
	unsigned short minutes;
	double seconds;

	unsigned int duration;

	DurationModifier *durationModifier;

	properties::dtstart_type get_dtstart()
	{
		properties::dtstart_type dtstart(icalendar_2_0::DateTimeType(::xml_schema::date_time(year, month, day, hours, minutes, seconds, 0, 0)));

		return dtstart;
	}

	properties::duration_type get_duration()
	{
		stringstream durationString;

		durationString << "PT" << duration << durationModifier->value();

		properties::duration_type duration(DurationPropType::duration_type(durationString.str()));

		return duration;
	}
};

class OptSchedule
{
private:
	vector<tavailable> m_vavailability;

	EiOptType::optType_type m_optType;
	OptReasonValue m_optReasonValue;

	string m_marketContext;

	string m_optID;

	string m_resourceID;

public:
	OptSchedule(EiOptType::optType_type optType, OptReasonValue optReasonValue, string marketContext, string resourceID = "", string optID = "");

	virtual ~OptSchedule();

	void addAvailable(int year, unsigned short month, unsigned short day,
            unsigned short hours, unsigned short minutes,
            double seconds, int duration, DurationModifier *durationModifier);

	void addAvailable(time_t dtstart, int duration, DurationModifier *durationModifier);
	void addAvailable(tm dtstartUTC, int duraiton, DurationModifier *durationModifier);

	VavailabilityType optScheduleToVavail();

	string optID();
	EiOptType::optType_type optType();
	OptReasonValue optReasonValue();
	string marketContext();
	string resourceID();

	void clear();
};

#endif /* OPTSCHEDULE_H_ */
