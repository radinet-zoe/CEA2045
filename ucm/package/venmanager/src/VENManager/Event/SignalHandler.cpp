/*
 * SignalHandler.cpp
 *
 *  Created on: 28-Aug-2014
 *      Author: dinesh
 */


#include <inttypes.h>
#include "SignalHandler.h"

extern "C"
{
	#include "../CEA2045/cea2045lib.h"
}

char *notifierSignal = new char[20];
char *notifierSignalVal = new char[20];


/**
    Method to update signal(s) of active Event in signaldetails.data file
    @param signalID
    @param signalCount - signal number
 */
bool SignalHandler::updateSignalData(string signalID)
{
	sqliteDB sdb;
	return sdb.sqliteDBdeletestring("signaldetails", signaldetails, "signalid", signalID);
}
/**
    Signal handler thread
    @param payload - signal payload
    @param signalCount - signal number
    @param condition variable for notifying
    @param notifierpointer - location where notify content is checked
 */
void SignalHandler::eventSignalThread(eiEventSignalsType payload, unsigned int signalCount, condition_variable *cvSignal, char *notifierP)
{
	sqliteDB sdb;
	mutex cv_Signal;
	unique_lock<mutex> lkSignal(cv_Signal);
	unsigned int signalIterator;
	unsigned int Duration, Delay=0;
	signalPayloadType *spt;
	PayloadFloatType *pft;
	ucmlogging log;
	char signame[20], sigtype[20], sigval[10];
	time_t currentInterval = time(0);

	log.logger("******************Signal- thread Started*********************" , ucmlogging::Info);

	notifierSignal = notifierP;
	strcpy(notifierSignalVal,"SIGNAL");

	eiEventSignalsType::eiEventSignal_iterator sigItr = payload.eiEventSignal().begin();

	//iterating to respective signal no.
	for(signalIterator=1; signalIterator < signalCount; signalIterator++)
		sigItr++;

	log.logger("Signal Name- "+sigItr->signalName(), ucmlogging::Info);
	log.logger("Signal Type- " + sigItr->signalType(), ucmlogging::Info);
	log.logger("Signal ID- " + sigItr->signalID(), ucmlogging::Info);

	intervals::interval_iterator intervalItr;
	intervals *EventIterationresponse = &sigItr->intervals();

	for (intervalItr = EventIterationresponse->interval().begin(); intervalItr != EventIterationresponse->interval().end(); intervalItr++)
	{
		if(cvSignal->wait_until(lkSignal, chrono::system_clock::from_time_t(currentInterval), [](){return (!strcmp(notifierSignal,notifierSignalVal));}))
		{
			log.logger("Notify Received for Signal - " + sigItr->signalID(), ucmlogging::Info);
			int threadVal = sdb.sqliteDBreadInt("signalthread", thread_status, "threadstatus");
			sdb.sqliteDBwriteInt("signalthread",(threadVal-1), thread_status, "threadstatus");

			if(!updateSignalData(sigItr->signalID()))
				log.logger("Failed to update report data file for - " +sigItr->signalID()+ " !!!", ucmlogging::Error);
			if(signalCount==1)//update datafile only once in signal thread 1
			{
				sleep(1);
				EventHandler eh;
				eh.updateEventDetails();
			}
			return;
		}
		else
		{
			log.logger("Signal " + sigItr->signalID() + " - TimedOut!!!", ucmlogging::Info);
			//Issue END_SHED command after SHED, CPP, GRID EMERGENCY, POWER LEVEL, GRID GUIDANCE
			if(Delay > 0)
			{
				log.logger("Issue END_SHED command after SHED, CPP, GRID EMERGENCY, POWER LEVEL, GRID GUIDANCE", ucmlogging::Info);
				if(!strcmp(signame, "SIMPLE") && ((!strcmp(sigval,"1")) || (!strcmp(sigval,"2")) || (!strcmp(sigval,"3"))))
				{
					bool eventRespstatus = oadr_cea_EventMap("END_SHED", NULL, NULL, 0);
					if(eventRespstatus)
						log.logger("Event message (END SHED) sent to sgd!! Response recieved!!", ucmlogging::Info);
					else
						log.logger("Event message (END SHED) sent to sgd!! No Response!!", ucmlogging::Error);
				}
				else if(!strcmp(signame, "LOAD_DISPATCH"))
				{
					bool eventRespstatus = oadr_cea_EventMap("END_SHED", NULL, NULL, 0);
					if(eventRespstatus)
						log.logger("Event message (END SHED) sent to sgd!! Response recieved!!", ucmlogging::Info);
					else
						log.logger("Event message (END SHED) sent to sgd!! No Response!!", ucmlogging::Error);
				}

			}

			log.logger("Signal " + sigItr->signalID() + " - Mapping CEA2045 Library", ucmlogging::Info);
			typedef ::icalendar_2_0::uid uid_type;

			spt = &((signalPayloadType&)intervalItr->streamPayloadBase().front());
			pft = &(PayloadFloatType&)spt->payloadBase();
			stringstream payload;
			payload<<pft->value();
			//log.logger("Payload value: " + payload.str(), ucmlogging::Info);

			DurationPropType *duration = &intervalItr->duration().get();
			string StringDur =  duration->duration();

			unsigned pos = StringDur.find_last_of("T");
			string str3 = StringDur.substr(pos+1);
			string str4 = str3.substr(0, str3.size()-1);

			stringstream convert(str4);
			if ( !(convert >> Duration) )
				Duration = 0;


			strcpy(signame,sigItr->signalName().c_str());
			strcpy(sigtype,sigItr->signalType().c_str());
			strcpy(sigval,payload.str().c_str());

			//Mapping to CEA2045 message
			bool eventRespstatus;
			eventRespstatus = oadr_cea_EventMap(signame, sigtype, sigval, Duration);
			if(eventRespstatus)
				log.logger("Event message sent to sgd!! Response recieved!!", ucmlogging::Info);
			else
				log.logger("Event message sent to sgd!! No Response!!", ucmlogging::Error);

		}
		Delay=Duration;
		currentInterval = currentInterval + (Delay*60);
		log.logger( "CEA2045 Library Mapping Done for - " + sigItr->signalID(), ucmlogging::Info);
		stringstream temp;
		temp<<Delay;
		log.logger(temp.str() +" Minute(s) for next interval." , ucmlogging::Info);
	} //end of interval iterator
	if(cvSignal->wait_until(lkSignal, chrono::system_clock::from_time_t(currentInterval), [](){return (!strcmp(notifierSignal,notifierSignalVal));}))
		log.logger("Notify Received for Signal - " + sigItr->signalID(), ucmlogging::Info);
	else
	{
		if(Delay > 0)
		{
			//Issue END_SHED command after SHED, CPP, GRID EMERGENCY, POWER LEVEL, GRID GUIDANCE
			log.logger("", ucmlogging::Info);
			logger("Issue END_SHED command after SHED, CPP, GRID EMERGENCY, POWER LEVEL, GRID GUIDANCE");
			if(!strcmp(signame, "SIMPLE") && ((!strcmp(sigval,"1")) || (!strcmp(sigval,"2")) || (!strcmp(sigval,"3"))))
			{
				bool eventRespstatus = oadr_cea_EventMap("END_SHED", NULL, NULL, 0);
				if(eventRespstatus)
					log.logger("Event message (END SHED) sent to sgd!! Response recieved!!", ucmlogging::Info);
				else
					log.logger("Event message (END SHED) sent to sgd!! No Response!!", ucmlogging::Error);
			}
			else if(!strcmp(signame, "LOAD_DISPATCH"))
			{
				bool eventRespstatus = oadr_cea_EventMap("END_SHED", NULL, NULL, 0);
				if(eventRespstatus)
					log.logger("Event message (END SHED) sent to sgd!! Response recieved!!", ucmlogging::Info);
				else
					log.logger("Event message (END SHED) sent to sgd!! No Response!!", ucmlogging::Error);
			}

		}
	}

	int threadVal = sdb.sqliteDBreadInt("signalthread", thread_status, "threadstatus");
	sdb.sqliteDBwriteInt("signalthread",(threadVal-1), thread_status, "threadstatus");

	if(!updateSignalData(sigItr->signalID()))
		log.logger("Failed to update report data file for - " +sigItr->signalID()+ " !!!", ucmlogging::Error);

	log.logger("-----------Exit from signal thread - " + sigItr->signalID(), ucmlogging::Info);

	if(signalCount==1)//update datafile only once in signal thread 1
	{
		sleep(1);
		EventHandler eh;
		eh.updateEventDetails();
	}

}// end of signal thread function

