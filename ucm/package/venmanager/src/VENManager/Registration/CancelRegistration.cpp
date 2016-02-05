/*
 * CancelRegistration.cpp
 *
 *  Created on: 26-Aug-2014
 *      Author: dinesh
 */

#include "stdio.h"
#include <string>
#include "CancelRegistration.h"
#include <oadr/ven/VEN2b.h>
#include <oadr/ven/http/HttpCurl.h>
#include "ClearRegistration.h"
#include "../XMLFileCreation.h"


/**
    cancelParty Constructor
 */
cancelParty::cancelParty()
{

}

/**
    Method to cancel registration
    @Param - VEN2B Class pointer
    @return - status
 */
bool cancelParty::CancelParty(VEN2b *ven)
{
	ucmlogging log;
	sqliteDB sdb;
	bool regStatus = false;
	try
	{
		//Cancel Request from VEN
		auto_ptr<CancelPartyRegistration> cancelRegistration = ven->cancelPartyRegistration();

		oadrPayload *payload = cancelRegistration->response();
		xmlFileCreation requestLog("cancelPartyRegistration.xml", cancelRegistration->requestBody());

		if (payload == NULL || !payload->oadrSignedObject().oadrCanceledPartyRegistration().present())
		{
			log.logger("Received unexpected payload" , ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to cancel registration!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("canceledPartyRegistration.xml", cancelRegistration->responseBody());
			return false;
		}

		oadrCanceledPartyRegistrationType *response = &payload->oadrSignedObject().oadrCanceledPartyRegistration().get();

		if (response->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Response code  from server: " + response->eiResponse().responseCode() + "Failed to  cancel registration!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("canceledPartyRegistration.xml", cancelRegistration->responseBody());
			return false;
		}

		//log file creation.
		xmlFileCreation responseLog("canceledPartyRegistration.xml", cancelRegistration->responseBody());

		log.logger("Cancel Registration Done!!! " ,ucmlogging::Info);
		//Clear Registration
		clearRegistration cr;
		cr.clearregistration();

		regStatus = true;
	}
	catch (CurlException& ex)
	{
		log.logger("caught exception: " + string(ex.what()) + "!!!Failed to cancel registration!!!",ucmlogging::Error);
		sdb.sqliteDBwriteString("errormsg",string(ex.what())+ "!!!Failed to cancel registration!!!", hmiven, "hmivenflag");
	}

	return regStatus;
}


/********************************************************************************/

