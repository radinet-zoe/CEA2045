/*
 * ReportHandler.cpp
 *
 *  Created on: 19-Sep-2014
 *      Author: dinesh
 */


#include "ReportHandler.h"


char *notifierReport = new char[20];
char *notifierReportVal = new char[20];

/**
    ReportHandler Constructor
    @param venID - VEN ID returned while registration
 */
ReportHandler::ReportHandler(string venID) :
	m_venID(venID)
{
	responseCode = 0;
}

/**
    Method to extract values from dplogging.csv logging file
    The position of each datapoint is defined in dplogging.conf file

    @param rID - Data point rid value
    @param parameter - Parameter to be extracted
    @return string - value.
 */
string ReportHandler::logfileExtractValue (string RID, string parameter)
{
	ucmlogging log;
	ifstream fin;
	string locationIndex="";
	unsigned int PosNumber =0;
	unsigned int posIndex;

	// Reading the dplogging.conf file, to identify the position of the datapoint in dplogging.csv
	fin.open(dpconffile);

	if(fin.fail())
	{
		log.logger("Failed to open dplogging.conf file!!!", ucmlogging::Error);
		return locationIndex;
	}
	while(!fin.eof())
	{
		string aLine = "";
		getline(fin,aLine);
		stringstream stream;
		int word = 0;
		stream<<aLine;
		string Parameter, singleContext;
		while(getline(stream, singleContext, '='))
		{
			if(!word)
				Parameter = singleContext;
			else
			{
				if(Parameter.compare("rID" + RID + "_" + parameter)==0)
				{
					locationIndex = singleContext;
					break;
				}
			}
			word++;
		}
	}
	fin.close();
	try
	{
		posIndex = atoi(locationIndex.c_str());
	}
	catch(...)
	{
		log.logger("Invalid Location Index for - rID"+ RID + "_" + parameter+ "!!! Using 0.", ucmlogging::Error);
		return "";
	}

	//get the data point value from dplogging.csv file
	//format of the file will be date,value1,value2,...valuen
	//position of each datapoint is defined in dplogging.conf
	fin.open(dplogfile);
	if(fin.fail())
	{
		log.logger("Failed to open dplogging.csv file!!!", ucmlogging::Error);
		return "";
	}


	fin.seekg(-2,ios_base::end);                // go to one spot before the EOF
	bool keepLooping = true;
	while(keepLooping)
	{
		char ch;
		fin.get(ch);                            // Get current byte's data

		if((int)fin.tellg() <= 1)				// The first line is the last line
		{
			fin.seekg(0);
			keepLooping = false;
		}
		else if(ch == '\n') 					// If the data was a newline indicates the last line in file
			keepLooping = false;
		else									// If the data was neither a newline nor at the 0 byte
			fin.seekg(-2,ios_base::cur);
	}
	string lastLine;
	getline(fin, lastLine);

	string retValue;
	stringstream stream;
	stream<<lastLine;
	while(getline(stream, retValue, ','))
	{
		if(PosNumber == posIndex)
			return retValue;
		PosNumber++;
	}
	fin.close();
	log.logger("Failed to read from dplogging.csv file!!!", ucmlogging::Error);
	return "";

}

/**
    Method to extract Payload Values (from dplogging.csv), for generating Update reports of type Telemetry Usage
    The position of each datapoint is defined in dplogging.conf file

    @param rID - Data point rid value
    @param payloadValue pointer - to store the float value of the rid (in case of telemetry usage)
 */

void ReportHandler::extractPayloadValUsage (string rID, float *payloadValue)
{
	ucmlogging log;
	// Extracting the value of rID from the data points logging file
	string payloadFloat = logfileExtractValue(rID, "payloadFloat:value");
	if((payloadFloat.compare("")))
	{
		try
		{
			*payloadValue = (float)atof(payloadFloat.c_str());
		}
		catch(...)
		{
			log.logger("Invalid value read for payloadFloat:value!!! Using 0. ", ucmlogging::Error);
			*payloadValue = 0;
		}
	}
	else
		*payloadValue = 0;
}

/**
    Method to extract Payload Values (from dplogging.csv), for generating Update reports of type Telemetry Status
    The position of each datapoint is defined in dplogging.conf file

    @param rID - Data point rid value
    @param CapMax pointer - to store the float value of the oadrCapacity:oadrMax
    @param CapCurrent pointer - to store the float value of oadrCapacity:oadrCurrent
    @param online pointer - to store the bool value of oadrOnline
    @param manual pointer - to store the bool value of oadrManualOverride
 */
void ReportHandler::extractPayloadValStatus (string rID, float *CapMax, float *CapCurrent, bool *online, bool *manual)
{
	ucmlogging log;

	if(!(logfileExtractValue(rID, "oadrPayloadResourceStatus:oadrOnline")).compare("true"))
		*online = true;
	else
		*online = false;

	if(!(logfileExtractValue(rID, "oadrPayloadResourceStatus:oadrManualOverride")).compare("true"))
		*manual = true;
	else
		*manual = false;

	string oadrCapacity = logfileExtractValue(rID, "oadrPayloadResourceStatus:oadrCapacity:oadrMax");

	if((oadrCapacity.compare("")))
	{
		try
		{
			*CapMax = (float)atof(oadrCapacity.c_str());
		}
		catch(...)
		{
			log.logger("Invalid Value read for oadrPayloadResourceStatus:oadrCapacity:oadrMax!!! Using 0. ", ucmlogging::Error);
			*CapMax = 0;
		}
	}
	else
		*CapMax = 0;

	oadrCapacity = logfileExtractValue(rID, "oadrPayloadResourceStatus:oadrCapacity:oadrCurrent");

	if((oadrCapacity.compare("")))
	{
		try
		{
			*CapCurrent = (float)atof(oadrCapacity.c_str());
		}
		catch(...)
		{
			log.logger("Invalid Value read for oadrPayloadResourceStatus:oadrCapacity:oadrCurrent!!! Using 0. ", ucmlogging::Error);
			*CapCurrent = 0;
		}
	}
	else
		*CapCurrent = 0;
}

/**
    Method to notify a report thread (to cancel)
    @param cvIndex - index number
    @param cvRpt - condition_variable array
    @param notifierP - value location

 */
void ReportHandler::reportWaitNotifier(int cvIndex, condition_variable cvRpt[], char *notifierP)
{
	ucmlogging log;
	unsigned int count = 3;
	int threadVal, previousthreadVal;
	sqliteDB sdb;
	// Reading the number of report threads running
	previousthreadVal = sdb.sqliteDBreadInt("reportthread", thread_status, "threadstatus");
	log.logger("Notifying REPORT", ucmlogging::Info);
	// Setting the notifier value to "REPORT"
	strcpy(notifierP,"REPORT");
	cvRpt[cvIndex].notify_all();
	sleep(5);

	threadVal = sdb.sqliteDBreadInt("reportthread", thread_status, "threadstatus");
	// Reading the number of report threads running, and checking whether the count has been decremented by one.
	// If not, send another notification and wait for 10 sec.
	while(((threadVal+1) - previousthreadVal) && count--)
	{
		log.logger("Notifying REPORT", ucmlogging::Info);
		strcpy(notifierP,"REPORT");
		cvRpt[cvIndex].notify_all();
		sleep(10);
	}
	strcpy(notifierP,"NULL");
}

