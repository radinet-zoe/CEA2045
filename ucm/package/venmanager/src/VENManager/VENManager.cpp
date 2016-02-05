/*
 * VENManager.cpp
 *
 *  Created on: 08-Dec-2014
 *      Author: dinesh
 */

#include "VENManager.h"

extern "C"
{
	#include "CEA2045/cea2045lib.h"
}


char *notifier = new char[20];
condition_variable cv_poll;
condition_variable cv_event;
condition_variable cv_log;
condition_variable cv_signal[maxsignal];
condition_variable cv_report[maxreport];


/**
    VENManager Constructor
    @param baseURL - VTN base URL
    @param venName - VEN Name from user
    @param venID - VEN ID returned while registration
    @param registrationID - Registration ID returned while registration
    @param vtnID - VTN ID returned while registration
    @param marketcontext - market contexts from user
 */
VENManager::VENManager(string baseURL, string venName, string venID, string registrationID, string vtnID) :
	m_baseURL(baseURL),
	m_venName(venName),
	m_venID(venID),
	m_registrationID(registrationID),
	m_vtnID(vtnID)
{

}

VENManager::~VENManager()
{

}


/**
   Method to check for registration status
   @return - status
 */
bool VENManager::ifregistered()
{
	if (m_venID == "" || m_registrationID == "")
		return false;

	return true;
}

/**
   Method to check whether VTN URL and VEN name exists
   @returns - status
 */
bool VENManager::ifurlandNameExist()
{
	if (m_baseURL == "" || m_venName == "")
		return false;

	return true;
}


/**
   Method to initialise sqlite database
   @return - nothing
 */
void VENManager::sqliteInitialise(void)
{
	sqliteDB sdb;
	ucmlogging log;
	sdb.sqliteDBwriteInt("pollfreq",10, hmiven, "hmiparam");
	sdb.sqliteDBwriteInt("minsampling",5, hmiven, "hmiparam");
	sdb.sqliteDBwriteInt("maxsampling",5, hmiven, "hmiparam");
	sdb.sqliteDBwriteInt("customeroverride",0, hmiven, "hmiparam");
	sdb.sqliteDBwriteInt("pollthread",0, thread_status, "threadstatus");
	sdb.sqliteDBwriteInt("eventthread",0, thread_status, "threadstatus");
	sdb.sqliteDBwriteInt("signalthread",0, thread_status, "threadstatus");
	sdb.sqliteDBwriteInt("logthread",0, thread_status, "threadstatus");
	sdb.sqliteDBwriteInt("reportthread",0, thread_status, "threadstatus");
	sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
	sdb.sqliteDBwriteString("errormsg","", hmiven, "hmivenflag");
	sdb.sqliteDBwriteString("commport","", hmiven, "cea2045config");

	changePermission(ven_param);
	changePermission(thread_status);
	changePermission(hmiven);
	changePermission(optschedule);
	changePermission(dplogfile);
}

/**
   Method to set permission of a folder
   @returns - nothing
 */

void VENManager::changePermission(string path)
{
	struct stat st = {0};
	if (!(stat(path.c_str(), &st) == -1))
	{
		string chmod;
		chmod = "chmod 0777 " +path;
		system(chmod.c_str());
		if(access(path.c_str(),W_OK) != 0)
		{
			cout<<path<<" Permission setting failed. Process Ending!!!"<<endl;
			cout<<"Try with root Permission!!!"<<endl;
			exit(1);
		}
	}
}


/**
   Method to create a folder
   @return - nothing
 */

void VENManager::createFolder(string path)
{
	struct stat st = {0};
	if ((stat(path.c_str(), &st) == -1))
	{
		string mkdir;
		mkdir = "mkdir -m 777 " +path;
		system(mkdir.c_str());
		if((stat(path.c_str(), &st)) == -1)
		{
			cout<<"Folder structure creation failed for "<<path<<". Process Ending!!!"<<endl;
			cout<<"Try with root Permission!!!"<<endl;
			exit(1);
		}
		changePermission(path);
	}
}

/**
   Method to remove a folder
   @return - nothing
 */

void VENManager::removeFolder(string path)
{
	struct stat st = {0};
	if (!(stat(path.c_str(), &st) == -1))
	{
		string rmdir;
		rmdir = "rm -rf "+path;
		if(system(rmdir.c_str()) == -1)
		{
			cout<<"Folder removal failed. Process Ending"<<endl;
			exit(1);
		}
	}
}

/**
   Method to create Folder Structure (data,log and conf) and the conf files (reports.conf,signals.conf,dplogging.conf)
   @return - Status
 */

bool VENManager::createFolderStructure(void)
{
	struct stat st = {0};
	confGeneration conf;

	createFolder(home_path);
	createFolder(conf_path);

	conf.generateSignalConf();
	conf.generateReportConf();

	removeFolder(Logpath);

	createFolder(Logpath);
	createFolder(data_path);
	createFolder(utils_path);
	createFolder(ftpsupdate);
	createFolder(XMLFilePath);
	createFolder(string(XMLFilePath)+string(ReportXMLfolder));
	createFolder(string(XMLFilePath)+string(RegReportXMLfolder));

	remove(eventdetails);
	remove(reportdetails);
	remove(signaldetails);
	remove(ven_param);
	remove(thread_status);

	//check if conf file exist
	if (stat(signalsConfFile, &st) == -1)
		return false;
	if (stat(reportConffile, &st) == -1)
		return false;
	if (stat(dpconffile, &st) == -1)
		return false;

	return true;
}
/**
   Method to update VEN Parameters from db file, on startup
   @return - nothing
 */
void VENManager::updateParameter()
{
	sqliteDB sdb;
	setVenId(sdb.sqliteDBreadString("venid", ven_param, "venparam"));
	setRegistrationId(sdb.sqliteDBreadString("registrationid", ven_param, "venparam"));
	setVtnId(sdb.sqliteDBreadString("vtnid", ven_param, "venparam"));
	setVenName(sdb.sqliteDBreadString("venname", hmiven, "hmiparam"));
	setBaseUrl(sdb.sqliteDBreadString("vtnurl", hmiven, "hmiparam"));
}


