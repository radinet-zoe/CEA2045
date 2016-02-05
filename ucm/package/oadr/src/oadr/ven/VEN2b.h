//*********************************************************************************************************************
// VEN2b.h
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
#ifndef VEN2B_H_
#define VEN2B_H_

#include <string>

#include <xercesc/util/PlatformUtils.hpp>

using namespace xercesc_3_1;

#include "http/IHttp.h"

#include "../request/CanceledReport.h"
#include "../request/RequestEvent.h"
#include "../request/QueryRegistration.h"
#include "../request/CreatePartyRegistration.h"
#include "../request/CancelPartyRegistration.h"
#include "../request/CreatedEvent.h"

#include "../request/opt/CreateOptSchedule.h"
#include "../request/opt/CreateOptEvent.h"
#include "../request/opt/CancelOptSchedule.h"

#include "../request/report/RegisterReport.h"
#include "../request/report/UpdateReport.h"
#include "../request/report/RegisteredReport.h"

#include "../request/opt/CreateOptSchedule.h"

#include "../request/CreatedReport.h"

#include "../request/Poll.h"



using namespace std;

class VEN2b
{
private:
	IHttp *m_http;
	string m_baseURL;
	string m_venName;
	string m_venID;
	string m_registrationID;

	static const string _EndpointEiEvent;
	static const string _EndpointEiReport;
	static const string _EndpointEiRegisterParty;
	static const string _EndpointEiOpt;
	static const string _EndpointEiPoll;

	void postRequest(Oadr2bRequest *request, string endPoint);

	void setIDs(oadrCreatedPartyRegistrationType *cpr);

public:
	VEN2b(IHttp *http, string baseURL, string venName, string venID = "", string registrationID = "");

	virtual ~VEN2b();

	string venID();
	string registrationID();

	bool isRegistered();

	auto_ptr<RequestEvent> requestEvent(string requestID = "", unsigned int replyLimit = 0);

	auto_ptr<QueryRegistration> queryRegistration(string requestID = "");

	auto_ptr<CreatePartyRegistration> createPartyRegistration(oadrProfileType profileType, oadrTransportType transportType, string transportAddress,
			bool reportOnly, bool xmlSignature, bool httpPullModel, string registrationID = "", string requestID = "");

	auto_ptr<CancelPartyRegistration> cancelPartyRegistration(string requestID = "");

	void clearRegistration();

	auto_ptr<CreatedEvent> createdEvent(string responseCode, string responseDescription, EventResponses &responses, string requestID = "");

	auto_ptr<RegisterReport> registerReport(ReportWrapperList *reports, string requestID = "");

	auto_ptr<UpdateReport> updateReport(ReportWrapperList *reports, time_t dtstart, string reportRequestID, time_t createdDateTime = 0, string requestID = "");

	auto_ptr<RegisteredReport> registeredReport(string requestID, string responseCode, string responseDescription);

	auto_ptr<CreateOptSchedule> createOptSchedule(OptSchedule &optSchedule, string requestID = "");

	auto_ptr<CancelOptSchedule> cancelOptSchedule(string optID, string requestID = "");

	auto_ptr<Poll> poll();

	auto_ptr<CreatedReport> createdreport(string venID, vector<string> *pendingReportIDs,string responseCode="200", string responseDescription="OK", string requestID = "");

	auto_ptr<CanceledReport> canceledreport(string venID, string responseCode="200", string responseDescription="OK", string requestID = "", vector<string> *pendingReportIDs = NULL);


	const string& getBaseUrl() const {
		return m_baseURL;
	}

	void setBaseUrl(const string& baseUrl) {
		m_baseURL = baseUrl;
	}

	const string& getVenName() const {
		return m_venName;
	}

	void setVenName(const string& venName) {
		m_venName = venName;
	}
};

#endif /* VEN2B_H_ */