/**
   Method to read reports.conf file and save the contents in maps
   map<string, vector<string>> rsid_rid_Map; // association b/w rsid & array of rids
   map<string, vector<string>> ridPayloadMap; // association b/w rid & array of oadrReportPayloads
   map<string, string> rsid_ReportNameMap; // association b/w rsid & report name
   map<string, string> rid_ReadingTypeMap; // association b/w rid & reading type
   map<string, string> rid_ReportTypeMap; // association b/w rid & reporttype
   map<string, string> rid_UnitMap; // association b/w rid & unit (used in Telemetry usage maps)
   map<string, string> rid_ItemUnitsMap; // association b/w rid & itemunit (used in Telemetry usage maps)

 */

void ReportHandler::getReportconf(void)
{
	ucmlogging log;
	ifstream fin;
	fin.open(reportConffile);
	string RSID;
	string RID;
	if(fin.fail())
	{
		log.logger("Failed to open reports.conf file!!!", ucmlogging::Error);
		return;
	}
	while(!fin.eof())
	{
		string aLine = "";
		getline(fin,aLine);
		stringstream stream;
		stream<<aLine;
		size_t RSIDLine = !aLine.find("[");
		if(RSIDLine)
		{
			aLine.erase (0,1);
			size_t pos = aLine.find("]");
			aLine.erase (pos,1);
			RSID = aLine;
			rsid_rid_Map[RSID];
		}
		if(!RSIDLine)
		{
			string singleContext;
			int word = 0;
			string parameter;
			while(getline(stream, singleContext, '='))
			{
				if(!word)
					parameter = singleContext;
				else
				{
					if(parameter.compare(RSID + "_rID")==0)
					{
						RID = singleContext;
						rsid_rid_Map[RSID].push_back(RID);
						ridPayloadMap[RID];
					}
					if(parameter.compare("reportName")==0)
						rsid_ReportNameMap.insert (pair<string,string>(RSID,singleContext));
					if(parameter.compare("reportType")==0)
						rid_ReportTypeMap.insert (pair<string,string>(RID,singleContext));
					if(parameter.compare("readingType")==0)
						rid_ReadingTypeMap.insert (pair<string,string>(RID,singleContext));
					if(parameter.compare("Unit")==0)
						rid_UnitMap.insert (pair<string,string>(RID,singleContext));
					if(parameter.compare("itemUnits")==0)
						rid_ItemUnitsMap.insert (pair<string,string>(RID,singleContext));
					if(parameter.compare("oadrReportPayload")==0)
						ridPayloadMap[RID].push_back(singleContext);
				}
				word++;
			}
		}
	}
	fin.close();
	log.logger("Sucessfully read reports.conf file!!!", ucmlogging::Info);
}

/**
   Method to read reportrequests.data file and save it in maps.
   The file stores the details of pending reports
   map<string, string> rrid_rsidMap; // association b/w rrid & rsid
   map<string, string> rrid_reportTypeMap; // association b/w rrid & reporttype (Telemetry or Metadata)
   map<string, string> rrid_granMap; // association b/w rrid & granularity
   map<string, string> rrid_rbdurationMap; // association b/w rrid & reprtback duration
   map<string, string> rrid_durMap; // association b/w rrid & duration
   map<string, time_t> rrid_startMap; // association b/w rrid & dtstart
   map<string, unsigned int> rrid_cvMap; // association b/w rrid & conditionvaribale index

 */

void ReportHandler::getReportdata(void)
{
	sqliteDB sdb;
	struct stat st = {0};
	if (!(stat(reportdetails, &st) == -1))
	{
		sdb.readDBreportdetails(&rrid_rsidMap,
								&rrid_reportTypeMap,
								&rrid_granMap,
								&rrid_rbdurationMap,
								&rrid_durMap,
								&rrid_startMap,
			 	 	 	    	&rrid_cvMap);
	}
}

/**
   Method to extract EnumValues and save it in map
 */
void ReportHandler::extractEnumValues(void)
{
	//Extracting reporttype enum values
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("usage",ReportEnumeratedType::usage));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("reading",ReportEnumeratedType::reading));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("demand",ReportEnumeratedType::demand));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("setPoint",ReportEnumeratedType::setPoint));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("deltaUsage",ReportEnumeratedType::deltaUsage));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("deltaSetPoint",ReportEnumeratedType::deltaSetPoint));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("deltaDemand",ReportEnumeratedType::deltaDemand));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("baseline",ReportEnumeratedType::baseline));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("deviation",ReportEnumeratedType::deviation));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("avgUsage",ReportEnumeratedType::avgUsage));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("avgDemand",ReportEnumeratedType::avgDemand));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("operatingState",ReportEnumeratedType::operatingState));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("upRegulationCapacityAvailable",ReportEnumeratedType::upRegulationCapacityAvailable));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("downRegulationCapacityAvailable",ReportEnumeratedType::downRegulationCapacityAvailable));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("regulationSetpoint",ReportEnumeratedType::regulationSetpoint));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("storedEnergy",ReportEnumeratedType::storedEnergy));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("targetEnergyStorage",ReportEnumeratedType::targetEnergyStorage));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("availableEnergyStorage",ReportEnumeratedType::availableEnergyStorage));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("price",ReportEnumeratedType::price));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("level",ReportEnumeratedType::level));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("powerFactor",ReportEnumeratedType::powerFactor));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("percentUsage",ReportEnumeratedType::percentUsage));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("percentDemand",ReportEnumeratedType::percentDemand));
	ReporttypeEnum.insert(pair<string, ReportEnumeratedType>("x_resourceStatus",ReportEnumeratedType::x_resourceStatus));

	//Extracting readingType enum values
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Direct_Read",ReadingTypeEnumeratedType::Direct_Read));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Net",ReadingTypeEnumeratedType::Net));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Allocated",ReadingTypeEnumeratedType::Allocated));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Estimated",ReadingTypeEnumeratedType::Estimated));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Summed",ReadingTypeEnumeratedType::Summed));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Derived",ReadingTypeEnumeratedType::Derived));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Mean",ReadingTypeEnumeratedType::Mean));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Peak",ReadingTypeEnumeratedType::Peak));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Hybrid",ReadingTypeEnumeratedType::Hybrid));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Contract",ReadingTypeEnumeratedType::Contract));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("Projected",ReadingTypeEnumeratedType::Projected));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("x_RMS",ReadingTypeEnumeratedType::x_RMS));
	ReadingtypeEnum.insert(pair<string, ReadingTypeEnumeratedType>("x_notApplicable",ReadingTypeEnumeratedType::x_notApplicable));

	//Extracting unit values
	eUnit.insert(pair<string, ReportWrapper::eEnergyItemType>("ApparentEnergy",ReportWrapper::eEnergyItemType::eEnergyApparent));
	eUnit.insert(pair<string, ReportWrapper::eEnergyItemType>("ReactiveEnergy",ReportWrapper::eEnergyItemType::eEnergyReactive));
	eUnit.insert(pair<string, ReportWrapper::eEnergyItemType>("RealEnergy",ReportWrapper::eEnergyItemType::eEnergyReal));
	pUnit.insert(pair<string, ReportWrapper::ePowerItemType>("ApparentPower",ReportWrapper::ePowerItemType::ePowerApparent));
	pUnit.insert(pair<string, ReportWrapper::ePowerItemType>("ReactivePower",ReportWrapper::ePowerItemType::ePowerReactive));
	pUnit.insert(pair<string, ReportWrapper::ePowerItemType>("RealPower",ReportWrapper::ePowerItemType::ePowerReal));
}

