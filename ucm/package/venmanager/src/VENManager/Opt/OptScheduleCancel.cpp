/*
 * OptScheduleCancel..cpp
 *
 *  Created on: 23-Oct-2014
 *      Author: dinesh
 */

#include "OptScheduleCancel.h"


//Constructor
OptScheduleCancel::OptScheduleCancel()
{

}
/**
    Method to send createopt schedule payload
     @param pointer to class VEN2b
     @param optID
     @Return status
 */
bool OptScheduleCancel::OptSchCancel(VEN2b *venCancelOpt, string optID)
{
	ucmlogging log;
	sqliteDB sdb;
	bool Status=false;

	try
	{
		//OptSchCancel Request from VEN
		auto_ptr<CancelOptSchedule> cancelopt = venCancelOpt->cancelOptSchedule(optID);

		oadrPayload *payload = cancelopt->response();
		xmlFileCreation requestLog("oadrCancelOpt.xml", cancelopt->requestBody());

		if (payload == NULL || !payload->oadrSignedObject().oadrCanceledOpt().present())
		{
			log.logger("Received unexpected payload"   , ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to cancelopt schedule!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("oadrCanceledOpt.xml", cancelopt->responseBody());
			return false;
		}

		oadrCanceledOptType *response = &payload->oadrSignedObject().oadrCanceledOpt().get();

		if (response->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Response code  from server: " + response->eiResponse().responseCode() + "Failed to  cancelopt schedule!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("oadrCanceledOpt.xml", cancelopt->responseBody());
			return false;
		}

		//log file creation.
		xmlFileCreation responseLog("oadrCanceledOpt.xml", cancelopt->responseBody());
		Status = true;
		}
		catch (CurlException& ex)
		{
			log.logger("caught exception: " + string(ex.what()) + "!!!Failed to cancelopt schedule!!!",ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg",string(ex.what())+ "!!!Failed to cancelopt schedule!!!", hmiven, "hmivenflag");
		}

		return Status;

}
