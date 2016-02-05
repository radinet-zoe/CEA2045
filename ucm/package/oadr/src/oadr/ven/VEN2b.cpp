//*********************************************************************************************************************
// VEN2b.cpp
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
#include "VEN2b.h"

const string VEN2b::_EndpointEiEvent = "/EiEvent";
const string VEN2b::_EndpointEiReport = "/EiReport";
const string VEN2b::_EndpointEiRegisterParty = "/EiRegisterParty";
const string VEN2b::_EndpointEiOpt = "/EiOpt";
const string VEN2b::_EndpointEiPoll = "/OadrPoll";

/********************************************************************************/

VEN2b::VEN2b(IHttp *http, string baseURL, string venName, string venID, string registrationID) :
	m_http(http),
	m_baseURL(baseURL),
	m_venName(venName),
	m_venID(venID),
	m_registrationID(registrationID)
{
	// initialize xerces; required for some of the code synthesis parsing routines
	// TODO: init/terminate need only be called once.  Should we add an object
	// counter and initialize xerces when the count == 0 and terminate when
	// count goes back to 0?
	XMLPlatformUtils::Initialize();
}

/********************************************************************************/

VEN2b::~VEN2b()
{
	if (m_http != NULL)
		delete (m_http);

	// cleanup xerces
	XMLPlatformUtils::Terminate();
}

/********************************************************************************/

string VEN2b::venID()
{
	return m_venID;
}

/********************************************************************************/

string VEN2b::registrationID()
{
	return m_registrationID;
}

/********************************************************************************/

bool VEN2b::isRegistered()
{
	if (m_venID == "" || m_registrationID == "")
		return false;

	return true;
}

/********************************************************************************/

void VEN2b::postRequest(Oadr2bRequest *request, string endPoint)
{
	string requestBody = request->generateRequestXML();

	m_http->post(m_baseURL + endPoint, requestBody);

	request->setHttpFields(requestBody, m_http->getResponseBody(), m_http->getResponseCode(), m_http->getResponseMessage());
}

/********************************************************************************/

auto_ptr<RequestEvent> VEN2b::requestEvent(string requestID, unsigned int replyLimit)
{
	auto_ptr<RequestEvent> request(new RequestEvent(m_venID, requestID, replyLimit));

	postRequest(request.get(), _EndpointEiEvent);

	return request;
}

/********************************************************************************/

void VEN2b::setIDs(oadrCreatedPartyRegistrationType *cpr)
{
	m_venID = (cpr->venID().present() ? *cpr->venID() : "");
	m_registrationID = (cpr->registrationID().present() ? *cpr->registrationID() : "");
}

/********************************************************************************/

auto_ptr<QueryRegistration> VEN2b::queryRegistration(string requestID)
{
	auto_ptr<QueryRegistration> request(new QueryRegistration(requestID));

	postRequest(request.get(), _EndpointEiRegisterParty);

	// the response should be a oadrCreatedPartyRegistration which contains the VEN ID and
	// registration ID which are needed for future communications
	if (request->response() != NULL && request->response()->oadrSignedObject().oadrCreatedPartyRegistration().present())
	{
		if (request->response()->oadrSignedObject().oadrCreatedPartyRegistration()->eiResponse().responseCode().compare("200") == 0)
		{
			oadrCreatedPartyRegistrationType *cpr = &request->response()->oadrSignedObject().oadrCreatedPartyRegistration().get();

			if (cpr->venID().present() && cpr->venID()->compare("") != 0)
				setIDs(cpr);
		}
	}

	return request;
}

/********************************************************************************/

auto_ptr<CreatePartyRegistration> VEN2b::createPartyRegistration(oadrProfileType profileType, oadrTransportType transportType,
		string transportAddress,
		bool reportOnly, bool xmlSignature, bool httpPullModel, string registrationID, string requestID)
{
	auto_ptr<CreatePartyRegistration> request(new CreatePartyRegistration(m_venID, requestID, registrationID,
			profileType, transportType, transportAddress, reportOnly, xmlSignature, m_venName, httpPullModel));

	postRequest(request.get(), _EndpointEiRegisterParty);

	// the response should be a oadrCreatedPartyRegistration which contains the VEN ID and
	// registration ID which are needed for future communications
	if (request->response() != NULL && request->response()->oadrSignedObject().oadrCreatedPartyRegistration().present())
	{
		if (request->response()->oadrSignedObject().oadrCreatedPartyRegistration()->eiResponse().responseCode().compare("200") == 0)
		{
			oadrCreatedPartyRegistrationType *cpr = &request->response()->oadrSignedObject().oadrCreatedPartyRegistration().get();

			setIDs(cpr);
		}
	}

	return request;
}