/**
    Method to extract duration element from payload.
    @param duration in string
    @return - duration in minutes as integer
*/
unsigned int  ReportHandler::durationExtract (string duration)
{
	unsigned int Duration;
	unsigned pos = duration.find_last_of("T");
	string str3 = duration.substr(pos+1);
	string str4 = str3.substr(0, str3.size()-1);
	stringstream convert(str4);
	if (!(convert >> Duration))
		Duration = 0;

	return Duration;
}

/**
    Method to validate, whether report start time is greater than the current time (for a new report request).
    @param current time since the epoch type
    @param report time since the epoch type
    @return - status
*/
bool ReportHandler::reportDateTimeCheck(time_t current, time_t report)
{
	double diff = difftime(report, current);
	if(diff>0)
		return true;
	else
		return false;
}

/**
    Method to extract the report dtstart time from payload.
    @param report time from iteration
    @return - report time since the epoch type
*/
time_t ReportHandler::getReportTime(oadrCreateReportType::oadrReportRequest_iterator RptItr)
{
	struct tm reportTime;
	reportTime.tm_isdst = 0;
	reportTime.tm_year= RptItr->reportSpecifier().reportInterval().get().properties().dtstart().date_time().year()-1900;
	reportTime.tm_mon = RptItr->reportSpecifier().reportInterval().get().properties().dtstart().date_time().month()-1;
	reportTime.tm_mday= RptItr->reportSpecifier().reportInterval().get().properties().dtstart().date_time().day();
	reportTime.tm_hour= RptItr->reportSpecifier().reportInterval().get().properties().dtstart().date_time().hours();
	reportTime.tm_min = RptItr->reportSpecifier().reportInterval().get().properties().dtstart().date_time().minutes();
	reportTime.tm_sec = RptItr->reportSpecifier().reportInterval().get().properties().dtstart().date_time().seconds();

	time_t rptTime = mktime(&reportTime);
	return rptTime;

}

/**
    Method to insert a valid report data to database (the report request maps), to update it in reportrequests.data file
    @param RRID
    @param RSID
    @param reportType
    @param granularity
    @param reportBackduration
    @param dtstart - event start time in epoch type
    @param duration - duration in minutes
    @param cvIndex - cvIndex for notification
 */
void ReportHandler::insertDB(string RRID, string RSID, string reportType, string granularity,
		      string reportBackduration,  time_t startTime, string duration,
			  unsigned int cvIndex)
{
	rrid_rsidMap.insert(pair<string, string>(RRID,RSID));
	rrid_reportTypeMap.insert (pair<string,string>(RRID,reportType));
	rrid_granMap.insert (pair<string,string>(RRID,granularity));
	rrid_rbdurationMap.insert (pair<string,string>(RRID,reportBackduration));
	rrid_startMap.insert (pair<string,time_t>(RRID,startTime));
	rrid_durMap.insert (pair<string,string>(RRID,duration));
	rrid_cvMap.insert (pair<string,unsigned int>(RRID,cvIndex));

}

/**
    Method to clear the DB maps, used for handling pending reports.
    @return nothing
 */
void ReportHandler::clearDB(void)
{
	rrid_rsidMap.clear();
	rrid_reportTypeMap.clear();
	rrid_granMap.clear();
	rrid_rbdurationMap.clear();
	rrid_startMap.clear();
	rrid_durMap.clear();
	rrid_cvMap.clear();
}

/**
    Method to delete the details of a report request from reportrequests.data file.
    @param reportRequestID
    @return status
 */
bool ReportHandler::updateDataFile(string reportRequestID)
{
	string removeFile;
	sqliteDB sdb;
	ucmlogging log;
	removeFile = string(XMLFilePath)+ string(ReportXMLfolder)+ reportRequestID+"_oadrCreateReport.xml";
	remove(removeFile.c_str());
	// Decrement the number of report threads by one.
	int threadVal = sdb.sqliteDBreadInt("reportthread", thread_status, "threadstatus");
	if(!(sdb.sqliteDBwriteInt("reportthread",(threadVal-1), thread_status, "threadstatus")))
	{
		log.logger("Failed to update threadstatus.db file for - " +reportRequestID+ " !!!", ucmlogging::Error);
		return false;
	}

	if(!(sdb.sqliteDBdeletestring("reportdetails", reportdetails, "reportrequestid", reportRequestID)))
	{
		log.logger("Failed to update reportrequests.data file for - " +reportRequestID+ " !!!", ucmlogging::Error);
		return false;
	}
	return true;
}

/**
   Method to send Update report of type TELEMETRY_USAGE.
 
   @param VEN2b - class
   @param telemetryUsage - ReportWrapper
   @param Reportdtstart
   @param Report Request ID
   @param createdDateTime
 */
void ReportHandler::updateReportUsage (VEN2b *venReport, ReportWrapper *telemetryUsage, time_t Reportdtstart,string RRID,time_t createdDateTime)
{
	ucmlogging log;
	auto_ptr<ReportWrapperList> reportWrapperList_usage(new ReportWrapperList());

	reportWrapperList_usage->addReportWrapper(telemetryUsage);
	log.logger("Sending UpdateReport Payload for - " + RRID, ucmlogging::Info);

	try
	{
		auto_ptr<UpdateReport> updReport = venReport->updateReport(reportWrapperList_usage.get(), Reportdtstart, RRID, createdDateTime);

		oadrPayload *payload = updReport->response();
		oadrUpdatedReportType *response;

		if (payload == NULL || !payload->oadrSignedObject().oadrUpdatedReport().present())
			log.logger("Received unexpected payload " , ucmlogging::Error);
		else
		{
			response = &payload->oadrSignedObject().oadrUpdatedReport().get();

			if (response->eiResponse().responseCode().compare("200") != 0)
				log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);;
		}
		xmlFileCreation requestLog(string(ReportXMLfolder)+"oadrUpdateReport.xml", updReport->requestBody());
		xmlFileCreation responseLog(string(ReportXMLfolder)+"oadrUpdatedReport.xml", updReport->responseBody());
	}
	catch (CurlException& ex)
	{
		log.logger("Caught exception: " + string(ex.what())  + "!!!Failed to send updateReport!!!",ucmlogging::Error);
	}

}

/**
   Method to send Update report of type TELEMETRY_STATUS.

   @param VEN2b - class
   @param telemetryUsage - ReportWrapper
   @param Reportdtstart
   @param Report Request ID
   @param createdDateTime
 */

void ReportHandler::updateReportStatus (VEN2b *venReport, ReportWrapper *telemetryStatus, time_t Reportdtstart,string RRID,time_t createdDateTime)
{
	ucmlogging log;
	auto_ptr<ReportWrapperList> reportWrapperList_status(new ReportWrapperList());

	reportWrapperList_status->addReportWrapper(telemetryStatus);
	log.logger("Sending UpdateReport Payload for - " + RRID, ucmlogging::Info);
	try
	{
		auto_ptr<UpdateReport> updReport = venReport->updateReport(reportWrapperList_status.get(), Reportdtstart, RRID, createdDateTime);

		oadrPayload *payload = updReport->response();
		oadrUpdatedReportType *response;
		if (payload == NULL || !payload->oadrSignedObject().oadrUpdatedReport().present())
			log.logger("Received unexpected payload" , ucmlogging::Error);
		else
		{
			response = &payload->oadrSignedObject().oadrUpdatedReport().get();
			if (response->eiResponse().responseCode().compare("200") != 0)
				log.logger("Received unexpected response code: " + response->eiResponse().responseCode(), ucmlogging::Error);;
		}
		xmlFileCreation requestLog(string(ReportXMLfolder)+"oadrUpdateReport.xml", updReport->requestBody());
		xmlFileCreation responseLog(string(ReportXMLfolder)+"oadrUpdatedReport.xml", updReport->responseBody());
	}
	catch (CurlException& ex)
	{
		log.logger("caught exception: " + string(ex.what()) + "!!!Failed to send updateReport!!!",ucmlogging::Error);
	}
}

