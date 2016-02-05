//*********************************************************************************************************************
// CreateOptEvent.h
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
#ifndef CREATEOPTEVENT_H_
#define CREATEOPTEVENT_H_

#include "CreateOpt.h"

#include "../Oadr2bHelper.h"

class CreateOptEvent: public CreateOpt
{
private:
	string m_eventID;
	unsigned int m_modificationNumber;

protected:
	virtual auto_ptr<oadrPayload> generatePayload();

public:
	CreateOptEvent(string venID, string optID, EiOptType::optType_type optType,
			OptReasonValue optReasonValue, string eventID, unsigned int modificationNumber,
			string requestID);

	virtual ~CreateOptEvent();
};

#endif /* CREATEOPTEVENT_H_ */