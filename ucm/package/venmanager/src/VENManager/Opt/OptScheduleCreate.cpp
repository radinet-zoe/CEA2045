/*
 * OptScheduleCreate.cpp
 *
 *  Created on: 23-Oct-2014
 *      Author: dinesh
 */

#include "OptScheduleCreate.h"

//Constructor
OptScheduleCreate::OptScheduleCreate()
{

}
/**
    Method to send createopt schedule payload
     @param pointer to class VEN2b
     @param Start time
     @param Stop time
     @param Reason
     @param Resource
     @Return status
 */
bool OptScheduleCreate::OptSchCreate(VEN2b *venCreateOpt, string startTime, string stopTime, string reason, string resource)
{
	ucmlogging log;
	sqliteDB sdb;
	bool Status = false;
	time_t starttime;
	time_t stoptime;
	stringstream stream;
	int duration;

	//to get marketcontext
	reportRegister rr;

	OptReasonValue reasonVal(reason);
	OptSchedule sched(OptTypeType::optOut, reasonVal, rr.getMarketContext(),resource, "");

	DurationModifier dval("SECONDS");
	stream<<startTime;
	stream>>starttime;
	stream.clear();
	stream<<stopTime;
	stream>>stoptime;
	duration = (stoptime-starttime);

	sched.addAvailable(starttime, duration, dval.SECONDS);
	try
	{
		//OptSchCreate Request from VEN
		auto_ptr<CreateOptSchedule> createopt = venCreateOpt->createOptSchedule(sched);

		oadrPayload *payload = createopt->response();
		xmlFileCreation requestLog("oadrCreateOpt.xml", createopt->requestBody());

		if (payload == NULL || !payload->oadrSignedObject().oadrCreatedOpt().present())
		{
			log.logger("Received unexpected payload" , ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to createopt schedule!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("oadrCreatedOpt.xml", createopt->responseBody());
			return false;
		}

		oadrCreatedOptType *response = &payload->oadrSignedObject().oadrCreatedOpt().get();

		if (response->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Response code  from server: " + response->eiResponse().responseCode() + "Failed to  createopt schedule!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("oadrCreatedOpt.xml", createopt->responseBody());
			return false;
		}

		//log file creation.
		xmlFileCreation responseLog("oadrCreatedOpt.xml", createopt->responseBody());
		setOptIdVal(response->optID());
		Status = true;
	}
	catch (CurlException& ex)
	{
		log.logger("caught exception: " + string(ex.what()) + "!!!Failed to createopt schedule!!!",ucmlogging::Error);
		sdb.sqliteDBwriteString("errormsg",string(ex.what())+ "!!!Failed to createopt schedule!!!", hmiven, "hmivenflag");
	}

	return Status;

}
