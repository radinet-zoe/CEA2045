//*********************************************************************************************************************
// Oadr2bRequest.h
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
#ifndef OADR2BREQUEST_H_
#define OADR2BREQUEST_H_

#include <string>

#include "../helper/RandomHex.h"

#include <oadrsd/2b/oadrPayload.hxx>
#include <oadrsd/2b/oadrSignedObject.hxx>

#include <oadrsd/2b/schemaVersionType.hxx>

#include <oadrsd/2b/oadr_20b.hxx>

using namespace std;
using namespace oadr2b::oadr;

class Oadr2bRequest
{
private:
	string m_requestType;
	string m_responseType;

	string m_venID;

	string m_requestID;

	string m_responseCode;
	string m_responseDescription;

	string m_requestBody;
	string m_responseBody;

	string m_httpResponseMessage;
	string m_httpResponseCode;

	auto_ptr<oadrPayload> m_request;
	auto_ptr<oadrPayload> m_response;

protected:

	string serializePayload(auto_ptr<oadrPayload> payload);

	virtual auto_ptr<oadrPayload> generatePayload() = 0; //pure virtual function

public:
	Oadr2bRequest(string requestType, string responseType, string requestID);
	Oadr2bRequest(string requestType, string responseType, string venID, string requestID);
	Oadr2bRequest(string requestType, string responseType, string venID, string responseCode, string responseDescription, string requestID);

	virtual ~Oadr2bRequest();

	virtual string generateRequestXML();

	void setHttpFields(string requestBody, string responseBody, string httpResponseCode, string httpResponseMessage);

	string requestID();
	string venID();
	string responseCode();
	string responseDescription();

	string requestType();
	string responseType();

	string responseBody();
	string requestBody();

	string httpResponseCode();
	string httpResponseMessage();

	oadrPayload *request();
	oadrPayload *response();
};

#endif /* OADR2BREQUEST_H_ */
