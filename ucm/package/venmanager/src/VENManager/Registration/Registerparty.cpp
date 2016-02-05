/*
 * Registerparty.cpp
 *
 *  Created on: 25-Aug-2014
 *      Author: dinesh
 */

#include "../Main.h"
#include "Registerparty.h"

using namespace std;

/**
    registerParty Constructor
    @param venID - VEN ID returned while registration
    @param registrationID - Registration ID returned while registration
 */
registerParty::registerParty(string venID, string registrationID):
		venID(venID), registrationID(registrationID)
{

}

/**
    Method to check if registered
    @return - status
 */
bool registerParty::ifregistered()
{
	if (venID == "" || registrationID == "")
		return false;

	return true;
}

/**
    Method to register VEN
    @return - status
 */
bool registerParty::RegisterParty(VEN2b *ven)
{
	ucmlogging log;
	sqliteDB sdb;
	bool regStatus = false;

	if(ifregistered())
	{
		//For re-registration
		log.logger("Doing ReRegistration: " ,ucmlogging::Info);
		try
		{
			auto_ptr<CreatePartyRegistration> cpr = ven->createPartyRegistration(oadrProfileType(oadrProfileType::cxx_2_0b),
										oadrTransportType(oadrTransportType::simpleHttp), "", false, false, true);
			oadrPayload *payload = cpr->response();
			xmlFileCreation requestLog("createPartyRegistration.xml", cpr->requestBody());

			if (payload == NULL || !payload->oadrSignedObject().oadrCreatedPartyRegistration().present())
			{
				log.logger("Received unexpected payload"  , ucmlogging::Error);
				sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to re register!!!", hmiven, "hmivenflag");
				xmlFileCreation responseLog("createdPartyRegistration.xml", cpr->responseBody());
				return false;
			}

			oadrCreatedPartyRegistrationType *response = &payload->oadrSignedObject().oadrCreatedPartyRegistration().get();

			if (response->eiResponse().responseCode().compare("200") != 0)
			{
				log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);
				sdb.sqliteDBwriteString("errormsg","Received unexpected Response code from server : " + response->eiResponse().responseCode() + "Failed to re register!!!", hmiven, "hmivenflag");
				xmlFileCreation responseLog("createdPartyRegistration.xml", cpr->responseBody());
				return false;
			}

			xmlFileCreation responseLog("createdPartyRegistration.xml", cpr->responseBody());

			log.logger("Registered!!! Details Below: " ,ucmlogging::Info);
			log.logger("        VEN ID: " + *response->venID() ,ucmlogging::Info);
			log.logger("RegistrationID: " + *response->registrationID() ,ucmlogging::Info);

			sqliteDB sdb;
			sdb.sqliteDBwriteString("venid",*response->venID(), ven_param, "venparam");
			sdb.sqliteDBwriteString("vtnid",response->vtnID(), ven_param, "venparam");
			sdb.sqliteDBwriteString("registrationid",*response->registrationID(), ven_param, "venparam");
			regStatus = true;
		}
		catch (CurlException& ex)
		{
			log.logger("caught exception: " + string(ex.what()) + "!!!Failed to re register!!!",ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg",string(ex.what()) + "!!!Failed to re register!!!", hmiven, "hmivenflag");
		}
	}
	else
	{
		//Fresh Registration
		log.logger( "Doing fresh Registration: " ,ucmlogging::Info);
		try
		{
			auto_ptr<CreatePartyRegistration> cpr = ven->createPartyRegistration(oadrProfileType(oadrProfileType::cxx_2_0b),
								oadrTransportType(oadrTransportType::simpleHttp), "", false, false, true);

			oadrPayload *payload = cpr->response();

			xmlFileCreation requestLog("createPartyRegistration.xml", cpr->requestBody());

			if (payload == NULL || !payload->oadrSignedObject().oadrCreatedPartyRegistration().present())
			{
				log.logger("Received unexpected payload" , ucmlogging::Error);
				sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to register!!!", hmiven, "hmivenflag");
				xmlFileCreation responseLog("createdPartyRegistration.xml", cpr->responseBody());
				return false;
			}

			oadrCreatedPartyRegistrationType *response = &payload->oadrSignedObject().oadrCreatedPartyRegistration().get();
			if (response->eiResponse().responseCode().compare("200") != 0)
			{
				log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);
				sdb.sqliteDBwriteString("errormsg","Received unexpected Response code  from server: " + response->eiResponse().responseCode() + "Failed to register!!!", hmiven, "hmivenflag");
				xmlFileCreation responseLog("createdPartyRegistration.xml", cpr->responseBody());
				return false;
			}

			xmlFileCreation responseLog("createdPartyRegistration.xml", cpr->responseBody());

			log.logger("Registered!!! Details Below: " ,ucmlogging::Info);
			log.logger("        VEN ID: " + *response->venID() ,ucmlogging::Info);
			log.logger("RegistrationID: " + *response->registrationID() ,ucmlogging::Info);

			sqliteDB sdb;
			sdb.sqliteDBwriteString("venid",*response->venID(), ven_param, "venparam");
			sdb.sqliteDBwriteString("vtnid",response->vtnID(), ven_param, "venparam");
			sdb.sqliteDBwriteString("registrationid",*response->registrationID(), ven_param, "venparam");
			regStatus = true;
		}
		catch (CurlException& ex)
		{
			log.logger("caught exception: " + string(ex.what()) + "!!!Failed to register!!!",ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg",string(ex.what()) + "!!!Failed to register!!!", hmiven, "hmivenflag");
		}
	}
	return regStatus;
}
