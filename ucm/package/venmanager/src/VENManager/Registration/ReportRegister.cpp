/*
 * ReportRegister.cpp
 *
 *  Created on: 22-Sep-2014
 *      Author: dinesh
 */


#include "ReportRegister.h"

/**
    registerReport Constructor
    @param venID - VEN ID returned while registration
 */
reportRegister::reportRegister(string venID) :
	m_venID(venID)
{

}

/**
    Method to get partyID
	@return - Single Market Context as string
 */
string reportRegister::getMarketContext()
{
	sqliteDB sdb;
	string marketContext;
	marketContext = sdb.sqliteDBreadString("drprogram", hmiven, "hmiparam");

	stringstream stream;
	stream<<marketContext;
	string singleContext;
	getline(stream, singleContext, ','); //get the first market context
	return singleContext;

}

/**
    Method to Register Report
    @param marketContext - marketContext from main
    @return Status
 */
//TODO: Need to be modified for addDescriptionPowerItem..
bool reportRegister::ReportRegister(VEN2b *venRegisterReport)
{

	auto_ptr<ReportWrapperList> reportWrapperList_status(new ReportWrapperList());
	auto_ptr<ReportWrapperList> reportWrapperList_usage(new ReportWrapperList());
	ReportWrapper *telemetryStatus;
	ReportWrapper *telemetryUsage;
	time_t createdDateTime = time(0);
	sqliteDB sdb;
	ucmlogging log;
	bool regStatus = false;

	try
	{
		//Creating instance and getting values into map from reports.conf file
		ReportHandler rh(getVenId());
		rh.getReportconf();
		rh.extractEnumValues();

		for (stringIterator=rh.rsid_ReportNameMap.begin(); stringIterator!=rh.rsid_ReportNameMap.end(); ++stringIterator)
		{
			string RSID;
			RSID = stringIterator->first;
			ReportName RptName(stringIterator->second);
			size_t found = stringIterator->second.find("TELEMETRY_USAGE");
			
			//Generating register report payload for TELEMETRY_USAGE METADATA report
			if(found!=string::npos)
			{
				//TODO: Duration, durationModifier
				// The duration element value in register report to be decided (now it is kept as 60M)
				telemetryUsage = new ReportWrapper(&RptName,60,DurationModifier::MINUTES, stringIterator->first, createdDateTime);
				VstringIterator  = rh.rsid_rid_Map.find(stringIterator->first);
				
				if(VstringIterator != rh.rsid_rid_Map.end())
				{
					for (vsitr  = VstringIterator->second.begin();vsitr != VstringIterator->second.end(); vsitr++ )
					{
						string rID = *vsitr ;
						map<string, string>::iterator sItr;

						//Getting Report Type of the riD
						sItr = rh.rid_ReportTypeMap.find(rID);
						ReportTypeItr = rh.ReporttypeEnum.find(sItr->second);
						if(!(ReportTypeItr != rh.ReporttypeEnum.end()))
						{
							log.logger("Invalid Report Type", ucmlogging::Error);
							sdb.sqliteDBwriteString("errormsg","Invalid Report Type!!!Failed to register report!!!", hmiven, "hmivenflag");
							return false;
						}

						//Getting Reading Type of the riD
						sItr = rh.rid_ReadingTypeMap.find(rID);
						ReadtypeItr = rh.ReadingtypeEnum.find(sItr->second);
						if(!(ReadtypeItr != rh.ReadingtypeEnum.end()))
						{
							log.logger("Invalid Read Type", ucmlogging::Error);
							sdb.sqliteDBwriteString("errormsg","Invalid Read Type!!!Failed to register report!!!", hmiven, "hmivenflag");
							return false;
						}

						//Getting Item Description of the riD
						sItr = rh.rid_UnitMap.find(rID);
						stringstream item;
						string itemDescription;
						item<<sItr->second;
						while(getline(item, itemDescription, ':'));
						eEnergyItr = rh.eUnit.find(itemDescription);
						if(!(eEnergyItr != rh.eUnit.end()))
						{
							log.logger("Invalid Item Description", ucmlogging::Error);
							sdb.sqliteDBwriteString("errormsg","Invalid Item Description!!!Failed to register report!!!", hmiven, "hmivenflag");
							return false;
						}

						//Getting Item Units of the riD
						sItr = rh.rid_ItemUnitsMap.find(rID);
						if(!(sItr != rh.rid_ItemUnitsMap.end()))
						{
							log.logger("Invalid Item Units", ucmlogging::Error);
							sdb.sqliteDBwriteString("errormsg","Invalid Item Unit!!!Failed to register report!!!", hmiven, "hmivenflag");
							return false;
						}

						//TODO: Multiple Marketcontext add??
						//TODO: Resource ID
						//TODO: onchange value
						/*NOTE: eEnergyItemType map shown below
								ApparentEnergy - eEnergyApparent
								ReactiveEnergy - eEnergyReactive,
								RealEnergy - eEnergyReal
						*/

						telemetryUsage->addDescriptionEnergyItem(rID,"resoruce1",
																 ReportTypeItr->second,
																 ReadtypeItr->second,
																 getMarketContext(),
																 sdb.sqliteDBreadInt("minsampling", hmiven, "hmiparam"),
																 sdb.sqliteDBreadInt("maxsampling", hmiven, "hmiparam"),
																 false,
																 DurationModifier::MINUTES,
																 eEnergyItr->second,
																 sItr->second,
																 SiScaleCodeType::none);


					}
					reportWrapperList_usage->addReportWrapper(telemetryUsage);
					auto_ptr<RegisterReport> registerReport_usage = venRegisterReport->registerReport(reportWrapperList_usage.get());

					xmlFileCreation requestLog(RegReportXMLfolder+RSID+"_oadrRegisterReport_TELEMETRY_USAGE.xml", registerReport_usage->requestBody());
					xmlFileCreation responseLog(RegReportXMLfolder+RSID+"_oadrRegisteredReport_TELEMETRY_USAGE.xml", registerReport_usage->responseBody());

					oadrPayload *payload = registerReport_usage->response();

					if (payload == NULL || !payload->oadrSignedObject().oadrRegisteredReport().present())
					{
						log.logger("Received unexpected payload"  , ucmlogging::Error);
						sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to register report!!!", hmiven, "hmivenflag");
						return false;
					}

					if (payload->oadrSignedObject().oadrRegisteredReport()->eiResponse().responseCode().compare("200") != 0)
					{
						log.logger("Received unexpected response code: " + payload->oadrSignedObject().oadrRegisteredReport()->eiResponse().responseCode(), ucmlogging::Error);;
						sdb.sqliteDBwriteString("errormsg","Received unexpected Response code from server : " + payload->oadrSignedObject().oadrRegisteredReport()->eiResponse().responseCode() + "Failed to register report!!!", hmiven, "hmivenflag");
						return false;
					}
				}
			}

			found = stringIterator->second.find("TELEMETRY_STATUS");
			
			//Generating register report payload for TELEMETRY_STATUS METADATA report
			if(found!=string::npos)
			{
				//TODO: Duration, durationModifier
				telemetryStatus = new ReportWrapper(&RptName, 24, DurationModifier::MINUTES, stringIterator->first, createdDateTime);

				VstringIterator  = rh.rsid_rid_Map.find(stringIterator->first);
				if(VstringIterator != rh.rsid_rid_Map.end())
				{
					for (vsitr  = VstringIterator->second.begin();vsitr != VstringIterator->second.end(); vsitr++ )
					{
						string rID = *vsitr ;

						//TODO: Multiple Marketcontext add??
						//TODO: Resource ID
						telemetryStatus->addDescriptionResourceStatus(rID, "resource1",
																	  getMarketContext(),
																	  sdb.sqliteDBreadInt("minsampling", hmiven, "hmiparam"),
																	  sdb.sqliteDBreadInt("maxsampling", hmiven, "hmiparam"),
																	  false,
																	  DurationModifier::MINUTES);


					}
					reportWrapperList_status->addReportWrapper(telemetryStatus);
					auto_ptr<RegisterReport> registerReport_status = venRegisterReport->registerReport(reportWrapperList_status.get());

					xmlFileCreation requestLog(RegReportXMLfolder+RSID+"_oadrRegisterReport_TELEMETRY_STATUS.xml", registerReport_status->requestBody());
					xmlFileCreation responseLog(RegReportXMLfolder+RSID+"_oadrRegisteredReport_TELEMETRY_STATUS.xml", registerReport_status->responseBody());

					oadrPayload *payload = registerReport_status->response();

					if (payload == NULL || !payload->oadrSignedObject().oadrRegisteredReport().present())
					{
						log.logger("Received unexpected payload"  , ucmlogging::Error);
						sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!Failed to register report!!!", hmiven, "hmivenflag");
						return false;
					}

					if (payload->oadrSignedObject().oadrRegisteredReport()->eiResponse().responseCode().compare("200") != 0)
					{
						log.logger("Received unexpected response code: " + payload->oadrSignedObject().oadrRegisteredReport()->eiResponse().responseCode(), ucmlogging::Error);;
						sdb.sqliteDBwriteString("errormsg","Received unexpected Response code from server : " + payload->oadrSignedObject().oadrRegisteredReport()->eiResponse().responseCode() + "Failed to register report!!!", hmiven, "hmivenflag");
						return false;
					}
				}
			}
		}
		regStatus = true;
	}
	catch(CurlException& ex)
	{
		log.logger("Caught exception: " + string(ex.what()) + "!!!Failed to register report!!!",ucmlogging::Error);
		sdb.sqliteDBwriteString("errormsg",string(ex.what()) + "!!!Failed to register report!!!", hmiven, "hmivenflag");
	}
	return regStatus;
}

