//*********************************************************************************************************************
// CreatePartyRegistration.h
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
#ifndef CREATEPARTYREGISTRATION_H_
#define CREATEPARTYREGISTRATION_H_

#include "Oadr2bRequest.h"

#include <oadrsd/2b/oadrCreatePartyRegistrationType.hxx>
#include <oadrsd/2b/oadrCreatedPartyRegistrationType.hxx>

class CreatePartyRegistration: public Oadr2bRequest
{
private:
	string m_registrationID;
	oadrProfileType m_profileType;
	oadrTransportType m_transportType;
	string m_transportAddress;
	bool m_reportOnly;
	bool m_xmlSignature;
	string m_venName;
	bool m_httpPullModel;

protected:
	virtual auto_ptr<oadrPayload> generatePayload();

public:
	CreatePartyRegistration(string venID, string requestID,
			string registrationID, oadrProfileType profileType, oadrTransportType transportType, string transportAddress,
			bool reportOnly, bool xmlSignature, string venName, bool httpPullModel);

	virtual ~CreatePartyRegistration();
};

#endif /* CREATEPARTYREGISTRATION_H_ */
