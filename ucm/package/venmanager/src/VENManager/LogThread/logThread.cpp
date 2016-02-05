/*
 * logThread.cpp
 *
 *  Created on: 06-Nov-2014
 *      Author: dinesh
 */



#include "logThread.h"

extern "C"
{
	#include "../CEA2045/cea2045lib.h"
}

using namespace std;

char *notifierLog = new char[20];
char *notifierLogVal = new char[20];

/**
    logThread Constructor
    @param logFreq - Log frequency in Minutes from user
 */
logThread::logThread(unsigned int logFreq) : logFreq(logFreq)
{
	logExit=1;
}


/**
    Log thread, which Logs data from SGD at a specified frequency
    @param condition variable for notifying
    @param notifierpointer - location where notify context is checked
    @return - nothing
*/
void logThread::startLog(condition_variable *cvLog, char *notifierP)
{
	//Log Thread starts...
	sqliteDB sdb;
	ucmlogging log;

	sdb.sqliteDBwriteInt("logthread",1, thread_status, "threadstatus");
	log.logger("Log Thread Started", ucmlogging::Info);
	//wait_Untill
	mutex cv_Log;
	unique_lock<mutex> lkLog(cv_Log);

	notifierLog = notifierP;
	strcpy(notifierLogVal,"LOG");

	time_t currentInterval = time(0);

	while(logExit)
	{
		if(cvLog->wait_until(lkLog, chrono::system_clock::from_time_t(currentInterval), [](){return (!strcmp(notifierLog,notifierLogVal));}))
		{
			log.logger("Notification received for Log thread. Log thread stopping!!!", ucmlogging::Info);
			logExit=0;
		}
		else
		{
			//Get Device Info
			if(getDeviceInfo())
				log.logger("Device Information recieved", ucmlogging::Info);
			else
				log.logger("Device Information not recieved", ucmlogging::Error);

			//Send Set Commodity Subscription to SGD
			log.logger("Sending Set Commodity Subscription request!!!", ucmlogging::Info);
			if(setCommoditySubscription())
				log.logger("Set Commodity Subscription request succeeded!!!", ucmlogging::Info);
			else
				log.logger("Invalid message/response code. Set Commodity Subscription request failed!!!", ucmlogging::Error);

			//call log report function from cea2045 library
			if(!(oadr_cea_logReportInfo(1)))
				log.logger("Invalid message (or) response code !!!", ucmlogging::Error);
			else
				log.logger("Datapoints logged to dplogging.csv file !!!", ucmlogging::Info);
		}

		currentInterval = currentInterval + (getLogFreq()*60);

	}
	sdb.sqliteDBwriteInt("logthread",0, thread_status, "threadstatus");
}
