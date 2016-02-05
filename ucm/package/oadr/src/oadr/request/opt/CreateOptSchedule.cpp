//*********************************************************************************************************************
// CreateOptSchedule.cpp
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
#include "CreateOptSchedule.h"

CreateOptSchedule::CreateOptSchedule(string venID, OptSchedule &optSchedule, string requestID) :
	CreateOpt(venID, optSchedule.optID(), optSchedule.optType(), optSchedule.optReasonValue(), requestID),
	m_optSchedule(optSchedule)
{
	m_createdDateTime = 0;
}

/********************************************************************************/

CreateOptSchedule::~CreateOptSchedule()
{
}

/********************************************************************************/

void CreateOptSchedule::createdDateTime(time_t createdDateTime)
{
	m_createdDateTime = createdDateTime;
}

/********************************************************************************/

auto_ptr<oadrPayload> CreateOptSchedule::generatePayload()
{
	icalendar_2_0::DateTimeType icalCreatedDateTime = (m_createdDateTime == 0 ? Oadr2bHelper::nowToiCalDateTime() : Oadr2bHelper::timetToiCalDateTime(m_createdDateTime));
	EiOptType::createdDateTime_type createdDateTime(icalCreatedDateTime);

	oadrCreateOptType::eiTarget_type target;

	auto_ptr<oadrCreateOptType> co(new oadrCreateOptType(optID(), optType(), optReasonValue().toString(), venID(),
			createdDateTime, requestID(), target));

	EiOptType::marketContext_type mc(m_optSchedule.marketContext());
	co->marketContext(mc);

	co->schemaVersion("2.0b");

	co->vavailability(m_optSchedule.optScheduleToVavail());

	if (m_optSchedule.resourceID() != "")
	{
		EiTargetType eitt;

		EiTargetType::resourceID_sequence rid_seq;

		rid_seq.push_back(m_optSchedule.resourceID());

		eitt.resourceID(rid_seq);

		co->eiTarget(eitt);
	}

	auto_ptr<oadrSignedObject> oso(new oadrSignedObject());

	oso->oadrCreateOpt(co);

	auto_ptr<oadrPayload> payload(new oadrPayload(oso));

	return payload;
}

/********************************************************************************/

OptSchedule &CreateOptSchedule::optSchedule()
{
	return m_optSchedule;
}