/**
   Method to handle CreateReportRequest for metadata reports (RSID= "METADATA") [not tested].
   This will handle METADATA report requests with:
   Reportbackduration = 0, one time reporting
   Reportbackduration != 0, send METADATA register reports, on every reportbackduration for a duration specified by "duration" element.
   duration = 0, infinite reporting
   duration != 0, send METADATA register reports, on every reportbackduration for a duration specified by "duration" element.

   @param pointer to class VEN2b
   @param RptItr - Single Report Payload
   @param marketContext - marketContext from Main
   @param Condition Variable array - To Notify pending reports
   @param Condition Variable - To Notify
   @param Notifier Pointer - To Notify
   @return nothing
 */

void ReportHandler::metadataThread(VEN2b *venReport, oadrCreateReportType::oadrReportRequest_iterator RptItr, condition_variable cvRpt[], condition_variable *cv,char *notifierP)
{
	std::mutex cv_Rpt;
	unique_lock<std::mutex> lkMetaThread(cv_Rpt);
	ucmlogging log;
	bool finish;
	int ReportBackDuration, Duration;
	reportRegister rr(getVenId());
	ReportHandler rh(getVenId());

	// Read the duration elements
	ReportBackDuration = durationExtract(RptItr->reportSpecifier().reportBackDuration().duration());
	Duration = durationExtract(RptItr->reportSpecifier().reportInterval().get().properties().duration().duration());

	notifierReport = notifierP;
	strcpy(notifierReportVal,"REPORT");

	using std::chrono::system_clock;
	time_t waitTimeout, Reportdtstart;
	waitTimeout = getReportTime(RptItr);
	Reportdtstart = waitTimeout;
	// To mark the end of reporting (either cancel or end of duration), for this particular report request
	finish = true;
	string RRID = RptItr->reportRequestID();
	string RSID = RptItr->reportSpecifier().reportSpecifierID();

	while(finish)
	{
		log.logger(RRID + "  Waiting for Report dtstart time - " + asctime(localtime(&waitTimeout)), ucmlogging::Info);

		if(cv->wait_until(lkMetaThread, system_clock::from_time_t(waitTimeout),[](){return (!strcmp(notifierReport,notifierReportVal));}))
		{
			log.logger("Notify Received for Report Thread - "+ RRID +
				       " Stopping Report thread!!!", ucmlogging::Info);
			updateDataFile(RRID);
			finish = false;
		}
		else
		{
			log.logger("Wait timed out for report - " + RRID ,ucmlogging::Info);

			//Read the list of pending reports
			rh.getReportdata();
			for (rh.intIterator=rh.rrid_cvMap.begin(); rh.intIterator!=rh.rrid_cvMap.end(); rh.intIterator++)
			{
				//Before sending a new Register report, all the pending reports needs to be cancelled (Conformance rule 306)
				//notify all pending report except this (METADATA) report request.
				//Assumed that reportSpecifierID of metadata report is "METADATA"
				rh.stringIterator = rh.rrid_reportTypeMap.find(rh.intIterator->first);
				if(strcmp(rh.stringIterator->second.c_str(),RSID.c_str()))
				{
					reportWaitNotifier(rh.intIterator->second, cvRpt, notifierP);
					updateDataFile(rh.intIterator->first);
				}
			}

			if(ReportBackDuration == 0) // One time reporting
			{
				rr.ReportRegister(venReport);
				//delete the corresponding report request entry from reportrequets.data file
				updateDataFile(RRID);
				finish = false;
			}
			else //ReportBackDuration!=0
			{
				rr.ReportRegister(venReport);

				if ((difftime(waitTimeout,Reportdtstart)>=((Duration - ReportBackDuration) * 60.0)) && (Duration != 0))
				{
					//The report request ran for the entire duration
					//delete the corresponding report request entry from reportrequets.data file
					updateDataFile(RRID);
					finish = false;
				}
				// Assuming there is no granularity element for a METADATA report request
				waitTimeout = waitTimeout + (ReportBackDuration * 60);
			}
		}
	}
	log.logger("-------Exit from Report Thread - " + RRID, ucmlogging::Info);
}

/**
   Method to handle CreateReportRequest for non-metadata reports.
   This will handle non-METADATA report requests with:
   Reportbackduration = 0, one time reporting
   Reportbackduration != 0, send datapoint values(collected at granularity interval), on every reportbackduration for a duration specified by "duration" element.
   granularity = 0, means granulairty value = oadrSamplingRate:oadrMinPeriod, and reporting only on change in data-point values
   granularity != 0
   duration = 0, infinite reporting
   duration != 0, send datapoint values(collected at granularity interval), on every reportbackduration for a duration specified by "duration" element.

   @param pointer to class VEN2b
   @param RptItr - Single Report Payload
   @param marketContext - marketContext from Main
   @param Condition Variable array - To Notify pending reports
   @param Condition Variable - To Notify
   @param Notifier Pointer - To Notify
   @return nothing
 */
