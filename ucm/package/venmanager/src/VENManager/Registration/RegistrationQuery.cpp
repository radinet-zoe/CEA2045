/*
 * RegistrationQuery.cpp
 *
 *  Created on: 26-Aug-2014
 *      Author: dinesh
 */

#include "RegistrationQuery.h"

using namespace std;

/**
    registrationQuery Constructor
 */
registrationQuery::registrationQuery()
{

}

/**
    Method to query registration
    @Param - VEN2B Class pointer
    @return - status
 */
bool registrationQuery::RegistrationQuery(VEN2b *ven)
{
	ucmlogging log;
	sqliteDB sdb;
	bool regStatus = false;
	log.logger("Query Registration: " ,ucmlogging::Info);
	try
	{
		//query registration request
		auto_ptr<QueryRegistration> queryRegistration = ven->queryRegistration();

		oadrPayload *payload = queryRegistration->response();
		xmlFileCreation requestLog("queryRegistration.xml", queryRegistration->requestBody());

		if (payload == NULL || !payload->oadrSignedObject().oadrCreatedPartyRegistration().present())
		{
			log.logger("Received unexpected payload" , ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to  Query registration!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("createdPartyRegistration.xml", queryRegistration->responseBody());
			return false;
		}

		oadrCreatedPartyRegistrationType *response = &payload->oadrSignedObject().oadrCreatedPartyRegistration().get();

		if (response->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);;
			sdb.sqliteDBwriteString("errormsg","Received unexpected Response code from server : " + response->eiResponse().responseCode() + "Failed to Query registration!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("createdPartyRegistration.xml", queryRegistration->responseBody());
			return false;
		}

		//log file creation.
		xmlFileCreation responseLog("createdPartyRegistration.xml", queryRegistration->responseBody());

		log.logger("Query-Registration Done!!! " ,ucmlogging::Info);
		regStatus = true;
	}
	catch(CurlException& ex)
	{
		log.logger("caught exception: " + string(ex.what()) + "!!!Failed to Query registration!!!",ucmlogging::Error);
		sdb.sqliteDBwriteString("errormsg",string(ex.what()) + "!!!Failed to Query registration!!!", hmiven, "hmivenflag");
	}
	return regStatus;
}