void VENManager::startDataLogThread(VEN2b *venObject,int restart)
{

	ucmlogging log;
	if(!restart)
	{
		//Start customer_overide/sleep/wake check
		log.logger("Starting customer_overide/sleep/wake check!!!", ucmlogging::Info);
		thread checkCustOverrideThread(checkCustOverride);
		checkCustOverrideThread.detach();
		//Start SGD Write thread
		//log.logger("Starting SGD write thread!!!", ucmlogging::Info);
		//thread writetoSGDthread(writetoSGD);
		//writetoSGDthread.detach();
		//thread to check customer override and notify signals
		CustomerOverride co;
		thread CustOverrideThread(&CustomerOverride::getOverrideStatus, co, venObject);
		CustOverrideThread.detach();
	}

	//Start Report Logging thread
	log.logger("Starting Log Thread", ucmlogging::Info);
	logThread lt(1);
	thread loggingThread(&logThread::startLog, lt, &cv_log, notifier);
	loggingThread.detach();

}


void VENManager::registerVEN(VEN2b *ven)
{
	ucmlogging log;
	sqliteDB sdb;
	// Check if VTN URL and VEN name exists (To handle UCM resets)
	if(ifurlandNameExist())
	{
		//If UCM was in registered state, do a re-registration with VEN id and Registration id
		if(ifregistered())
		{
			log.logger("Registration Status - Registered", ucmlogging::Info);
			registerParty rh(getVenId(), getRegistrationId());
			if(!rh.RegisterParty(ven))
				log.logger("Re-Registration Failed!!!", ucmlogging::Error);
		}
		//If UCM was not in registered state, do a fresh registration with VTN url and VEN name
		else
		{
			log.logger("Registration Status - Not registered - Registering!!!", ucmlogging::Info);
			registerParty rh;
			if(!rh.RegisterParty(ven))
				log.logger("Register Party Failed!!!", ucmlogging::Error);
			setVenId(sdb.sqliteDBreadString("venid", ven_param, "venparam"));
			setRegistrationId(sdb.sqliteDBreadString("registrationid", ven_param, "venparam"));
			setVtnId(sdb.sqliteDBreadString("vtnid", ven_param, "venparam"));

			log.logger("Sending Register Report!!!", ucmlogging::Info);
			reportRegister rr(getVenId());
			if(rr.ReportRegister(ven))
				log.logger("Report(s) Successfully registered!!!", ucmlogging::Info);
			else
				log.logger("Report(s) Registration Failed!!!", ucmlogging::Error);
		}
	}
	else
	{
		log.logger("VTN URL and VEN Name Not found!!!", ucmlogging::Error);
		sdb.sqliteDBwriteString("errormsg","", hmiven, "hmivenflag");
	}


}


/**
   Method to send oadrCreatedEvent message to VEN
   @param - pointer to VEN2b
   @param - EventResponses
   @return - status
 */
