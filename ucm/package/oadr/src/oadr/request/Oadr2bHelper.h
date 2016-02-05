//*********************************************************************************************************************
// Oadr2bHelper.h
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
#ifndef OADR2BHELPER_H_
#define OADR2BHELPER_H_

#include <time.h>

#include <oadrsd/2b/DateTimeType.hxx>

#include <oadrsd/2b/DurationPropType.hxx>
#include <oadrsd/2b/DurationValueType.hxx>
#include <oadrsd/2b/dtstart.hxx>
#include <oadrsd/2b/oadrLoadControlStateType.hxx>
#include <oadrsd/2b/oadrLoadControlStateTypeType.hxx>

#include "DurationModifier.h"

class Oadr2bHelper
{
private:
	virtual ~Oadr2bHelper();

public:
	Oadr2bHelper();

	static icalendar_2_0::dtstart timetTo_dtstart(time_t &utc);
	static icalendar_2_0::DateTimeType timetToiCalDateTime(time_t &utc);
	static icalendar_2_0::DateTimeType nowToiCalDateTime();

	static string generateDurationValueType(int duration, DurationModifier *durationModifier);
	static icalendar_2_0::DurationPropType generateDurationPropType(int duration, DurationModifier *durationModifier);

	static oadr2b::oadr::oadrLoadControlStateTypeType generateLoadControlStateType(float min, float max, float current, float normal);

	static oadr2b::oadr::oadrLoadControlStateType generateEmptyLoadControlState();
};

#endif /* OADR2BHELPER_H_ */
