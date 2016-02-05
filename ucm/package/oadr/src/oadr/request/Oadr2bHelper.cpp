//*********************************************************************************************************************
// Oadr2bHelper.cpp
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
#include "Oadr2bHelper.h"

Oadr2bHelper::Oadr2bHelper()
{
}

/********************************************************************************/

Oadr2bHelper::~Oadr2bHelper()
{
}

/********************************************************************************/

icalendar_2_0::dtstart Oadr2bHelper::timetTo_dtstart(time_t &utc)
{
	icalendar_2_0::DateTimeType temp_DateTimeType = timetToiCalDateTime(utc);

	icalendar_2_0::dtstart temp_dtstart(temp_DateTimeType);

	return temp_dtstart;
}

/********************************************************************************/

icalendar_2_0::DateTimeType Oadr2bHelper::timetToiCalDateTime(time_t &utc)
{
	tm tm_utc;

	gmtime_r(&utc, &tm_utc);

	icalendar_2_0::DateTimeType dateTimeType(icalendar_2_0::DateTimeType(::xml_schema::date_time(tm_utc.tm_year + 1900,
			tm_utc.tm_mon + 1, tm_utc.tm_mday, tm_utc.tm_hour, tm_utc.tm_min, tm_utc.tm_sec, 0, 0)));

	return dateTimeType;
}

/********************************************************************************/

icalendar_2_0::DateTimeType Oadr2bHelper::nowToiCalDateTime()
{
	time_t now;

	time(&now);

	return timetToiCalDateTime(now);
}

/********************************************************************************/

string Oadr2bHelper::generateDurationValueType(int duration, DurationModifier *durationModifier)
{
	stringstream ss;

	ss << "PT" << duration << durationModifier->value();

	return ss.str();
}

/********************************************************************************/

icalendar_2_0::DurationPropType Oadr2bHelper::generateDurationPropType(int duration, DurationModifier *durationModifier)
{
	string value = generateDurationValueType(duration, durationModifier);
	icalendar_2_0::DurationPropType durationProp(value);

	return durationProp;
}

/********************************************************************************/

oadr2b::oadr::oadrLoadControlStateTypeType Oadr2bHelper::generateLoadControlStateType(float min, float max, float current, float normal)
{
	oadr2b::oadr::oadrLoadControlStateTypeType loadControlState(current);

	loadControlState.oadrMin(min);
	loadControlState.oadrMax(max);
	loadControlState.oadrNormal(normal);

	return loadControlState;
}

/********************************************************************************/

oadr2b::oadr::oadrLoadControlStateType Oadr2bHelper::generateEmptyLoadControlState()
{
	oadr2b::oadr::oadrLoadControlStateType loadControlState;

	loadControlState.oadrCapacity(generateLoadControlStateType(0.0, 0.0, 0.0, 0.0));
	loadControlState.oadrLevelOffset(generateLoadControlStateType(0.0, 0.0, 0.0, 0.0));
	loadControlState.oadrPercentOffset(generateLoadControlStateType(0.0, 0.0, 0.0, 0.0));
	loadControlState.oadrSetPoint(generateLoadControlStateType(0.0, 0.0, 0.0, 0.0));

	return loadControlState;
}