bool VENManager::sendCreatedEvent(VEN2b *venObject, EventResponses er)
{
	ucmlogging log;
	sqliteDB sdb;
	bool status;
	try
	{
		log.logger("Sending oadrCreatedEvent message" ,ucmlogging::Info);
		auto_ptr<CreatedEvent> ce = venObject->createdEvent("200", "OK", er);

		xmlFileCreation requestLog("oadrCreatedEvent.xml", ce->requestBody());
		oadrPayload *payload = ce->response();

		if (payload == NULL || !payload->oadrSignedObject().oadrResponse().present())
		{
			log.logger("Received unexpected payload" , ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Payload from server!!!oadrCreatedEvent Transaction Failed!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("oadrResponse.xml", ce->responseBody());
			return false;

		}
		else if (payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode().compare("200") != 0)
		{
			log.logger("Received unexpected response code: " + payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode(), ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Received unexpected Response code from server: " + payload->oadrSignedObject().oadrResponse()->eiResponse().responseCode() + "oadrCreatedEvent Failed!!!", hmiven, "hmivenflag");
			xmlFileCreation responseLog("oadrResponse.xml", ce->responseBody());
			return false;
		}
		else
		{
			status = true;
			xmlFileCreation responseLog("oadrResponse.xml", ce->responseBody());
			log.logger("oadrCreatedEvent transaction is successful!!!" ,ucmlogging::Info);
		}
	}
	catch (CurlException& ex)
	{
		log.logger("Caught exception: " + string(ex.what()) + "!!!Failed to send oadrCreatedEvent!!!",ucmlogging::Error);
		sdb.sqliteDBwriteString("errormsg",string(ex.what())+ "!!!Failed to send oadrCreatedEvent!!!", hmiven, "hmivenflag");
		return false;
	}
	return status;
}
/**
   Method to update the Event, when opted out from HMI
   @param - pointer to VEN2b
   @return - status
 */
bool VENManager::updateEventsOptout(VEN2b *venObject)
{
	EventHandler eh;
	sqliteDB sdb;
	ucmlogging log;
	EventResponses er;
	unsigned int duration=0;
	int customerOverride;
	time_t currentTime = time(0);
	bool sendCreatedEventReq = false;

	customerOverride = sdb.sqliteDBreadInt("customeroverride", hmiven, "hmiparam");
	eh.getdata();

	for (eh.timeIterator=eh.eventiddtstartmap.begin(); eh.timeIterator!=eh.eventiddtstartmap.end(); ++eh.timeIterator)
	{
		bool optout = false;
		eh.uintIterator = eh.eventiddurationmap.find(eh.timeIterator->first);
		duration = eh.uintIterator->second;
		eh.intIterator = eh.eventidmodmap.find(eh.timeIterator->first);

		eh.stringIterator = eh.eventidoptstatusmap.find(eh.timeIterator->first);
		if(eh.stringIterator != eh.eventidoptstatusmap.end())
		{
			if(eh.stringIterator->second.compare("opt-out_sch")==0)
				optout = false;
			else
				optout = true;
		}

		optout &= eh.optOutStatus(eh.timeIterator->second, duration);

		eh.stringIterator = eh.eventidreqidmap.find(eh.timeIterator->first);

		if(optout)
		{
			if(eh.eventActiveSignalCheck(currentTime, eh.timeIterator->second, duration))
			{
				if((customerOverride!=0))
				{
					//Create oadrCreatedEvent payload and update the eventdetails.db with the optid and the new opt status
					if(eh.stringIterator->second.compare("")!=0)
					{
						er.addEventResponse("200", "OK", eh.timeIterator->first, eh.intIterator->second, OptTypeType::optOut, eh.stringIterator->second);
						sendCreatedEventReq = true;
					}
				}
				else
					log.logger("Event is active and Customer override is disabled!!! Cannot opt-out the Event - " +eh.timeIterator->first, ucmlogging::Info);
			}
			else
			{
				//Create oadrCreatedEvent payload and update the eventdetails.db with the optid and the new opt status
				if(eh.stringIterator->second.compare("")!=0)
				{
					er.addEventResponse("200", "OK", eh.timeIterator->first, eh.intIterator->second, OptTypeType::optOut, eh.stringIterator->second);
					sendCreatedEventReq = true;
				}
			}
		}
	}
	bool success = false;
	if(sendCreatedEventReq)
		success = sendCreatedEvent(venObject,er);
	else
		success = true;

	if(!success)
		return false;

	for (eh.timeIterator=eh.eventiddtstartmap.begin(); eh.timeIterator!=eh.eventiddtstartmap.end(); ++eh.timeIterator)
	{
		bool optout = false;
		eh.uintIterator = eh.eventiddurationmap.find(eh.timeIterator->first);
		duration = eh.uintIterator->second;

		eh.stringIterator = eh.eventidoptstatusmap.find(eh.timeIterator->first);
		if(eh.stringIterator != eh.eventidoptstatusmap.end())
		{
			if(eh.stringIterator->second.compare("opt-out_sch")==0)
				optout = false;
			else
				optout = true;
		}
		optout &= eh.optOutStatus(eh.timeIterator->second, duration);
		if(optout)
		{
			if(eh.eventActiveSignalCheck(currentTime, eh.timeIterator->second, duration))
			{
				if((customerOverride!=0))
				{
					waitNotifier("SIGNAL");
					if((getThreadStatus("SIGNAL")))
					{
						sdb.sqliteDBwriteString("errormsg","Failed to stop signal thread(s). Signal thread(s) still running!!!", hmiven, "hmivenflag");
						log.logger("Failed to stop Signal thread(s). Signal thread(s) still running!!! opt-out of Event failed!!!", ucmlogging::Error);
						return false;
					}

					if(!(sdb.sqliteDBdeletestring("eventdetails", eventdetails, "eventid", eh.timeIterator->first)))
					{
						sdb.sqliteDBwriteString("errormsg","Failed to delete "+eh.timeIterator->first+" from eventdetails.db!!!", hmiven, "hmivenflag");
						log.logger("eventdetails.db delete failed for the Event - " + eh.timeIterator->first, ucmlogging::Error);
						return false;
					}
				}
			}
			else
			{
				if(!(sdb.sqliteDBwriteStringWithCond("optstatus", "opt-out_sch", eventdetails, "eventdetails","eventid",eh.timeIterator->first)))
				{
					log.logger("Opt state update failed for the eventID - " +eh.timeIterator->first, ucmlogging::Error);
					sdb.sqliteDBwriteString("errormsg","Failed to update eventdetails.db with the opt state!!!", hmiven, "hmivenflag");
					return false;
				}
				if(!(sdb.sqliteDBwriteStringWithCond("optid", eh.getOptId(), eventdetails, "eventdetails","eventid",eh.timeIterator->first)))
				{
					log.logger("optID update failed for the eventID - " +eh.timeIterator->first, ucmlogging::Error);
					sdb.sqliteDBwriteString("errormsg","Failed to update eventdetails.db with the optID!!!", hmiven, "hmivenflag");
					return false;
				}
			}
		}
	}
	return true;
}

/**
   Method to update the Event, when opted in from HMI
   @param - pointer to VEN2b
   @return - status
 */
bool VENManager::updateEventsOptin(VEN2b *venObject, string optID)
{
	EventHandler eh;
	EventResponses er;
	ucmlogging log;
	sqliteDB sdb;
	bool sendCreatedEventReq = false;
	bool retStatus = false;
	int customerOverride;
	unsigned int duration=0;
	time_t currentTime = time(0);

	map<string,string>::iterator sIterator;
	map<string,int>::iterator modnoIterator;
	map<string,string>::iterator reqidIterator;

	customerOverride = sdb.sqliteDBreadInt("customeroverride", hmiven, "hmiparam");
	eh.getdata();
	for (sIterator=eh.eventidoptidmap.begin(); sIterator!=eh.eventidoptidmap.end(); ++sIterator)
	{
		if(sIterator->second.compare(optID) == 0)
		{
			reqidIterator = eh.eventidreqidmap.find(sIterator->first);
			modnoIterator = eh.eventidmodmap.find(sIterator->first);
			eh.uintIterator = eh.eventiddurationmap.find(sIterator->first);
			duration = eh.uintIterator->second;
			eh.timeIterator = eh.eventiddtstartmap.find(sIterator->first);
			if(eh.eventActiveSignalCheck(currentTime, eh.timeIterator->second, duration))
			{
				if((customerOverride!=0))
				{
					//Create oadrCreatedEvent payload and update the eventdetails.db with the optid and the new opt status
					if(reqidIterator->second.compare("")!=0)
					{
						er.addEventResponse("200", "OK", sIterator->first, modnoIterator->second, OptTypeType::optIn, reqidIterator->second);
						sendCreatedEventReq = true;
					}
				}
				else
					log.logger("Event is active and Customer override is disabled!!! Cannot opt-in the Event - " +eh.timeIterator->first, ucmlogging::Info);
			}
			else
			{
				if(reqidIterator->second.compare("")!=0)
				{
					er.addEventResponse("200", "OK", sIterator->first, modnoIterator->second, OptTypeType::optIn, reqidIterator->second);
					sendCreatedEventReq = true;
				}
			}
		}
	}
	if(sendCreatedEventReq)
		retStatus = sendCreatedEvent(venObject,er);
	else
		retStatus = true;

	return retStatus;
}


/**
   Method to check for any running threads
   This will return the number of threads for any service

   @param - Thread name
   @return - status
 */
int VENManager::getThreadStatus(string threadName)
{
	int threadStatus = 0;
	sqliteDB sdb;
	if(!strcmp(threadName.c_str(),"EVENT"))
		threadStatus = sdb.sqliteDBreadInt("eventthread", thread_status, "threadstatus");
	if(!strcmp(threadName.c_str(),"SIGNAL"))
		threadStatus = sdb.sqliteDBreadInt("signalthread", thread_status, "threadstatus");
	if(!strcmp(threadName.c_str(),"POLL"))
		threadStatus = sdb.sqliteDBreadInt("pollthread", thread_status, "threadstatus");
	if(!strcmp(threadName.c_str(),"REPORT"))
		threadStatus = sdb.sqliteDBreadInt("reportthread", thread_status, "threadstatus");
	if(!strcmp(threadName.c_str(),"LOG"))
		threadStatus = sdb.sqliteDBreadInt("logthread", thread_status, "threadstatus");

	return threadStatus;
}

/**
   Method to notify threads
   The notifierString will be :
   Events - "EVENT"
   Reports - "REPORT"
   Signals - "SIGNAL"
   Polling - "POLL"
   log - "LOG"

   @param - notifierString
   @return - nothing
 */
void VENManager::waitNotifier(string notifierString)
{
	ucmlogging log;
	unsigned int count = 3;
	int threadVal;
	ReportHandler rh(getVenId());
	EventHandler eh;

	//if thread is not running, the waitNotifier should not send notify.
	threadVal = getThreadStatus(notifierString);
    if(threadVal)
    {
    	// if notifierString is "REPORT"
    	if(!strcmp(notifierString.c_str(),"REPORT"))
    	{
    		rh.getReportdata();
    		map<string, unsigned int>::iterator iItr;
    		iItr = rh.rrid_cvMap.begin();
    		while(threadVal-- && iItr != rh.rrid_cvMap.end())
    		{
    			log.logger("Notifying " + notifierString, ucmlogging::Info);
    			strcpy(notifier,notifierString.c_str());
    			cv_report[iItr->second].notify_all();
    			iItr++;
    			sleep(1);
    		}

    	}
    	// if notifierString is "SIGNAL"
    	else if(!strcmp(notifierString.c_str(),"SIGNAL"))
    	{
    		eh.getsignalindexmap();
    		map<string, unsigned int>::iterator iItr;
    		iItr = eh.signalcvindexmap.begin();
    		while(threadVal-- && iItr != eh.signalcvindexmap.end())
    		{
				log.logger("Notifying " + notifierString, ucmlogging::Info);
				strcpy(notifier,notifierString.c_str());
				cv_signal[iItr->second].notify_all();
				iItr++;
				sleep(1);
    		}
    		if(oadr_cea_EventMap("END_SHED", NULL, NULL, 0))
				log.logger("Event Cancellation request!! END SHED sent to SGD!!", ucmlogging::Info);
    		else
				log.logger("Event Cancellation request!! END SHED sent, No response from SGD!!", ucmlogging::Error);
    	}
    	// if notifierString is "EVENT"
    	else if(!strcmp(notifierString.c_str(),"EVENT"))
    	{
    		log.logger("Notifying " + notifierString, ucmlogging::Info);
    		strcpy(notifier,notifierString.c_str());
    		cv_event.notify_all();
    		sleep(1);
    	}
    	// if notifierString is "POLL"
    	else if(!strcmp(notifierString.c_str(),"POLL"))
    	{
    		log.logger("Notifying " + notifierString, ucmlogging::Info);
    		strcpy(notifier,notifierString.c_str());
    		cv_poll.notify_all();
    		sleep(1);
    	}
    	// if notifierString is "LOG"
    	else if(!strcmp(notifierString.c_str(),"LOG"))
    	{
    		log.logger("Notifying " + notifierString, ucmlogging::Info);
    		strcpy(notifier,notifierString.c_str());
    		cv_log.notify_all();
    		sleep(1);
    	}
		threadVal = getThreadStatus(notifierString);
    }

	// Send additional notify, if threads are not cancelled at the first attempt.
    count = count*threadVal;
    while(threadVal && count--)
    {
    	if(!strcmp(notifierString.c_str(),"REPORT"))
    	{
    		rh.getReportdata();
    		map<string, unsigned int>::iterator iItr;
    		iItr = rh.rrid_cvMap.begin();
    		while(threadVal-- && iItr != rh.rrid_cvMap.end())
    		{
    			log.logger("Notifying " + notifierString, ucmlogging::Info);
    			strcpy(notifier,notifierString.c_str());
    			cv_report[iItr->second].notify_all();
    			iItr++;
    			sleep(1);
    		}

    	}
    	else if(!strcmp(notifierString.c_str(),"SIGNAL"))
    	{
    		eh.getsignalindexmap();
    		map<string, unsigned int>::iterator iItr;
    		iItr = eh.signalcvindexmap.begin();
    		while(threadVal-- && iItr != eh.signalcvindexmap.end())
    		{
				log.logger("Notifying " + notifierString, ucmlogging::Info);
				strcpy(notifier,notifierString.c_str());
				cv_signal[iItr->second].notify_all();
				iItr++;
				sleep(1);
    		}
    	}
    	else if(!strcmp(notifierString.c_str(),"EVENT"))
    	{
    		log.logger("Notifying " + notifierString, ucmlogging::Info);
    		strcpy(notifier,notifierString.c_str());
    		cv_event.notify_all();
    		sleep(1);
    	}
    	else if(!strcmp(notifierString.c_str(),"POLL"))
    	{
    		log.logger("Notifying " + notifierString, ucmlogging::Info);
    		strcpy(notifier,notifierString.c_str());
    		cv_poll.notify_all();
    		sleep(1);
    	}
    	else if(!strcmp(notifierString.c_str(),"LOG"))
    	{
    		log.logger("Notifying " + notifierString, ucmlogging::Info);
    		strcpy(notifier,notifierString.c_str());
    		cv_log.notify_all();
    		sleep(1);
    	}
    	sleep(10);
    	threadVal = getThreadStatus(notifierString);
    }
    strcpy(notifier,"RESET");
}


void VENManager::Login(void)
{

	// This is OpenADR Configuration page login button
	// Username and password will be read and compared

	ucmlogging log;
	sqliteDB sdb;
	log.logger("Attempting to login to the OpenADR configuration page ", ucmlogging::Info);
	string username = sdb.sqliteDBreadString("oadrusername", hmiven, "hmiparam");
	string password = sdb.sqliteDBreadString("oadrpassword", hmiven, "hmiparam");
	if((!(strcmp(username.c_str(),loginUsername))) && (!(strcmp(password.c_str(),loginPassword))))
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	else
	{
		sdb.sqliteDBwriteString("errormsg","Invalid Login credentials!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
		log.logger("Invalid Login details!!!", ucmlogging::Error);
	}

}

void VENManager::StartPolling(VEN2b *venObject)
{

	// Start Polling button
	// getting poll frequency and registering VEN
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("Start Polling button is clicked ", ucmlogging::Info);
	int pollFreq=0;
	pollFreq = sdb.sqliteDBreadInt("pollfreq", hmiven, "hmiparam");

	if(!(ifregistered()))
	{
		//VEN Registration
		log.logger("VEN is trying to Register with VTN ", ucmlogging::Info);
		registerParty rh;
		success = rh.RegisterParty(venObject);
		if(!success)
		{
			log.logger("Register Party Failed!!!", ucmlogging::Error);
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			log.logger("Failed to register!!!", ucmlogging::Error);
			return;
		}
		log.logger("VEN Registration is successful ", ucmlogging::Info);
		updateParameter();

		//register report
		log.logger("Sending Register Report!!!", ucmlogging::Info);
		reportRegister rr(getVenId());
		// Register Report
		if(rr.ReportRegister(venObject))
		{
			success &=true;
			log.logger("Report(s) Successfully registered!!!", ucmlogging::Info);
							}
		else
		{
			success = false;
			log.logger("Report(s) Registration Failed!!!", ucmlogging::Error);
		}
	}
	else
		success = true;

	if(success)
	{
		if((getThreadStatus("POLL")))
		{
			sdb.sqliteDBwriteString("errormsg","Poll thread already running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
			log.logger("Poll thread already running!!!", ucmlogging::Error);
			return;
		}
		//Start polling thread
		log.logger("VEN Polling is enabled", ucmlogging::Info);
		pollHandler ph(getVenId(), getRegistrationId(),getVtnId(), pollFreq);
		thread pollingThread(&pollHandler::startPolling, ph, venObject, this, &cv_poll, &cv_event, cv_signal, cv_report, notifier);
		pollingThread.detach();
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to register report!!!", ucmlogging::Error);
	}

}

void VENManager::StopPolling(void)
{
	//Stop polling button
	//Send notification to the polling thread and stop polling

	ucmlogging log;
	sqliteDB sdb;
	log.logger("Stopping VEN Polling ", ucmlogging::Info);
	waitNotifier("POLL");
	if(!(getThreadStatus("POLL")))
	{
		log.logger("VEN Polling is stopped ", ucmlogging::Info);
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Failed to stop poll thread. Poll thread still running!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
		log.logger("Failed to Stop poll thread. Poll thread still running!!!", ucmlogging::Error);
	}
}

void VENManager::RegisterVEN(VEN2b *venObject)
{
	// Register button
	// VEN Registration
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("VEN is trying to Register with VTN ", ucmlogging::Info);
	registerParty rh;
	success = rh.RegisterParty(venObject);
	if(!success)
	{
		log.logger("Register Party Failed!!!", ucmlogging::Error);
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to Register!!!", ucmlogging::Error);
		return;
	}
	updateParameter();

	log.logger("Sending Register Report!!!", ucmlogging::Info);
	// Register Report
	reportRegister rr(getVenId());
	if(rr.ReportRegister(venObject))
	{
		success &=true;
		log.logger("Report(s) Successfully registered!!!", ucmlogging::Info);
	}
	else
	{
		success = false;
		log.logger("Report(s) Registration Failed!!!", ucmlogging::Error);
	}

	if(success)
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to Register report!!!", ucmlogging::Error);
	}
}

void VENManager::QueryRegistration(VEN2b *venObject)
{
	// Query Registration button
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("VEN is trying to Query Registration details from VTN ", ucmlogging::Info);
	registrationQuery qr;
	success = qr.RegistrationQuery(venObject);
	if(success)
	{
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
		log.logger("Query Registration is successful ", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to Query registration details!!!", ucmlogging::Error);
	}
}

void VENManager::CancelRegistration(VEN2b *venObject)
{
	//Cancel Registration button
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("VEN is trying to Cancel the registration with VTN ", ucmlogging::Info);
	cancelParty cp;
	if((ifregistered()))
		success = cp.CancelParty(venObject);
	else
		success = true;
	if(success)
	{
		waitNotifier("POLL");
		if((getThreadStatus("POLL")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop poll thread. Poll thread still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
			log.logger("Failed to stop poll thread.Poll thread still running!!!Cancel Registration failed!!!", ucmlogging::Error);
			return;
		}
		waitNotifier("EVENT");
		if((getThreadStatus("EVENT")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop event thread. Event thread still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
			log.logger("Failed to stop event thread. Event thread still running!!!Cancel Registration failed!!!", ucmlogging::Error);
			return;
		}
		waitNotifier("SIGNAL");
		if((getThreadStatus("SIGNAL")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop signal thread(s). Signal thread(s) still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
			log.logger("Failed to stop signal thread(s). Signal thread(s) still running!!!Cancel Registration failed!!!", ucmlogging::Error);
			return;
		}
		waitNotifier("REPORT");
		if((getThreadStatus("REPORT")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop report thread(s). Report thread(s) still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
			log.logger("Failed to stop report thread(s). Report thread(s) still running!!!Cancel Registration failed!!!", ucmlogging::Error);
			return;
		}
		remove(eventdetails);
		remove(reportdetails);
		remove(signaldetails);
		log.logger("Cancel Registration is successful ", ucmlogging::Info);
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to Cancel the registration!!!", ucmlogging::Error);
	}
}

void VENManager::ClearRegistration(void)
{
	//Clear Registration button
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;
	clearRegistration cr;

	log.logger("Clear Registration button is clicked ", ucmlogging::Info);
	log.logger("Clearing Registration details without notifying VTN ", ucmlogging::Info);
	success = cr.clearregistration();
	if(success)
	{
		log.logger("Clear Registration is successful ", ucmlogging::Info);
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Failed to clear registration!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to clear registration!!!", ucmlogging::Error);
	}
}

void VENManager::ReRegistration(VEN2b *venObject)
{
	//Re-registration button
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("VEN is trying to do a re-registration with VTN ", ucmlogging::Info);
	registerParty rh(getVenId(), getRegistrationId());
	success = rh.RegisterParty(venObject);
	if(success)
	{
		waitNotifier("POLL");
		if((getThreadStatus("POLL")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop poll thread. Poll thread still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			log.logger("Failed to stop poll thread. Poll thread still running!!!Re-Registration failed!!!", ucmlogging::Error);
			return;
		}
		waitNotifier("EVENT");
		if((getThreadStatus("EVENT")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop event thread. Event thread still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			log.logger("Failed to stop event thread. Event thread still running!!!Re-Registration failed!!!", ucmlogging::Error);
			return;
		}
		waitNotifier("SIGNAL");
		if((getThreadStatus("SIGNAL")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop signal thread(s). Signal thread(s) still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			log.logger("Failed to stop signal thread(s). Signal thread(s) still running!!!Re-Registration failed!!!", ucmlogging::Error);
			return;
		}
		waitNotifier("REPORT");
		if((getThreadStatus("REPORT")))
		{
			sdb.sqliteDBwriteString("errormsg","Failed to stop report thread(s). Report thread(s) still running!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			log.logger("Failed to stop report thread(s). Report thread(s) still running!!!Re-Registration failed!!!", ucmlogging::Error);
			return;
		}
		remove(eventdetails);
		remove(reportdetails);
		remove(signaldetails);
		log.logger("Re-registration is successful ", ucmlogging::Info);
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to do Re-registration!!!", ucmlogging::Error);
	}
}


void VENManager::OptEvent(VEN2b *venObject)
{
	//Opt-out or Opt-in and Event from HMI
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	string eventID, reqID;
	unsigned int duration=0, modificationNo=0;
	bool optout=false;
	int customerOverride;
	time_t currentEvent=0;
	time_t currentTime = time(0);
	EventHandler eh;

	eventID = sdb.sqliteDBreadString("eventid", hmiven, "optconfig");
	customerOverride = sdb.sqliteDBreadInt("customeroverride", hmiven, "hmiparam");
	log.logger("Changing the opt status for the Event - " +eventID, ucmlogging::Info);

	//read the event details from Event-database
	eh.getdata();
	eh.stringIterator = eh.eventidreqidmap.find(eventID);
	if(eh.stringIterator != eh.eventidreqidmap.end())
		reqID = eh.stringIterator->second;

	eh.stringIterator = eh.eventidoptstatusmap.find(eventID);
	if(eh.stringIterator != eh.eventidoptstatusmap.end())
	{
		if(eh.stringIterator->second.compare("opt-in")==0)
			optout = true;
		else
			optout = false;
	}
	eh.intIterator = eh.eventidmodmap.find(eventID);
	if(eh.intIterator != eh.eventidmodmap.end())
		modificationNo = eh.intIterator->second;

	eh.uintIterator = eh.eventiddurationmap.find(eventID);
	if(eh.uintIterator != eh.eventiddurationmap.end())
		duration = eh.uintIterator->second;

	eh.timeIterator = eh.eventiddtstartmap.find(eventID);
	if(eh.timeIterator != eh.eventiddtstartmap.end())
		currentEvent = eh.timeIterator->second;

	if(eh.eventActiveSignalCheck(currentTime, currentEvent, duration))
	{
		if((customerOverride==0))
		{
			log.logger("Event is active and Customer override is disabled!!! Cannot opt-out the Event - " +eventID, ucmlogging::Info);
			sdb.sqliteDBwriteString("errormsg","Event is active and Customer override is disabled!!! Cannot opt-out the Event!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			return;
		}
	}
	//send created event with optstatus to VTN
	if(reqID.compare("")!=0)
	{
		EventResponses er;
		if(optout)
			er.addEventResponse("200", "OK", eventID, modificationNo, OptTypeType::optOut, reqID);
		else
			er.addEventResponse("200", "OK", eventID, modificationNo, OptTypeType::optIn, reqID);

		success = sendCreatedEvent(venObject,er);
	}
	else
		success = true;

	//if event is active stop the event, else update opt-state in event - database.
	if(success)
	{
		if(eh.eventActiveSignalCheck(currentTime, currentEvent, duration))
		{
			if((customerOverride!=0))
			{
				if(optout)
				{
					waitNotifier("SIGNAL");
					if((getThreadStatus("SIGNAL")))
					{
						sdb.sqliteDBwriteString("errormsg","Failed to stop signal thread(s). Signal thread(s) still running!!!", hmiven, "hmivenflag");
						sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
						log.logger("Failed to stop signal thread(s). Signal thread(s) still running!!! Event opt-out failed!!!", ucmlogging::Error);
						return;
					}
					if(!(sdb.sqliteDBdeletestring("eventdetails", eventdetails, "eventid", eventID)))
					{
						sdb.sqliteDBwriteString("errormsg","Failed to delete "+eventID+" from eventdetails.db!!!", hmiven, "hmivenflag");
						sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
					}
					else
					{
						log.logger("Deleted eventID - " +eventID+" from database!!!", ucmlogging::Info);
						sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
					}
				}
				else
				{
					if(!(sdb.sqliteDBwriteStringWithCond("optstatus", optout?"opt-out_hmi":"opt-in", eventdetails, "eventdetails","eventid",eventID)))
					{
						log.logger("eventdetails.db update failed for eventID - " +eventID, ucmlogging::Error);
						sdb.sqliteDBwriteString("errormsg","Failed to update eventdetails.db!!!", hmiven, "hmivenflag");
						sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
					}
					else
					{
						log.logger("eventdetails.db updated for eventID - " +eventID, ucmlogging::Info);
						sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
					}
				}
			}
		}
		else
		{
			if(!(sdb.sqliteDBwriteStringWithCond("optstatus", optout?"opt-out_hmi":"opt-in", eventdetails, "eventdetails","eventid",eventID)))
			{
				log.logger("eventdetails.db update failed for eventID - " +eventID, ucmlogging::Error);
				sdb.sqliteDBwriteString("errormsg","Failed to update eventdetails.db!!!", hmiven, "hmivenflag");
				sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
			}
			else
			{
				log.logger("eventdetails.db updated for eventID - " +eventID, ucmlogging::Info);
				sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
			}
		}
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to send oadrcreatedEvent message!!!", ucmlogging::Error);
	}
}


void VENManager::CreateOpt(VEN2b *venObject)
{
	//create an OPT-Schedule from HMI

	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("Create opt schedule button is clicked ", ucmlogging::Info);
	string starttime, stoptime, optreason;
	starttime = sdb.sqliteDBreadString("starttime", hmiven, "optconfig");
	stoptime = sdb.sqliteDBreadString("endtime", hmiven, "optconfig");
	optreason = sdb.sqliteDBreadString("optoutreason", hmiven, "optconfig");
	OptScheduleCreate optsch;
	log.logger("Sending oadrcreateOptSchedule message to VTN!!!", ucmlogging::Info);
	success = optsch.OptSchCreate(venObject, starttime, stoptime, optreason);
	if(success)
	{
		if(!(sdb.insertDBoptschedules(starttime, stoptime, optreason, optsch.getOptIdVal())))
		{
			log.logger("optschedules.db update failed for optID - " +optsch.getOptIdVal(), ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Failed to update optschedules.db database!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		}
		else
		{
			log.logger("optschedules.db updated with optID - " +optsch.getOptIdVal(), ucmlogging::Info);
			// Sending oadrCreatedEvent messages with opt-out status, for all the Events which falls in this schedule
			if(updateEventsOptout(venObject))
				sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
			else
			{
				sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
				log.logger("Failed to send oadrcreatedEvent for the relevant Events!!!", ucmlogging::Error);
			}
		}
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to create the opt-out schedule!!!", ucmlogging::Error);
	}

}

void VENManager::CancelOpt(VEN2b *venObject)
{
	//Cancel an OPT-Schedule from HMI
	ucmlogging log;
	sqliteDB sdb;
	bool success= false;

	log.logger("Cancel opt schedule button is clicked ", ucmlogging::Info);
	string optID;
	OptScheduleCancel optcancel;
	optID = sdb.sqliteDBreadString("optid", hmiven, "optconfig");
	log.logger("Sending oadrcancelOptSchedule to VTN!!!", ucmlogging::Info);
	success = optcancel.OptSchCancel(venObject, optID);
	if(success)
	{
		if(!(sdb.sqliteDBdeletestring("optoutschedules", optschedule, "optid", optID)))
		{
			log.logger("optschedules.db delete failed for optID - " +optID, ucmlogging::Error);
			sdb.sqliteDBwriteString("errormsg","Failed to update the optschedule.db database!!!", hmiven, "hmivenflag");
			sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		}
		else
		{
			log.logger("optschedules.db delete is successful for optID - " +optID, ucmlogging::Info);
			EventHandler eh;
			eh.getdata();
			// Sending oadrCreatedEvent messages with opt-in status, for all the Events which falls in this schedule
			if(!(updateEventsOptin(venObject,optID)))
			{
				sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
				log.logger("Failed to send oadrcreatedEvent for the relevant Events!!!", ucmlogging::Error);
			}
			else
			{
				if(!(eh.updateEventDetailsOptstatus(optID)))
				{
					log.logger("Failed to update eventdetails.db, with the opt state of optID - " +optID, ucmlogging::Error);
					sdb.sqliteDBwriteString("errormsg","Failed to update eventdetails.db database!!!", hmiven, "hmivenflag");
					sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
				}
				else
				{
					sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
					log.logger("Cancellation of the opt-out schedule is successful!!!", ucmlogging::Info);
				}
			}
		}
	}
	else
	{
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Failed to Cancel the opt-out schedule!!!", ucmlogging::Error);
	}
}


void VENManager::StartLogThread(VEN2b *venObject)
{
	//Start LogThread button
	ucmlogging log;
	sqliteDB sdb;

	log.logger("Starting VEN LogThread ", ucmlogging::Info);
	if(!(getThreadStatus("LOG")))
	{
		startDataLogThread(venObject, 1);
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","LogThread already running!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
		log.logger("LogThread already running!!!", ucmlogging::Error);
	}
}


void VENManager::StopLogThread(void)
{
	//Stop LogThread button
	ucmlogging log;
	sqliteDB sdb;

	//Send notification to the LogThread thread
	log.logger("Stopping VEN LogThread ", ucmlogging::Info);
	waitNotifier("LOG");
	if(!(getThreadStatus("LOG")))
	{
		log.logger("VEN LogThread is stopped ", ucmlogging::Info);
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Failed to stop LogThread. LogThread still running!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag"); // Set the flag value to 9, to indicate error
		log.logger("Failed to Stop LogThread. LogThread still running!!!", ucmlogging::Error);
	}
}

void  VENManager::OpenComPort(void)
{
	//Open communication port
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Connecting to the comm port!!!", ucmlogging::Info);
	char port[20];
	string commport = sdb.sqliteDBreadString("commport", hmiven, "cea2045config");
	strcpy(port,commport.c_str());
	if(open_serialport(port) == 0)
	{
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Comm Port opened successfully!!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("commport","", hmiven,"cea2045config");
		sdb.sqliteDBwriteString("errormsg","Error Opening CommPort!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Comm port opening Error!!!", ucmlogging::Error);
	}
}

void VENManager::CloseComPort(void)
{
	//Close communication port
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Disconnecting the comm port!!!", ucmlogging::Info);
	if(close_serialport() == 0)
	{
		sdb.sqliteDBwriteString("commport","", hmiven,"cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Comm Port closed successfuly!!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Error Closing CommPort!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Comm Port closing Error!!!", ucmlogging::Error);
	}
}

void VENManager::QueryOperation(void)
{
	//Query Operational state of SGD
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Querying the Operational state of SGD!!!", ucmlogging::Info);
	int opstate = getOperationalState();
	if(opstate == 0)
	{
		sdb.sqliteDBwriteInt("operstate",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Operational State --> Idle Normal", ucmlogging::Info);
	}
	else if(opstate == 1)
	{
		sdb.sqliteDBwriteInt("operstate",1, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Operational State --> Running Normal", ucmlogging::Info);
	}
	else if(opstate == 2)
	{
		sdb.sqliteDBwriteInt("operstate",2, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Operational State --> Running Curtailed Grid", ucmlogging::Info);
	}
	else if(opstate == 3)
	{
		sdb.sqliteDBwriteInt("operstate",3, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Operational State --> Running Heightened Grid", ucmlogging::Info);
	}
	else if(opstate == 4)
	{
		sdb.sqliteDBwriteInt("operstate",4, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Operational State --> Idle Grid", ucmlogging::Info);
	}
	else if(opstate == 5)
	{
		sdb.sqliteDBwriteInt("operstate",5, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Operational State --> SGD Error Condition", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Operational state not recieved!!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Operational State --> Unused opstate OR No reponse from SGD", ucmlogging::Error);
	}
}

void VENManager::SendOutsideComm(void)
{
	//Send Outside communication status to SGD
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Sending Outside communication status to SGD", ucmlogging::Info);
	int commstatus = sdb.sqliteDBreadInt("commstatus", hmiven, "cea2045config");
	if(sendCommstatus(commstatus))
	{
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Communication Status succesfully sent to SGD !! Ack recieved!! ", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Communication status sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Communication Status sent!! No response from SGD!!", ucmlogging::Error);
	}
}


void VENManager::ShedEvent(void)
{
	//Shed Event
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Load SHED signal to SGD", ucmlogging::Info);
	int duration = sdb.sqliteDBreadInt("duration", hmiven, "cea2045config");
	if(oadr_cea_EventMap("SIMPLE", NULL, "1", duration))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Shed Event succesfully sent to SGD !! Ack recieved!! ", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Shed Event sent !! No response from SGD !!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Shed Event sent !! No response from SGD !! ", ucmlogging::Error);
	}
}

void VENManager::EndShed(void)
{
	//End Shed
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing End SHED signal to SGD", ucmlogging::Info);
	if(oadr_cea_EventMap("END_SHED", NULL, NULL, 0))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("End Shed Event succesfully sent to SGD !! Ack recieved!! ", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","End Shed Event sent !! No response from SGD !!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("End Shed Event sent !! No response from SGD !!", ucmlogging::Error);
	}
}


void VENManager::LoadUp(void)
{
	//Load Up
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Load up signal to SGD", ucmlogging::Info);
	int duration = sdb.sqliteDBreadInt("duration", hmiven, "cea2045config");
	if(oadr_cea_EventMap("SIMPLE", NULL, "0", duration))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Load Up Event succesfully sent to SGD !! Ack recieved!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Load Up Event sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Load Up Event sent!! No response from SGD!! ", ucmlogging::Error);
	}

}

void VENManager::CriticalPeakEvent(void)
{
	//Critical Peak Event
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Critical Peak Event signal to SGD", ucmlogging::Info);
	int duration = sdb.sqliteDBreadInt("duration", hmiven, "cea2045config");
	if(oadr_cea_EventMap("SIMPLE", NULL, "2", duration))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Critical Peak Event succesfully sent to SGD !! Ack recieved!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Critical peak Event sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Critical Peak Event sent!! No response from SGD!!", ucmlogging::Error);
	}

}

void VENManager::GridEmergency(void)
{
	//Grid Emergency
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Grid Emergency signal to SGD", ucmlogging::Info);
	int duration = sdb.sqliteDBreadInt("duration", hmiven, "cea2045config");
	if(oadr_cea_EventMap("SIMPLE", NULL, "3", duration))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Grid Emergency Event succesfully sent to SGD !! Ack recieved!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Grid Emergency Event sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Grid Emergency Event sent!! No response from SGD", ucmlogging::Error);
	}
}

void VENManager::GridGuidanceGoodTime(void)
{
	//Grid Guidance - Good time to use energy
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Grid Guidance - Good time to use energy signal to SGD", ucmlogging::Info);
	if(oadr_cea_EventMap("GRID_GUIDANCE", NULL, NULL, 2))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Grid Guidance successfully sent to SGD !! Ack received!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Grid Guidance sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Grid Guidance sent!! No response from SGD!!", ucmlogging::Error);
	}

}

void VENManager::GridGuidanceBadTime(void)
{
	//Grid Guidance - Bad time to use energy
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Grid Guidance - Bad time to use energy signal to SGD", ucmlogging::Info);
	if(oadr_cea_EventMap("GRID_GUIDANCE", NULL, NULL, 0))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Grid Guidance successfully sent to SGD !! Ack received!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Grid Guidance sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Grid Guidance sent!! No response from SGD!!", ucmlogging::Error);
	}
}


void VENManager::GridGuidanceNeutral(void)
{
	//Grid Guidance - Neutral
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Grid Guidance - Neutral signal to SGD", ucmlogging::Info);
	if(oadr_cea_EventMap("GRID_GUIDANCE", NULL, NULL, 1))
	{
		sdb.sqliteDBwriteInt("duration",0, hmiven, "cea2045config");
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Grid Guidance successfully sent to SGD !! Ack received!!", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Grid Guidance sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Grid Guidance sent!! No response from SGD!!", ucmlogging::Error);
	}

}


void VENManager::SetPrice(void)
{
	//Set Price
	ucmlogging log;
	sqliteDB sdb;
	log.logger("Issuing Set Price signal to SGD", ucmlogging::Info);
	char price[20];
	string relprice = sdb.sqliteDBreadString("relativeprice", hmiven, "cea2045config");
	strcpy(price, relprice.c_str());
	if(oadr_cea_EventMap("ELECTRICITY_PRICE", "priceMultiplier", price, 0))
	{
		sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag");
		log.logger("Set Price command successfully sent to SGD !! Ack received!! ", ucmlogging::Info);
	}
	else
	{
		sdb.sqliteDBwriteString("errormsg","Set Price cmd sent!! No response from SGD!!", hmiven, "hmivenflag");
		sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
		log.logger("Set Price command sent!! No response from SGD!! ", ucmlogging::Error);
	}
}