void ReportHandler::reportThread(VEN2b *venReport, oadrCreateReportType::oadrReportRequest_iterator RptItr, condition_variable *cv,char *notifierP)
{
	std::mutex cv_Rpt;
	unique_lock<std::mutex> lkreportThread(cv_Rpt);
	ucmlogging log;
	sqliteDB sdb;

	string RSID, RRID;
	bool finish, Valueupdated=true ,online, manual, statusReport = false, usageReport =false;
	float CapMax, CapCurrent, payloadValue;
	int ReportBackDuration, Granularity, Duration, newGranularity, Rptbktime = 0;
	bool addInterval=true;
	bool firstEntry = true; //for adding logdata to db in case of granularity==0 for first time.

	// Taking granularity, reportbackduration and duration from the payload.
	Granularity = durationExtract(RptItr->reportSpecifier().granularity().duration());
	ReportBackDuration = durationExtract(RptItr->reportSpecifier().reportBackDuration().duration());
	Duration = durationExtract(RptItr->reportSpecifier().reportInterval().get().properties().duration().duration());
	log.logger("Report Thread - " +RptItr->reportRequestID() +" started!!!",ucmlogging::Info);
	// Used while notifying report threads
	notifierReport = notifierP;
	strcpy(notifierReportVal,"REPORT");
	using std::chrono::system_clock;
	time_t waitTimeout, Reportdtstart;

	RSID = RptItr->reportSpecifier().reportSpecifierID().c_str();
	RRID = RptItr->reportRequestID();

	ReportWrapper *telemetryStatus;
	ReportWrapper *telemetryUsage;

	ReportName RptNameu("TELEMETRY_USAGE");
	// duration element in Updatereport equals the duration element in Createreport payload
	telemetryUsage = new ReportWrapper(&RptNameu,(ReportBackDuration?Duration:0),DurationModifier::MINUTES, RSID, waitTimeout);

	ReportName RptNames("TELEMETRY_STATUS");
	// duration element in Updatereport equals the duration element in Createreport payload
	telemetryStatus = new ReportWrapper(&RptNames, (ReportBackDuration?Duration:0), DurationModifier::MINUTES, RSID, waitTimeout);


	// To mark the end of reporting (either cancel or end of duration), for this particular report request
	finish = true;

	newGranularity = sdb.sqliteDBreadInt("minsampling", hmiven, "hmiparam");

	//CReating instance and getting values into map from confile
	ReportHandler rh(getVenId());
	rh.getReportconf();
	rh.extractEnumValues();

	waitTimeout = rh.getReportTime(RptItr);// initialise waitTimeout with dtstart, thereafter increment with granularity value.
	Reportdtstart = waitTimeout;

	while(finish)
	{
		//For ReportBackDuration ==0, wait will get datapoints at starttime + granularity.
		if(ReportBackDuration)
			waitTimeout = waitTimeout + ((Granularity?Granularity:newGranularity) * 60);

		log.logger(RRID + "  Waiting for Report dtstart time - " + asctime(localtime(&waitTimeout)), ucmlogging::Info);

		// Wait until waitTimeout or a notify
		if(cv->wait_until(lkreportThread, system_clock::from_time_t(waitTimeout),[](){return (!strcmp(notifierReport,notifierReportVal));}))
		{
			log.logger("Notify Received for Report Thread - "+ RRID +
					   " Stopping Report thread!!!", ucmlogging::Info);
			updateDataFile(RRID);
			finish = false;
		}
		else
		{
			// Wait until timed out
			time_t createdDateTime = time(0);

			log.logger("Wait timed out for report - " + RRID ,ucmlogging::Info);

			// if Reportbackduration = 0,  One time reporting
			if(ReportBackDuration == 0)
			{
				rh.stringIterator = rh.rsid_ReportNameMap.find(RSID);
				size_t found = rh.stringIterator->second.find("TELEMETRY_USAGE");

				// If the RSID of the report request corresponds to a TELEMETRY_USAGE METADATA report
				if(found!=string::npos)
				{
					rh.VstringIterator  = rh.rsid_rid_Map.find(rh.stringIterator->first);

					if(rh.VstringIterator != rh.rsid_rid_Map.end())
					{
						for (rh.vsitr  = rh.VstringIterator->second.begin();rh.vsitr != rh.VstringIterator->second.end(); rh.vsitr++ )
						{
							string rID = *rh.vsitr ;

							// Adding the interval description & data
							telemetryUsage->addInterval(rID, createdDateTime, 0, DurationModifier::MINUTES);

							map<string,vector<string>>::const_iterator VstrIterator;
							VstrIterator = rh.ridPayloadMap.find(rID);
							if(VstrIterator != rh.ridPayloadMap.end())
							{
								extractPayloadValUsage (rID, &payloadValue);
								telemetryUsage->addOadrReportPayloadFloat(VstrIterator->first, payloadValue);
							}

						}
					}
					//Sending update report
					updateReportUsage (venReport, telemetryUsage, Reportdtstart, RRID, createdDateTime);
				}

				found = rh.stringIterator->second.find("TELEMETRY_STATUS");

				// If the RSID of the report request corresponds to a TELEMETRY_STATUS METADATA report
				if(found!=string::npos)
				{
					rh.VstringIterator  = rh.rsid_rid_Map.find(rh.stringIterator->first);
					if(rh.VstringIterator != rh.rsid_rid_Map.end())
					{
						for (rh.vsitr  = rh.VstringIterator->second.begin();rh.vsitr != rh.VstringIterator->second.end(); rh.vsitr++ )
						{
							string rID = *rh.vsitr ;

							// Adding the interval description & data
							telemetryStatus->addInterval(rID, createdDateTime, 0, DurationModifier::MINUTES);

							map<string,vector<string>>::const_iterator VstrIterator;
							vector<string>::const_iterator vitr;
							VstrIterator = rh.ridPayloadMap.find(rID);

							if(VstrIterator != rh.ridPayloadMap.end())
							{
								// Extracting the datapoint values from dplogging.csv file and updating in payload
								extractPayloadValStatus (rID, &CapMax, &CapCurrent, &online, &manual);

								oadr2b::oadr::oadrLoadControlStateType loadControlState;
								loadControlState.oadrCapacity(Oadr2bHelper::generateLoadControlStateType(0.0,CapMax, CapCurrent, 0.0));
								telemetryStatus->addOadrReportPayloadResourceStatus(VstrIterator->first,online, manual, loadControlState);
							}
						}
					}

					//Sending update report
					updateReportStatus (venReport, telemetryStatus, Reportdtstart, RRID, createdDateTime);
				}
				updateDataFile(RRID);
				finish = false;
			}
			else
			{
				// If Reportbackduration != 0 && granularity == 0, infinte reporting
				if(Granularity == 0)
				{
					rh.stringIterator = rh.rsid_ReportNameMap.find(RSID);

					// if the RSID of the report request corresponds to a TELEMETRY_USAGE METADATA report
					size_t found = rh.stringIterator->second.find("TELEMETRY_USAGE");
					if(found!=string::npos)
					{
						rh.VstringIterator  = rh.rsid_rid_Map.find(rh.stringIterator->first);
						if(rh.VstringIterator != rh.rsid_rid_Map.end())
						{
							for (rh.vsitr  = rh.VstringIterator->second.begin();rh.vsitr != rh.VstringIterator->second.end(); rh.vsitr++ )
							{
								string rID = *rh.vsitr ;

								// Adding the interval description & data
								if(addInterval)
									telemetryUsage->addInterval(rID, createdDateTime, ReportBackDuration, DurationModifier::MINUTES);

								map<string,vector<string>>::const_iterator VstrIterator;
								VstrIterator = rh.ridPayloadMap.find(rID);
								if(VstrIterator != rh.ridPayloadMap.end())
								{
									// Extracting the datapoint values from dploggin.csv file and updating in payload
									extractPayloadValUsage (rID, &payloadValue);
									if(firstEntry)
										rh.rid_PayloadvalMap.insert(pair<string, float>(rID,payloadValue));
									else
									{
										Valueupdated = false;
										//compare with old value and update
										rh.floatIterator = rh.rid_PayloadvalMap.find(rID);
										if(rh.floatIterator != rh.rid_PayloadvalMap.end())
										{
											if(rh.floatIterator->second != payloadValue)
											{
												Valueupdated = true;
												rh.floatIterator->second=payloadValue;
											}
										}
									}
									// If values are not changed from the previous reporting interval, set Data quality to No_New_Value___Previous_Value_Used
									if(!(Valueupdated))
										telemetryUsage->addOadrReportPayloadFloat(VstrIterator->first, payloadValue, oadrDataQualityType(oadrDataQualityType::No_New_Value___Previous_Value_Used));
									else
										telemetryUsage->addOadrReportPayloadFloat(VstrIterator->first, payloadValue);
								}
							}
							addInterval = false;
							firstEntry=false;
						}
						usageReport = true;
					}

					// If the RSID of the report request corresponds to a TELEMETRY_STATUS METADATA report
					found = rh.stringIterator->second.find("TELEMETRY_STATUS");
					if(found!=string::npos)
					{
						rh.VstringIterator  = rh.rsid_rid_Map.find(rh.stringIterator->first);
						if(rh.VstringIterator != rh.rsid_rid_Map.end())
						{
							for (rh.vsitr  = rh.VstringIterator->second.begin();rh.vsitr != rh.VstringIterator->second.end(); rh.vsitr++ )
							{
								string rID = *rh.vsitr;
								// Adding the interval description & data
								if(addInterval)
									telemetryStatus->addInterval(rID, createdDateTime, ReportBackDuration, DurationModifier::MINUTES);

								map<string,vector<string>>::const_iterator VstrIterator;
								VstrIterator = rh.ridPayloadMap.find(rID);
								if(VstrIterator != rh.ridPayloadMap.end())
								{
									// Extracting the datapoint values from dploggin.csv file and updating in payload
									extractPayloadValStatus (rID, &CapMax, &CapCurrent, &online, &manual);
									if(firstEntry)
									{
										rh.rid_CapMaxMap.insert(pair<string, float>(rID,CapMax));
										rh.rid_CapCurrentMap.insert(pair<string, float>(rID,CapCurrent));
										rh.rid_onlineMap.insert(pair<string, bool>(rID,online));
										rh.rid_manualMap.insert(pair<string, bool>(rID,manual));
									}
									else
									{
										Valueupdated = false;
										//compare with old value and update
										rh.floatIterator = rh.rid_CapMaxMap.find(rID);
										if(rh.floatIterator != rh.rid_CapMaxMap.end())
										{
											if(rh.floatIterator->second != CapMax)
											{
												Valueupdated = true;
												rh.floatIterator->second=CapMax;
											}
										}

										rh.floatIterator = rh.rid_CapCurrentMap.find(rID);
										if(rh.floatIterator != rh.rid_CapCurrentMap.end())
										{
											if(rh.floatIterator->second != CapCurrent)
											{
												Valueupdated = true;
												rh.floatIterator->second=CapCurrent;
											}
										}

										rh.boolIterator = rh.rid_onlineMap.find(rID);
										if(rh.boolIterator != rh.rid_onlineMap.end())
										{
											if(rh.boolIterator->second != online)
											{
												Valueupdated = true;
												rh.boolIterator->second=online;
											}
										}

										rh.boolIterator = rh.rid_manualMap.find(rID);
										if(rh.boolIterator != rh.rid_manualMap.end())
										{
											if(rh.boolIterator->second != manual)
											{
												Valueupdated = true;
												rh.boolIterator->second=manual;
											}
										}
									}

									oadr2b::oadr::oadrLoadControlStateType loadControlState;
									//// If values are not changed from the previous reporting interval, set Data quality to No_New_Value___Previous_Value_Used
									if( !(Valueupdated))
									{
										loadControlState.oadrCapacity(Oadr2bHelper::generateLoadControlStateType(0.0,CapMax, CapCurrent, 0.0));
										telemetryStatus->addOadrReportPayloadResourceStatus(VstrIterator->first,
																							online,
																							manual,
																							loadControlState,
																							oadrDataQualityType(oadrDataQualityType::No_New_Value___Previous_Value_Used));
									}
									else
									{
										loadControlState.oadrCapacity(Oadr2bHelper::generateLoadControlStateType(0.0,CapMax, CapCurrent, 0.0));
										telemetryStatus->addOadrReportPayloadResourceStatus(VstrIterator->first,online, manual, loadControlState);
									}
								}
							}
							addInterval = false;
							firstEntry=false;
						}
						statusReport = true;
					}
					Rptbktime+= newGranularity;
					if(Rptbktime >= ReportBackDuration)
					{
						//Sending Update Report
						if(usageReport)
						{
							//Sending update report
							updateReportUsage (venReport, telemetryUsage, Reportdtstart, RRID, createdDateTime);
							// duration element in Updatereport equals the duration element in Createreport payload
							telemetryUsage = new ReportWrapper(&RptNameu,ReportBackDuration,DurationModifier::MINUTES, RSID, waitTimeout);
							addInterval = true;
						}
						else if(statusReport)
						{
							//Sending update report
							updateReportStatus (venReport, telemetryStatus, Reportdtstart, RRID, createdDateTime);
							// duration element in Updatereport equals the duration element in Createreport payload
							telemetryStatus = new ReportWrapper(&RptNames, ReportBackDuration, DurationModifier::MINUTES, RSID, waitTimeout);
							addInterval = true;
						}
						Rptbktime = 0;
					}
					if ((waitTimeout>=(Reportdtstart+(Duration * 60.0))) && (Duration != 0))
					{
						updateDataFile(RRID);
						finish = false;
					}
				}
				else //Granularity!=0
				{
					rh.stringIterator = rh.rsid_ReportNameMap.find(RSID);
					// If the RSID of the report request corresponds to a TELEMETRY_USAGE METADATA report
					size_t found = rh.stringIterator->second.find("TELEMETRY_USAGE");
					if(found!=string::npos)
					{
						rh.VstringIterator  = rh.rsid_rid_Map.find(rh.stringIterator->first);
						if(rh.VstringIterator != rh.rsid_rid_Map.end())
						{
							for (rh.vsitr  = rh.VstringIterator->second.begin();rh.vsitr != rh.VstringIterator->second.end(); rh.vsitr++ )
							{
								string rID = *rh.vsitr;
								// Adding the interval description & data
								if(addInterval)
									telemetryUsage->addInterval(rID, createdDateTime, ReportBackDuration, DurationModifier::MINUTES);

								map<string,vector<string>>::const_iterator VstrIterator;
								VstrIterator = rh.ridPayloadMap.find(rID);
								if(VstrIterator != rh.ridPayloadMap.end())
								{
									// Extracting the datapoint values from dploggin.csv file and updating in payload
									extractPayloadValUsage (rID, &payloadValue);
									telemetryUsage->addOadrReportPayloadFloat(VstrIterator->first, payloadValue);
								}
							}
							addInterval = false;
						}
						usageReport = true;
					}
					// If the RSID of the report request corresponds to a TELEMETRY_STATUS METADATA report
					found = rh.stringIterator->second.find("TELEMETRY_STATUS");
					if(found!=string::npos)
					{
						rh.VstringIterator  = rh.rsid_rid_Map.find(rh.stringIterator->first);
						if(rh.VstringIterator != rh.rsid_rid_Map.end())
						{
							for (rh.vsitr  = rh.VstringIterator->second.begin();rh.vsitr != rh.VstringIterator->second.end(); rh.vsitr++ )
							{
								string rID = *rh.vsitr;
								// Adding the interval description & data
								if(addInterval)
									telemetryStatus->addInterval(rID, createdDateTime, ReportBackDuration, DurationModifier::MINUTES);

								map<string,vector<string>>::const_iterator VstrIterator;
								VstrIterator = rh.ridPayloadMap.find(rID);
								if(VstrIterator != rh.ridPayloadMap.end())
								{
									// Extracting the datapoint values from dploggin.csv file and updating in payload
									extractPayloadValStatus (rID, &CapMax, &CapCurrent, &online, &manual);

									oadr2b::oadr::oadrLoadControlStateType loadControlState;
									loadControlState.oadrCapacity(Oadr2bHelper::generateLoadControlStateType(0.0,CapMax, CapCurrent, 0.0));
									telemetryStatus->addOadrReportPayloadResourceStatus(VstrIterator->first, online, manual, loadControlState);
								}
							}
							addInterval = false;
						}
						statusReport = true;
					}//end of if status
					Rptbktime+=Granularity;
					if(Rptbktime >= ReportBackDuration)
					{
						if(usageReport)
						{
							//Sending update report
							updateReportUsage (venReport, telemetryUsage, Reportdtstart, RRID, createdDateTime);
							// duration element in Updatereport equals the duration element in Createreport payload
							telemetryUsage = new ReportWrapper(&RptNameu,ReportBackDuration,DurationModifier::MINUTES, RSID, waitTimeout);
							addInterval = true;
						}
						else if(statusReport)
						{
							//Sending update report
							updateReportStatus (venReport, telemetryStatus, Reportdtstart, RRID, createdDateTime);
							// duration element in Updatereport equals the duration element in Createreport payload
							telemetryStatus = new ReportWrapper(&RptNames, ReportBackDuration, DurationModifier::MINUTES, RSID, waitTimeout);
							addInterval = true;
						}
						Rptbktime=0;
					}
					if ((waitTimeout>=(Reportdtstart+(Duration * 60.0))) && (Duration != 0))
					{
						updateDataFile(RRID);
						finish = false;
					}
				}
			}
		}
	}
	log.logger("-------Exit from Report Thread - " + RRID, ucmlogging::Info);
}