/********************************************************************************/

auto_ptr<CancelPartyRegistration> VEN2b::cancelPartyRegistration(string requestID)
{
	auto_ptr<CancelPartyRegistration> request(new CancelPartyRegistration(m_venID, m_registrationID, requestID));

	postRequest(request.get(), _EndpointEiRegisterParty);

	if (request->response() != NULL && request->response()->oadrSignedObject().oadrCreatedPartyRegistration().present())
	{
		string responseCode = request->response()->oadrSignedObject().oadrCreatedPartyRegistration()->eiResponse().responseCode();

		// if the response indicates OK, the request was successful
		// 453 indicates the registrationID was invalid
		// any other return should indicate the cancel was not successful
		if (responseCode.compare("200") == 0 || responseCode.compare("453") == 0)
			clearRegistration();
	}

	return request;
}

/********************************************************************************/

void VEN2b::clearRegistration()
{
	m_venID.clear();
	m_registrationID.clear();
}

/********************************************************************************/

auto_ptr<CreatedEvent> VEN2b::createdEvent(string responseCode, string responseDescription, EventResponses &responses, string requestID)
{
	auto_ptr<CreatedEvent> request(new CreatedEvent(m_venID, responseCode, responseDescription, responses, requestID));

	postRequest(request.get(), _EndpointEiEvent);

	return request;
}

/********************************************************************************/

auto_ptr<RegisterReport> VEN2b::registerReport(ReportWrapperList *reports, string requestID)
{
	auto_ptr<RegisterReport> request(new RegisterReport(m_venID, requestID, reports));

	postRequest(request.get(), _EndpointEiReport);

	return request;
}

/********************************************************************************/

auto_ptr<RegisteredReport> VEN2b::registeredReport(string requestID, string responseCode, string responseDescription)
{
	auto_ptr<RegisteredReport> request(new RegisteredReport(venID(), requestID, responseCode, responseDescription));

	postRequest(request.get(), _EndpointEiReport);

	return request;
}

/********************************************************************************/

auto_ptr<UpdateReport> VEN2b::updateReport(ReportWrapperList *reports, time_t dtstart, string reportRequestID, time_t createdDateTime, string requestID)
{
	auto_ptr<UpdateReport> request(new UpdateReport(m_venID, requestID, reports, dtstart, reportRequestID, createdDateTime));

	postRequest(request.get(), _EndpointEiReport);

	return request;
}

/********************************************************************************/

auto_ptr<CreateOptSchedule> VEN2b::createOptSchedule(OptSchedule &optSchedule, string requestID)
{
	auto_ptr<CreateOptSchedule> request(new CreateOptSchedule(m_venID, optSchedule, requestID));

	postRequest(request.get(), _EndpointEiOpt);

	return request;
}

/********************************************************************************/

auto_ptr<CancelOptSchedule> VEN2b::cancelOptSchedule(string optID, string requestID)
{
	auto_ptr<CancelOptSchedule> request(new CancelOptSchedule(m_venID, optID, requestID));

	postRequest(request.get(), _EndpointEiOpt);

	return request;
}

/********************************************************************************/

auto_ptr<Poll> VEN2b::poll()
{
	auto_ptr<Poll> request(new Poll(m_venID));

	postRequest(request.get(), _EndpointEiPoll);

	return request;
}

/***********************************************************************************/

/********************************************************************************/


auto_ptr<CreatedReport> VEN2b::createdreport(string venID, vector<string> *pendingReportIDs, string responseCode, string responseDescription, string requestID)
{
	auto_ptr<CreatedReport> request(new CreatedReport(m_venID,pendingReportIDs, responseCode, responseDescription, requestID));

	postRequest(request.get(), _EndpointEiReport);

	return request;
}

/***********************************************************************************/


auto_ptr<CanceledReport> VEN2b::canceledreport(string venID, string responseCode, string responseDescription, string requestID, vector<string> *pendingReportIDs)
{
	auto_ptr<CanceledReport> request(new CanceledReport(m_venID, responseCode, responseDescription, requestID, pendingReportIDs));

	postRequest(request.get(), _EndpointEiReport);

	return request;
}

/***********************************************************************************/
