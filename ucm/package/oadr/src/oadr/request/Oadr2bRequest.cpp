//*********************************************************************************************************************
// Oadr2bRequest.cpp
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
#include "Oadr2bRequest.h"

Oadr2bRequest::Oadr2bRequest(string requestType, string responseType, string requestID)
{
	m_requestType = requestType;
	m_responseType = responseType;

	m_requestID = (requestID != "" ? requestID : RandomHex::instance()->generateRandomHex());
}

/********************************************************************************/

Oadr2bRequest::Oadr2bRequest(string requestType, string responseType, string venID, string requestID)
{
	m_requestType = requestType;
	m_responseType = responseType;

	m_venID = venID;

	m_requestID = (requestID != "" ? requestID : RandomHex::instance()->generateRandomHex());
}

/********************************************************************************/

Oadr2bRequest::Oadr2bRequest(string requestType, string responseType, string venID,  string responseCode, string responseDescription, string requestID)
{
	m_requestType = requestType;
	m_responseType = responseType;

	m_venID = venID;

	m_responseCode = responseCode;
	m_responseDescription = responseDescription;

	m_requestID = (requestID != "" ? requestID : RandomHex::instance()->generateRandomHex());
}

/********************************************************************************/

Oadr2bRequest::~Oadr2bRequest()
{
}

/********************************************************************************/

string Oadr2bRequest::serializePayload(auto_ptr<oadrPayload> payload)
{
	xml_schema::namespace_infomap map;
	stringstream ss;

	map[""].name = "";
	map[""].schema = "";

	oadrPayload_(ss, *payload, map);

	return ss.str();
}

/********************************************************************************/

void Oadr2bRequest::setHttpFields(string requestBody, string responseBody, string httpResponseCode, string httpResponseMessage)
{
	m_requestBody = requestBody;
	m_responseBody = responseBody;
	m_httpResponseCode = httpResponseCode;
	m_httpResponseMessage = httpResponseMessage;

	if (m_httpResponseCode.compare("200") == 0 && m_responseBody.length() > 0)
	{
		istringstream iss(m_responseBody);

		// TODO: not currently validating the XML; need to figure out
		// how to specify the location
		auto_ptr<oadrPayload> response(oadrPayload_(iss, xsd::cxx::tree::flags::keep_dom | xsd::cxx::tree::flags::dont_validate));

		m_response = response;
	}
}

/********************************************************************************/

string Oadr2bRequest::requestID()
{
	return m_requestID;
}

/********************************************************************************/

string Oadr2bRequest::venID()
{
	return m_venID;
}

/********************************************************************************/

string Oadr2bRequest::responseCode()
{
	return m_responseCode;
}

/********************************************************************************/

string Oadr2bRequest::responseDescription()
{
	return m_responseDescription;
}

/********************************************************************************/

string Oadr2bRequest::requestBody()
{
	return m_requestBody;
}
/********************************************************************************/

string Oadr2bRequest::responseBody()
{
	return m_responseBody;
}

/********************************************************************************/

string Oadr2bRequest::httpResponseCode()
{
	return m_httpResponseCode;
}

/********************************************************************************/

string Oadr2bRequest::httpResponseMessage()
{
	return m_httpResponseMessage;
}

/********************************************************************************/

string Oadr2bRequest::generateRequestXML()
{
	m_request = generatePayload();

	return serializePayload(m_request);
}

/********************************************************************************/

oadrPayload *Oadr2bRequest::request()
{
	return m_request.get();
}

/********************************************************************************/

oadrPayload *Oadr2bRequest::response()
{
	return m_response.get();
}