/**
   Method to Validate oadrCreateReport payload
   @param RptItr - Single Report Payload
 * @return Response Code
 */

int ReportHandler::ValidateReport(oadrCreateReportType::oadrReportRequest_iterator RptItr)
{
	bool status = true;
	bool ifRSIDFound = false;
	bool ifRIDsupported = false;
	bool rptinDB = false;
	string responsemsg = "OK";
	ucmlogging log;
	ifstream infile;
	responseCode = 200;
	responseDescription = "OK";
	time_t currentReport = getReportTime(RptItr);

	//Checking if reportRequestid is null
	if(!(strcmp(RptItr->reportRequestID().c_str(), "")))
	{
		responseCode = 452;
		responseDescription = "Invalid ID";
		responsemsg = "Report Request ID is Invalid";
		status = false;
	}

	//Checking if reportRequestid is already present in reportrequests.data file
	//TODO: To be decided
	stringIterator = rrid_rsidMap.find(RptItr->reportRequestID());
	if(stringIterator != rrid_rsidMap.end())
	{
		responseCode = 451;
		responseDescription = "Not allowed";
		responsemsg = "Report exist in database";
		rptinDB = true;
		status = false;
	}

	//Checking if Reportdtstart of the new report request is less than current time
	if((!(reportDateTimeCheck(time(0), currentReport))) && status)
	{
		responseCode = 451;
		responseDescription = "Not allowed";
		responsemsg = "Report start time in past";
		status = false;
	}

	//Checking if ReportSpecifID is NULL
	if(!(strcmp(RptItr->reportSpecifier().reportSpecifierID().c_str(), "")) && status)
	{
		responseCode = 452;
		responseDescription = "Invalid ID";
		responsemsg = "Report Specifier ID is NULL";
		status = false;
	}

	//Checking if ReportSpecifrID is present in the reports.conf file -if present RSID is supported
	//ignore for the case of reportSpecifierID = METADATA
	if(strcmp(RptItr->reportSpecifier().reportSpecifierID().c_str(), "METADATA"))
	{
		stringIterator = rsid_ReportNameMap.find(RptItr->reportSpecifier().reportSpecifierID());
		if(stringIterator != rsid_ReportNameMap.end())
			ifRSIDFound = true;

		if(!ifRSIDFound && status)
		{
			responseCode = 454;
			responseDescription = "Invalid Data";
			responsemsg = "Report Specifier ID not supported";
			status = false;
		}
	}

	//Checking if rid is NULL
	ReportSpecifierType *specifierP = &RptItr->reportSpecifier();
	ReportSpecifierType::specifierPayload_iterator PayloadItr;
	for(PayloadItr = specifierP->specifierPayload().begin(); PayloadItr != specifierP->specifierPayload().end(); PayloadItr++)
	{
			if(!(strcmp(PayloadItr->rID().c_str(), "")) && status)
			{
				responseCode = 452;
				responseDescription = "Invalid ID";
				responsemsg = "RID is NULL";
				status = false;
			}
	}

	//Checking if rid is supported by RSID
	//ignore for the case of reportSpecifierID = METADATA
	if(strcmp(RptItr->reportSpecifier().reportSpecifierID().c_str(), "METADATA"))
	{
		for(PayloadItr = specifierP->specifierPayload().begin(); PayloadItr != specifierP->specifierPayload().end(); PayloadItr++)
		{
			VstringIterator = rsid_rid_Map.find(RptItr->reportSpecifier().reportSpecifierID());
			if(VstringIterator != rsid_rid_Map.end())
			{
				for (vsitr  = VstringIterator->second.begin();vsitr != VstringIterator->second.end(); vsitr++ )
				{
						string value = *vsitr ;
						if(!(strcmp(value.c_str(),PayloadItr->rID().c_str())))
							ifRIDsupported = true;
				}
			}

			if(!ifRIDsupported && status)
			{
				responseCode = 454;
				responseDescription = "Invalid Data";
				responsemsg = "RID not supported";
				status = false;
			}
		}
	}
	//Checking if Granularity, Duration, ReportbackDuration parameters are present or not for NonMetadata reports
	if(strcmp(RptItr->reportSpecifier().reportSpecifierID().c_str(), "METADATA"))
	{
		if(((!strcmp(RptItr->reportSpecifier().granularity().duration().c_str(), "")) ||
		   (!strcmp(RptItr->reportSpecifier().reportBackDuration().duration().c_str(), "")) ||
		   (!strcmp(RptItr->reportSpecifier().reportInterval().get().properties().duration().duration().c_str(), ""))) &&
		   status)
		{
			responseCode = 454;
			responseDescription = "Invalid Data";
			responsemsg = "Report Duration values missing";
			status = false;
		}
	}

	stringstream temp;
	temp<<responseCode;
	log.logger("======================================================", ucmlogging::Info);
	log.logger("Report Request ID- " +  RptItr->reportRequestID(), ucmlogging::Info);
	log.logger("Report Specifier ID- " +  RptItr->reportSpecifier().reportSpecifierID(), ucmlogging::Info);
	log.logger("Granularity - " + (RptItr->reportSpecifier().granularity().duration()), ucmlogging::Info);
	log.logger("ReportBack Duration - " + (RptItr->reportSpecifier().reportBackDuration().duration()), ucmlogging::Info);
	log.logger("Duration - " + (RptItr->reportSpecifier().reportInterval().get().properties().duration().duration()), ucmlogging::Info);
	log.logger("Start Time - " + string(asctime(localtime(&currentReport))), ucmlogging::Info);
	log.logger("Valid - " + string(((status) ? "Yes" : "No")), ucmlogging::Info);
	log.logger("Response Code - " + temp.str(), ucmlogging::Info);
	log.logger("Response Description - " + responsemsg, ucmlogging::Info);
	log.logger("======================================================", ucmlogging::Info);

	if(!status)
	{
		if(!rptinDB)
		{
			string removeFile;
			removeFile = string(XMLFilePath)+ string(ReportXMLfolder)+ RptItr->reportRequestID()+"_oadrCreateReport.xml";
			remove(removeFile.c_str());
		}
		log.logInvalidEventReport(RptItr->reportRequestID(), responsemsg);
	}

	return responseCode;
}

/**
   Method to Validate report and spawn report threads
   @param pointer to class VEN2b
   @param oadrCreateReportType - Report Payload
   @param marketContext - marketContext from main
   @param Condition Variable - To Notify
   @param Noftifier Pointer - To Notify
   @return Status
 */

//TODO: Report which is present in db but missing from payload
//TODO: Report which is present in db but values modified in next payload
bool ReportHandler::reportScheduler(VEN2b *venReport, oadrCreateReportType CreateReportPayload, condition_variable cvRpt[], char *notifierP)
{
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Report Scheduler Started!!! Verifying Payload!!!" ,ucmlogging::Info);

	vector<string> pendingReports;
	vector<thread> reportThread;

	unsigned int cnt;
	bool exitstatus, innerexitstatus;
	if(CreateReportPayload.oadrReportRequest().size())
	{
		//Updating map from reportrequests.data file, Initially reportrequests.data file will be empty
		getReportdata();

		//Extracting from reports.conf file
		getReportconf();

		oadrCreateReportType::oadrReportRequest_iterator RptItr;
		for(RptItr = CreateReportPayload.oadrReportRequest().begin(); RptItr!=CreateReportPayload.oadrReportRequest().end();RptItr++)
		{
			time_t currentReport = getReportTime(RptItr);
			//Validating reportrequest payload
			int ErrorCode = ValidateReport(RptItr);
			if(ErrorCode == 200)
			{
				pendingReports.push_back(RptItr->reportRequestID());
				exitstatus = true;
				for(cnt=0;cnt<maxreport && exitstatus;cnt++)
				{
					innerexitstatus = true;
					
					//To identify the first unused cv_id for spawning a new report thread
					for (intIterator=rrid_cvMap.begin(); intIterator!=rrid_cvMap.end(); ++intIterator)
					{
						if(intIterator->second == cnt)
						{
							innerexitstatus = false;
							break;
						}
					}

					if(innerexitstatus)
					{
						// If RSID is "METADATA", spawn metadataThread thread
						if (!(RptItr->reportSpecifier().reportSpecifierID().compare("METADATA")))
						{
							insertDB(RptItr->reportRequestID(), RptItr->reportSpecifier().reportSpecifierID(),
									 "METADATA", RptItr->reportSpecifier().granularity().duration(),
									 RptItr->reportSpecifier().reportBackDuration().duration(),
									 currentReport,
									 RptItr->reportSpecifier().reportInterval().get().properties().duration().duration(), cnt);

							log.logger("spawning metadata thread" , ucmlogging::Info);
							ReportHandler RH(getVenId());
							reportThread.push_back(thread(&ReportHandler::metadataThread, &RH, venReport, RptItr, cvRpt, &cvRpt[cnt], notifierP));
						}
						else // if RSID correponds to a TELEMETRY report, spawn reportThread thread
						{
							insertDB(RptItr->reportRequestID(), RptItr->reportSpecifier().reportSpecifierID(),
									 "TELEMETRY", RptItr->reportSpecifier().granularity().duration(),
									 RptItr->reportSpecifier().reportBackDuration().duration(),
									 currentReport,
									 RptItr->reportSpecifier().reportInterval().get().properties().duration().duration(), cnt);

							ReportHandler RH(getVenId());
							reportThread.push_back(thread(&ReportHandler::reportThread, &RH, venReport, RptItr, &cvRpt[cnt], notifierP));
						}
						exitstatus = false;
					}
				}
			}
			sleep(1);
		}
		for(auto& thread : reportThread)
			thread.detach();


		sdb.sqliteDBwriteInt("reportthread",(rrid_cvMap.size()), thread_status, "threadstatus");
	}
	//Sending oadrCreatedReport to VTN
	try
	{
		log.logger("Sending created report request" , ucmlogging::Info);
		auto_ptr<CreatedReport> createRpt = venReport->createdreport(getVenId(), &pendingReports, "200", "OK", CreateReportPayload.requestID());
		xmlFileCreation requestLog(string(ReportXMLfolder)+"oadrCreatedReport.xml", createRpt->requestBody());

		xmlFileCreation responseLog(string(ReportXMLfolder)+"oadrResponse.xml", createRpt->responseBody());
		oadrPayload *payload = createRpt->response();

		if (payload == NULL || !payload->oadrSignedObject().oadrResponse().present())
		{
			log.logger("Received unexpected payload", ucmlogging::Error);
		}

		else if (payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode(), ucmlogging::Error);
		}
	}
	catch (CurlException& ex)
	{
		log.logger("caught exception: " + string(ex.what()) + "!!!Failed to send createdReport!!!",ucmlogging::Error);
	}

	//Dump map values (existing pending reports data + new reports data) to reportrequests.db file

	remove(reportdetails);
	for (stringIterator=rrid_rsidMap.begin(); stringIterator!=rrid_rsidMap.end(); ++stringIterator)
	{
		map<string,string>::iterator reporttypeIterator;
		map<string,string>::iterator granularityIterator;
		map<string,string>::iterator rptbckIterator;
		map<string,string>::iterator durationIterator;

		timeIterator = rrid_startMap.find(stringIterator->first);
		intIterator = rrid_cvMap.find(stringIterator->first);
		reporttypeIterator = rrid_reportTypeMap.find(stringIterator->first);
		granularityIterator = rrid_granMap.find(stringIterator->first);
		rptbckIterator = rrid_rbdurationMap.find(stringIterator->first);
		durationIterator = rrid_durMap.find(stringIterator->first);

		sdb.insertDBreportdetails(stringIterator->first,
								  stringIterator->second,
								  reporttypeIterator->second,
								  granularityIterator->second,
								  rptbckIterator->second,
								  timeIterator->second,
								  durationIterator->second,
								  intIterator->second);
	}
	//clear DB
	clearDB();
	return true;

}

/**
   Method to cancel reports
   @param pointer to class VEN2b
   @param oadrCreateReportType - Report Payload
   @param Condition Variable - To Notify
   @param Notfifier Pointer - To Notify
   @return Status
 */
bool ReportHandler::cancelReport(VEN2b *venCancelReport, oadrCancelReportType CancelRptPayload,condition_variable cvRpt[], char *notifierP)
{
	oadrCancelReportType::reportRequestID_iterator ReportRequestIDItr;
	vector<string> pendingReports;
	ucmlogging log;

	bool reportrequestidfound = false;
	getReportdata();

	for(ReportRequestIDItr = CancelRptPayload.reportRequestID().begin(); ReportRequestIDItr != CancelRptPayload.reportRequestID().end(); ReportRequestIDItr++)
	{
		intIterator = rrid_cvMap.find(ReportRequestIDItr->data());
		if(intIterator != rrid_cvMap.end())
		{
			reportrequestidfound = true;
			reportWaitNotifier(intIterator->second, cvRpt, notifierP);

			updateDataFile(ReportRequestIDItr->data());
		}
	}
	//clear DB
	clearDB();
	//Updating maps from reportrequests.data file, Initially reportrequests.data file will be empty
	getReportdata();
	for (intIterator=rrid_cvMap.begin(); intIterator!=rrid_cvMap.end(); ++intIterator)
		pendingReports.push_back(intIterator->first);

	if(reportrequestidfound)
		responseCode = 200;
	else
		responseCode = 452;
	//send oadrcancelledreport to VTN, for the report requests (specified by report request id) to be cancelled
	try
	{
		stringstream temp;
		temp<<responseCode;
		auto_ptr<CanceledReport> canceledRpt = venCancelReport->canceledreport(getVenId(),temp.str(), "OK", CancelRptPayload.requestID(), &pendingReports);

		xmlFileCreation requestLog(string(ReportXMLfolder)+"oadrCanceledReport.xml", canceledRpt->requestBody());
		xmlFileCreation responseLog(string(ReportXMLfolder)+"oadrResponse.xml", canceledRpt->responseBody());

		oadrPayload *payload = canceledRpt->response();

		if (payload == NULL || !payload->oadrSignedObject().oadrResponse().present())
		{
			log.logger("Received unexpected payload", ucmlogging::Error);
			return false;
		}

		else if (payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode(), ucmlogging::Error);
			return false;
		}
	}
	catch (CurlException& ex)
	{
		log.logger("caught exception: " + string(ex.what()) + "!!!Failed to send canceledReport!!!",ucmlogging::Error);
		return false;
	}

	return true;

}


