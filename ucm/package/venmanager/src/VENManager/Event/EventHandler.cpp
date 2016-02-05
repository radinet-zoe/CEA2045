/*
 * EventHandler.cpp
 *
 *  Created on: 27-Aug-2014
 *      Author: dinesh
 */

#include "EventHandler.h"
#include "../Main.h"

using namespace std;

char *notifierEvent = new char[20];
char *notifierEventVal = new char[20];

/**
    EventHandler Constructor
    @param venID - VEN ID returned while registration
    @param vtnID - VTN ID returned while registration
 */
EventHandler::EventHandler(string venID, string vtnID) :
		m_venID(venID), m_vtnID(vtnID)
{
	optOut = false;
}

/**
    Method to insert a valid event data to the database
    @param eventID
    @param modification number
    @param duration - duration in minuted
    @param dtstart - event start time in epoch type
    @param eStatus - event status (Avtive or Pending)
    @param erequestID - event requestID
    @param optStatus - event optstatus
    @param optId
 */
void EventHandler::insertDB(string eventID, int modificationNumber, unsigned int Duration, time_t dtstart, string eStatus,
		                    string erequestID, string optStatus, string optID)
{
	eventidmodmap.insert (pair<string,int>(eventID,modificationNumber));
	eventiddtstartmap.insert (pair<string,time_t>(eventID,dtstart));
	eventiddurationmap.insert (pair<string,unsigned int>(eventID,Duration));
	eventidstatusmap.insert (pair<string,string>(eventID,eStatus));
	eventidoptstatusmap.insert (pair<string,string>(eventID,optStatus));
	eventidreqidmap.insert (pair<string,string>(eventID,erequestID));
	eventidoptidmap.insert (pair<string,string>(eventID,optID));
}

/**
    Method to clear an invalid event or a missing event, from the list of known events
    @param eventID
 */
void EventHandler::clearDB(string eventID)
{
	map<string,int>::iterator iIterator;
	map<string,unsigned int>::iterator uiIterator;
	map<string,time_t>::iterator tIterator;
	map<string,string>::iterator sIterator;
	//delete modification no
	iIterator = eventidmodmap.find(eventID);
	if(iIterator != eventidmodmap.end())
		eventidmodmap.erase (iIterator);
	//delete duration
	uiIterator = eventiddurationmap.find(eventID);
	if(uiIterator != eventiddurationmap.end())
		eventiddurationmap.erase (uiIterator);
	//delete starttime
	tIterator = eventiddtstartmap.find(eventID);
	if(tIterator != eventiddtstartmap.end())
		eventiddtstartmap.erase (tIterator);
	//delete status
	sIterator = eventidstatusmap.find(eventID);
	if(sIterator != eventidstatusmap.end())
		eventidstatusmap.erase (sIterator);
	//delete optstatus
	sIterator = eventidoptstatusmap.find(eventID);
	if(sIterator != eventidoptstatusmap.end())
		eventidoptstatusmap.erase (sIterator);
	//delete reqID
	sIterator = eventidreqidmap.find(eventID);
	if(sIterator != eventidreqidmap.end())
		eventidreqidmap.erase (sIterator);
	//delete optid
	sIterator = eventidoptidmap.find(eventID);
	if(sIterator != eventidoptidmap.end())
		eventidoptidmap.erase (sIterator);
}

/**
    Method to read the known events list from the eventdetails.db file, and store it in map
 */
void EventHandler::getdata()
{
	sqliteDB sdb;
	struct stat st = {0};
	if (!(stat(eventdetails, &st) == -1))
	{
		sdb.readDBeventdetails(&eventidmodmap,
			  	  	  	       &eventiddtstartmap,
			  	  	  	       &eventiddurationmap,
			  	  	  	       &eventidstatusmap,
			  	  	  	       &eventidoptstatusmap,
			  	  	  	       &eventidreqidmap,
			  	  	  	       &eventidoptidmap);
	}
	if (!(stat(optschedule, &st) == -1))
		sdb.readDBoptschedules(&optIDStartTimemap, &optIDStopTimemap);

}

/**
    Method to read the signal(s) list (of the active Event), from the signaldetails.db file and store it in map
 */
void EventHandler::getsignalindexmap(void)
{
	sqliteDB sdb;
	sdb.readDBsignaldetails(&signalcvindexmap);
}

/**
    Method to update the Event details in the eventdetails.db database, with optstatus
 */
bool EventHandler::updateEventDetailsOptstatus(string optID)
{
	sqliteDB sdb;
	EventHandler eh;
	int customerOverride;
	unsigned int duration=0;
	time_t currentTime = time(0);
	struct stat st = {0};
	ucmlogging log;
	map<string,string>::iterator sIterator;
	bool status = true;

	if ((stat(eventdetails, &st) == -1))
		return true;

	customerOverride = sdb.sqliteDBreadInt("customeroverride", hmiven, "hmiparam");
	eh.getdata();
	for (sIterator=eh.eventidoptidmap.begin(); sIterator!=eh.eventidoptidmap.end(); ++sIterator)
	{
		if(sIterator->second.compare(optID) == 0)
		{
			eh.uintIterator = eh.eventiddurationmap.find(sIterator->first);
			duration = eh.uintIterator->second;
			eh.timeIterator = eh.eventiddtstartmap.find(sIterator->first);
			if(eh.eventActiveSignalCheck(currentTime, eh.timeIterator->second, duration))
			{
				if((customerOverride!=0))
				{
					if(!(sdb.sqliteDBwriteStringWithCond("optstatus", "opt-in", eventdetails, "eventdetails","eventid",sIterator->first)))
						status = false;
					if(!(sdb.sqliteDBwriteStringWithCond("optid", "", eventdetails, "eventdetails","eventid",sIterator->first)))
						status = false;
				}
			}
			else
			{
				if(!(sdb.sqliteDBwriteStringWithCond("optstatus", "opt-in", eventdetails, "eventdetails","eventid",sIterator->first)))
					status = false;
				if(!(sdb.sqliteDBwriteStringWithCond("optid", "", eventdetails, "eventdetails","eventid",sIterator->first)))
					status = false;
			}
		}
	}

	return status;
}

/**
    Method to update Event details in the eventdetails.db file
 */
void EventHandler::updateEventDetails()
{
	sqliteDB sdb;
	ucmlogging log;
	struct stat st = {0};
	if (!(stat(eventdetails, &st) == -1))
		getdata();
	else
		return;

	map <string,int> tempmap;
	tempmap.insert(eventidmodmap.begin(), eventidmodmap.end());

	//iterate through events and remove events with past time from stored map
	for (intIterator=tempmap.begin(); intIterator!=tempmap.end(); ++intIterator)
	{
		map<string,unsigned int>::iterator intiterator;

		timeIterator = eventiddtstartmap.find(intIterator->first);
		intiterator = eventiddurationmap.find(intIterator->first);

		if(!eventDateCheck(time(0), timeIterator->second))
		{
			time_t currentEvent = timeIterator->second;
			unsigned int duration = intiterator->second;

			//to make the event active, if signal(s) of that event is active or duration is zero...
			if((eventActiveSignalCheck(time(0), currentEvent, duration)) || (!duration))
			{
				if(!(sdb.sqliteDBwriteStringWithCond("status", "Active", eventdetails, "eventdetails","eventid", (intIterator->first))))
					log.logger("eventdetails.db update failed for the Event - " + (intIterator->first), ucmlogging::Error);
			}
			else
				sdb.sqliteDBdeletestring("eventdetails", eventdetails, "eventid", (intIterator->first));
		}
	}

	//clear the context of map.
	tempmap.clear();
	tempmap.insert(eventidmodmap.begin(), eventidmodmap.end());
	for (intIterator=tempmap.begin(); intIterator!=tempmap.end(); ++intIterator)
		clearDB(intIterator->first);

}

/**
    Method to check whether any active signal, is associated with the event
	@param current time since the epoch type
    @param event time since the epoch type
    @param duration in minutes
    @return - status
 */
bool EventHandler::eventActiveSignalCheck(time_t current, time_t event, unsigned int duration)
{
	double diffStart = difftime(event, current);
	event += (duration*60); //etime in seconds and duration in minutes...
	double diffEnd = difftime(event, current);
	if(diffStart<0 && diffEnd>0)
		return true;
	else
		return false;
}

/**
    Method to validate VTNID
	@param vtnID
	@return - status
 */
bool EventHandler::validateVTNID(string VTNID)
{
	if(strcmp(VTNID.c_str(),getVtnId().c_str()))
		return false;
	return true;

}

/**
    Method to validate VENID
	@param venID
	@return - status
 */
bool EventHandler::validateVENID(string VENID)
{
	if(strcmp(VENID.c_str(),getVenId().c_str()))
			return false;
		return true;
}

/**
    Method to validate partyID
	@param partyID
	@param marketContext map from main
	@return - status
 */
bool EventHandler::validatepartyID(string VEN_Name, string PartyID)
{
	sqliteDB sdb;
	string marketContext;
	marketContext = sdb.sqliteDBreadString("drprogram", hmiven, "hmiparam");

	stringstream stream;
	stream<<marketContext;
	string singleContext;
	while(getline(stream, singleContext, ','))
	{
		string pID;
		pID = VEN_Name+":"+singleContext;
		if(!(pID.compare(PartyID)))
			return true;
	}
	return false;
}

/**
    Method to make sure that the modification no. of a known event is not less than the previous value
	@param eventID
	@param modification number
	@return - status
 */
bool EventHandler::validatemodificationNo(string eventID, int modificationNo)
{
	if(modificationNo == -1)
		return false;
	intIterator=eventidmodmap.find(eventID);
	if(intIterator != eventidmodmap.end())
	{
		if(modificationNo < intIterator->second)
			return true;
	}
	return false;
}

/**
    Method to validate, whether event start time is greater than the current time.
    @param current time since the epoch type
    @param event time since the epoch type
    @return - status
*/
bool EventHandler::eventDateCheck(time_t current, time_t event)
{
	double diff = difftime(event, current);
	if(diff>0)
		return true;
	else
		return false;
}

/**
    Method to extract the event dtstart time from payload.
    @param event time from iteration
    @return - event time since the epoch type
*/
time_t EventHandler::getEventTime(oadrDistributeEventType::oadrEvent_iterator itr)
{
	struct tm eventTime;
	eventTime.tm_isdst = 0;
	eventTime.tm_year= itr->eiEvent().eiActivePeriod().properties().dtstart().date_time().year()-1900;
	eventTime.tm_mon = itr->eiEvent().eiActivePeriod().properties().dtstart().date_time().month()-1;
	eventTime.tm_mday= itr->eiEvent().eiActivePeriod().properties().dtstart().date_time().day();
	eventTime.tm_hour= itr->eiEvent().eiActivePeriod().properties().dtstart().date_time().hours();
	eventTime.tm_min = itr->eiEvent().eiActivePeriod().properties().dtstart().date_time().minutes();
	eventTime.tm_sec = itr->eiEvent().eiActivePeriod().properties().dtstart().date_time().seconds();

	time_t evntTime = mktime(&eventTime);
	return evntTime;
}

/**
    Method to extract duration element from payload.
    @param duration in string
    @return - duration in minutes as integer
*/
unsigned int  EventHandler::durationExtract (string duration)
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
 * Method to validate signal interval payload values
 * @param signalIntervals
 * @param Values data type extracted form conf file
 * @param Values extracted form conf file
 * @param response description
 * @param Signal name
 * @return status
 */

bool EventHandler::validatePayloadFloatValues(intervals *signalIntervals, string valueType, string values, string *responsemsg, string signalName)
{
	ucmlogging log;
	intervals::interval_iterator intervalItr;
	PayloadFloatType *pft;
	signalPayloadType *spt;
	float upperlimit=0.0;
	float lowerlimit=0.0;
	stringstream valueRange;
	valueRange<<values;

	if(valueType.compare("any")==0)
		return true;

	if(values.compare("any")==0)
		return true;
	else
	{
		string singleContent;
		int i=0;
		while(getline(valueRange, singleContent, ':'))
		{
			if(i==0)
			{
				try
				{
					upperlimit = atof(singleContent.c_str());
				}
				catch(...)
				{
					log.logger("Invalid Range of payload values in signal.conf for signal - \""+signalName+"\"", ucmlogging::Error);
					*responsemsg = "Invalid Range of Payload Values in signal.conf for signal - \""+signalName+"\"";
					return false;
				}
			}
			else
			{
				try
				{
					lowerlimit = atof(singleContent.c_str());
				}
				catch(...)
				{
					log.logger("Invalid Range of payload values in signal.conf for signal - \""+signalName+"\"", ucmlogging::Error);
					*responsemsg = "Invalid Range of Payload Values in signal.conf for signal - \""+signalName+"\"";
					return false;
				}
			}
			i++;
		}
	}

	for (intervalItr = signalIntervals->interval().begin(); intervalItr != signalIntervals->interval().end(); intervalItr++)
	{
		spt = &((signalPayloadType&)intervalItr->streamPayloadBase().front());
		pft = &(PayloadFloatType&)spt->payloadBase();
		stringstream payload;
		payload<<pft->value();
		//log.logger("Payload value: " + payload.str(), ucmlogging::Info);
		if(valueType.compare("integer")==0)
		{
			int payloadVal = atoi(payload.str().c_str());
			if(payloadVal<(int)upperlimit || payloadVal>(int)lowerlimit)
			{
				//*responsemsg = "Invalid Payload Values for signal - \""+signalName+"\". Valid Values - "+to_string((int)upperlimit)+" to "+to_string((int)lowerlimit);
				*responsemsg = "Invalid Payload Values for signal -\""+signalName+"\"";
				return false;
			}

		}
		else if(valueType.compare("float")==0)
		{
			float payloadVal = atof(payload.str().c_str());
			if(payloadVal<upperlimit || payloadVal>lowerlimit)
			{
				//*responsemsg = "Invalid Payload Values for signal - \""+signalName+"\". Valid Values - "+to_string(upperlimit)+" to "+to_string(lowerlimit);
				*responsemsg = "Invalid Payload Values for signal -\""+signalName+"\"";
				return false;
			}
		}
	}
	return true;
}
/**
    Method to validate the signals - signal type, name and duration are validated
    @param signal payload
    @return - response code
    @return - response description
    @return - status
*/
bool EventHandler::validateSignals(oadrDistributeEventType::oadrEvent_iterator singleEvent, string *responseCode,
								   string *responseDescription, string *responsemsg)
{
	unsigned int eventDuration = durationExtract(singleEvent->eiEvent().eiActivePeriod().properties().duration().duration());
	ucmlogging log;
	eiEventSignalsType EventSignal = singleEvent->eiEvent().eiEventSignals();
	eiEventSignalsType::eiEventSignal_iterator sigItr;
	currencyType *ctp;
	ifstream fin;
	string payloadValues;

	for(sigItr=EventSignal.eiEventSignal().begin();sigItr!=EventSignal.eiEventSignal().end();sigItr++)
	{
		bool ifSignalValid = false;
		bool ifSignalFound = false;
		bool ifCurrencyValid = false;
		bool ifpayloadValuesValid = false;
		string PayloadValues;
		unsigned int signalDuration=0;
		string sigName = "[" + sigItr->signalName() + "]";
		//validate interval duration...
		intervals::interval_iterator intervalItr;
		intervals *EventIterationresponse = &sigItr->intervals();
		stringstream currency;

		//get currency type
		if(sigItr->itemBase().present())
		{
			ctp = &(currencyType&)sigItr->itemBase().get();
			currency<<ctp->itemDescription();
		}

		//log.logger("currenctVal - "+to_string(sigItr->currentValue()->payloadFloat().value()), ucmlogging::Info);
		//duration validation
		for (intervalItr = EventIterationresponse->interval().begin(); intervalItr != EventIterationresponse->interval().end(); intervalItr++)
		{
			DurationPropType *duration = &intervalItr->duration().get();
			signalDuration += durationExtract(duration->duration());
		}
		if(eventDuration != signalDuration)
		{
			*responseCode = "451";
			*responseDescription = "Not Allowed";
			*responsemsg = "Signal Duration not adding up to event duration";
			return false;
		}

		//UID validation
		unsigned int UID;
		for (intervalItr = EventIterationresponse->interval().begin(), UID = 0; intervalItr != EventIterationresponse->interval().end(); intervalItr++, UID++)
		{
			icalendar_2_0::uid *uid =  &intervalItr->uid().get();
			stringstream udiExtract;
			unsigned int UIDValue;

			udiExtract << uid->text();
			udiExtract >> UIDValue;
			if(UID != UIDValue)
			{
				*responseCode = "450";
				*responseDescription = "Out of sequence";
				*responsemsg = "Signal UID not in sequential order";
				return false;
			}
		}

		//signal name and Type Validation
		fin.open(signalsConfFile);
		if(fin.fail())
		{
			log.logger("Input file opening failed for signals.conf.", ucmlogging::Warning);
			*responseCode = "453";
			*responseDescription = "Not recognized";
			*responsemsg = "Signal validation failed.";
			return false;
		}
		while(!fin.eof())
		{
			string temp = "";
			getline(fin,temp);
			//validate signal name
			if(!(strcmp(temp.c_str(),sigName.c_str())))
			{
				ifSignalFound = true;
				getline(fin,temp);
				//validate signal type
				if(!(strcmp(temp.c_str(),sigItr->signalType().c_str())))
				{
					ifSignalValid = true;
					getline(fin,temp);

					//validate currency
					if(sigItr->itemBase().present())
					{
						string currencyType = currency.str();
						if(!(strcmp(currencyType.c_str(),temp.c_str())))
						{
							ifCurrencyValid = true;
							string valueType;
							getline(fin,valueType);
							getline(fin,PayloadValues);
							getline(fin,PayloadValues);
							ifpayloadValuesValid = validatePayloadFloatValues(&sigItr->intervals(),valueType, PayloadValues,responsemsg,sigItr->signalName());
						}
					}
					else
					{
						if(temp.compare("None")==0)
						{
							ifCurrencyValid = true;
							string valueType;
							getline(fin,valueType);
							getline(fin,PayloadValues);
							getline(fin,PayloadValues);
							ifpayloadValuesValid = validatePayloadFloatValues(&sigItr->intervals(),valueType, PayloadValues,responsemsg,sigItr->signalName());
						}
					}
				}
			}
		}
		fin.close();
		if(!ifSignalFound) // if signal not found
		{
			*responseCode = "453";
			*responseDescription = "Not recognized";
			*responsemsg = "Signal -\""+sigItr->signalName()+"\" not found in signals.conf file";
			 return false;
		}
		if(!ifSignalValid) // if signalType not valid
		{
			*responseCode = "460";
			*responsemsg = "Signal type - \""+sigItr->signalType()+"\" not supported for signal - \""+sigItr->signalName()+"\"";
			*responseDescription = "Signal not supported";
			return false;
		}
		if(!ifCurrencyValid) // if currencyType not valid
		{
			*responseCode = "460";
			*responsemsg = "Currency type - \""+currency.str()+"\" not supported for signal - \""+sigItr->signalName()+"\"";
			*responseDescription = "Signal not supported";
			return false;
		}
		if(!ifpayloadValuesValid) // if currencyType not valid
		{
			*responseCode = "454";
			*responseDescription = "Invalid Data";
			return false;
		}
	}
	return true;
}

/**
    Method to check whether the event exist in the list of known events
    @param eventID
    @return - status
*/
bool EventHandler::ifEventidexists(string eventID)
{
	if(!eventidmodmap.size())
		return false;
	intIterator=eventidmodmap.find(eventID);
	if(intIterator != eventidmodmap.end())
		return true;
	return false;
}

/**
    Method to check, whether the event run time is opted out. Opt-out schedules are stored in optoutschedules.db file.
    @param event time since the epoch type
    @return - status
*/
bool EventHandler::optOutStatus(time_t eventTime, unsigned int Duration)
{
	sqliteDB sdb;
	map<string,time_t>::iterator stopIterator;
	map<string,time_t>::iterator startIterator;
	bool optstate = false;
	ucmlogging log;
	time_t eventStop = eventTime + (Duration*60);

	for(startIterator = optIDStartTimemap.begin();startIterator != optIDStartTimemap.end(); ++startIterator)
	{
		stopIterator = optIDStopTimemap.find(startIterator->first);
		if(eventTime<stopIterator->second && startIterator->second<eventStop)
		{
			setOptId(startIterator->first);
			optstate = true;
		}
	}
	return optstate;
}

/**
    Method to validate an event
    @param pointer to class VENManager
    @param request ID
    @param event payload
    @param marketContext - marketContext map from main
    @param previousEvent - valid previous event (time since the epoch type)
    @return - status
*/
bool EventHandler::validateEvent(VENManager *vm, string reqID, oadrDistributeEventType::oadrEvent_iterator itr,
								 time_t *previousEvent, bool *previousDuration)
{
	ucmlogging log;
	bool status=true;
	responseCode = "200";
	responseDescription = "OK";
	string responsemsg = "OK";
	time_t currentEvent;

	//get event duration
	unsigned int duration;
	duration = durationExtract(itr->eiEvent().eiActivePeriod().properties().duration().duration());

	//get event's dtstart time
	currentEvent =  getEventTime(itr);
	//validate event payload

	//check for an event which is not in the list of known events, and the status in cancelled state
	if((!(ifEventidexists(itr->eiEvent().eventDescriptor().eventID()))) &&
	   (!(strcmp(itr->eiEvent().eventDescriptor().eventStatus().c_str(),"cancelled"))))
	{
		responseCode = "200";
		responseDescription = "OK";
		responsemsg = "Event is cancelled";
		status = false;
	}

	//known event modification number validation
	if((ifEventidexists(itr->eiEvent().eventDescriptor().eventID())) &&
	   (validatemodificationNo(itr->eiEvent().eventDescriptor().eventID(), itr->eiEvent().eventDescriptor().modificationNumber()))	&&
	    status)//-modification validate
	{
		responseCode = "450";
		responseDescription = "Out of sequence";
		responsemsg = "Event Modification Number validation Failed";
		status = false;
	}

	// event whose dtstart time is in the past (except for known events)
	if((!eventDateCheck(time(0), currentEvent)) && status)
	{
		if(!(ifEventidexists(itr->eiEvent().eventDescriptor().eventID())))
		{
			responseCode = "451";
			responseDescription = "Not Allowed";
			responsemsg = "Event Start time in past ";
			status = false;
		}
	}

	// event which does not have any duration element..// need to verify for NULL value in duration...
	if((!strcmp(itr->eiEvent().eiActivePeriod().properties().duration().duration().c_str(), "NULL")) &&
		status)
	{
		responseCode = "451";
		responseDescription = "Not Allowed";
		responsemsg = "Event Duration is Invalid";
		status = false;
	}

	// if eiTarget is defined, then check for Marketcontext and VEN validity
	if(((itr->eiEvent().eiTarget().venID().size()) ||
		(itr->eiEvent().eiTarget().partyID().size()) ||
		(itr->eiEvent().eiTarget().groupID().size()) ||
		(itr->eiEvent().eiTarget().resourceID().size())) && status)
	{
		EiTargetType *venResponse = &itr->eiEvent().eiTarget();
		status = false;
		//Checking for VENID in target..
		oadr2b::ei::EiTargetType::venID_iterator venItr;
		for(venItr=venResponse->venID().begin(); venItr!=venResponse->venID().end();venItr++)
			status = status || validateVENID(venItr->data());

		//Checking for MarketcontextID in target..
		oadr2b::ei::EiTargetType::partyID_iterator partyItr;
		for(partyItr=venResponse->partyID().begin(); partyItr!=venResponse->partyID().end();partyItr++)
			status = status || validatepartyID(vm->getVenName(),partyItr->data());
		//TODO: GroupID, ResourceID validation
		if(!status)
		{
			responseCode = "452";
			responseDescription = "Invalid ID";
			responsemsg = "Event Target is Invalid";
		}
	}

	//check for overlapping events..
	if(status)
	{
		if(*previousDuration) //if previous event Duration is zero, all the events following will be invalid
		{
			if(currentEvent>*previousEvent)
			{
				//assign current event's end time to previousEvent, for checking next event
				*previousEvent = currentEvent + (duration*60);
				if(!duration)
					*previousDuration = false;
			}
			else
			{
				responseCode = "450";
				responseDescription = "Out of sequence";
				responsemsg = "Event is overlapping with another event";
				status = false;
			}
		}
		else
		{
			responseCode = "450";
			responseDescription = "Out of sequence";
			responsemsg = "Event is overlapping with another event";
			status = false;
		}
	}

	//to validate signals.. not yet complete!!!
	status &= validateSignals(itr ,&responseCode, &responseDescription, &responsemsg);

	if((!(status)) && (ifEventidexists(itr->eiEvent().eventDescriptor().eventID())))
	{
		//Erase that map entry
		clearDB(itr->eiEvent().eventDescriptor().eventID());
		if(eventActiveSignalCheck(time(0), currentEvent, duration))
			vm->waitNotifier("SIGNAL");
	}

	if(status)
	{
		//to notify the signals, if the an active signal, has future time in new payload
		if((ifEventidexists(itr->eiEvent().eventDescriptor().eventID())))
		{
			timeIterator = eventiddtstartmap.find(itr->eiEvent().eventDescriptor().eventID());
			uintIterator = eventiddurationmap.find(itr->eiEvent().eventDescriptor().eventID());
			if(eventActiveSignalCheck(time(0), timeIterator->second, uintIterator->second))
			{
				if(currentEvent != timeIterator->second)
					vm->waitNotifier("SIGNAL");
			}
		}
		//to notify active signals which are cancelled.
		if(!(strcmp(itr->eiEvent().eventDescriptor().eventStatus().c_str(),"cancelled")))
		{
			if(eventActiveSignalCheck(time(0), currentEvent, duration))
				vm->waitNotifier("SIGNAL");
			clearDB(itr->eiEvent().eventDescriptor().eventID());
			status = false;
			responsemsg = "Event is cancelled";
		}
		else
		{
			string optstatus="opt-in";
			string optID;
			setOptOut(false);
			string eventStatus = "Pending";

			// Update the map(Eventid, modification no, duration, status)
			if(ifEventidexists(itr->eiEvent().eventDescriptor().eventID()))
			{
				if(eventActiveSignalCheck(time(0), currentEvent, duration))
					eventStatus = "Active";

				map<string,string>::iterator sIterator;
				sIterator = eventidoptidmap.find(itr->eiEvent().eventDescriptor().eventID());
				if(sIterator != eventidoptidmap.end())
					optID = sIterator->second;

				stringIterator = eventidoptstatusmap.find(itr->eiEvent().eventDescriptor().eventID());

				if(stringIterator->second.compare("opt-out_hmi")==0)
				{
					optstatus = "opt-out_hmi";
					setOptOut(true);
				}

				if(stringIterator->second.compare("opt-out_sch")==0)
				{
					timeIterator = optIDStartTimemap.find(optID);
					if(timeIterator != optIDStartTimemap.end())
					{
						optstatus = "opt-out_sch";
						setOptOut(true);

					}
					else
					{
						optstatus = "opt-in";
						setOptOut(false);
					}

				}
				//to make sure that the event with start time changed in next payload, need to chk opt status
				timeIterator = eventiddtstartmap.find(itr->eiEvent().eventDescriptor().eventID());
				if((stringIterator->second.compare("opt-in")==0) || (currentEvent != timeIterator->second))
				{
					setOptOut(optOutStatus(currentEvent, duration));
					if(isOptOut())
					{
						optstatus = "opt-out_sch";
						optID = getOptId();
					}
					else
					{
						optstatus = "opt-in";
						setOptOut(false);
					}
				}
				clearDB(itr->eiEvent().eventDescriptor().eventID());
			}
			else
			{
				setOptOut(optOutStatus(currentEvent, duration));
				if(isOptOut())
				{
					optstatus = "opt-out_sch";
					optID = getOptId();
				}
			}

			insertDB(itr->eiEvent().eventDescriptor().eventID(),
			     	 itr->eiEvent().eventDescriptor().modificationNumber(),
					 durationExtract(itr->eiEvent().eiActivePeriod().properties().duration().duration()),
				 	 currentEvent,eventStatus, reqID,optstatus,(isOptOut())?optID:"");
		}
	}
	stringstream temp;
	temp<<itr->eiEvent().eventDescriptor().modificationNumber();

	log.logger("======================================================", ucmlogging::Info);
	log.logger("Event ID -" + itr->eiEvent().eventDescriptor().eventID(), ucmlogging::Info);
	log.logger("modification no - " + temp.str(), ucmlogging::Info);
	log.logger("Start Time - " + string(asctime(localtime(&currentEvent))), ucmlogging::Info);
	log.logger("Duration - " + itr->eiEvent().eiActivePeriod().properties().duration().duration(), ucmlogging::Info);
	log.logger("Valid - " + string((status) ? "Yes" : "No"), ucmlogging::Info);
	log.logger("Response Code - " + responseCode, ucmlogging::Info);
	log.logger("Response Description - " + responsemsg, ucmlogging::Info);
	log.logger("======================================================", ucmlogging::Info);

	if(!status)
		log.logInvalidEventReport(itr->eiEvent().eventDescriptor().eventID(), responsemsg);

	return status;
}

/**
    Sends oadrCreatedevent response, updates eventdetails.db file with valid events and starts the event thread
    @param pointer to class VEN2b
    @param pointer to class VENManager
    @param oadrDistributeEvent payload
    @param marketContext - marketContext map from main
    @param condition variable for notifying
    @param notifierP - location where notify context is checked
    @return - status
*/
bool EventHandler::eventSchedular(VEN2b *venCreatedEvent, VENManager *vm,
								  oadrDistributeEventType response,
		 	 	 	 	 	 	  condition_variable *cvEvent, condition_variable cvSignal[], char *notifierP)
{
	ucmlogging log;
	sqliteDB sdb;
	getdata();//scan and get data from the file...

	//Application level validation done here....
	if(!validateVTNID(response.vtnID()))
	{
		log.logger("Invalid VTN ID!!!", ucmlogging::Warning);
		//TODO need to add error response here....
	}

	oadrDistributeEventType::oadrEvent_iterator itr;
	EventResponses er;
	if (response.oadrEvent().size())
	{
		time_t previousEvent=0;
		bool previousDuration = true;
		//to create DB and validate events
		for (itr = response.oadrEvent().begin(); itr != response.oadrEvent().end(); itr++)
		{
			validateEvent(vm, response.requestID(), itr, &previousEvent ,&previousDuration);
			if(!(itr->oadrResponseRequired().compare("always")))
			{
				if(isOptOut())
					er.addEventResponse(responseCode, responseDescription, itr->eiEvent().eventDescriptor().eventID(), itr->eiEvent().eventDescriptor().modificationNumber(),
						OptTypeType::optOut, response.requestID());
				else
					er.addEventResponse(responseCode, responseDescription, itr->eiEvent().eventDescriptor().eventID(), itr->eiEvent().eventDescriptor().modificationNumber(),
							OptTypeType::optIn, response.requestID());
				setOptOut(false);
			}
		}
	}
	map <string,int> tempmap;
	tempmap.insert(eventidmodmap.begin(), eventidmodmap.end());

	//to check for known events, which are not present in the oadrDistribute payload (implied cancel)
	for (intIterator=tempmap.begin(); intIterator!=tempmap.end(); ++intIterator)
	{
		bool eventFound = false;
		for (itr = response.oadrEvent().begin(); itr != response.oadrEvent().end(); itr++)
		{
			if(!(strcmp(itr->eiEvent().eventDescriptor().eventID().c_str(),intIterator->first.c_str())))
			{
				eventFound = true;
			}
		}
		if(!eventFound)
		{
			//notify signals, if deleted event is having active signals.
			map<string,unsigned int>::iterator intiterator;
			timeIterator = eventiddtstartmap.find(intIterator->first);
			intiterator = eventiddurationmap.find(intIterator->first);
			if(eventActiveSignalCheck(time(0), timeIterator->second, intiterator->second))
				vm->waitNotifier("SIGNAL");
			//clear that event from list.
			clearDB(intIterator->first);
		}
	}
	try
	{
		log.logger("Sending oadrCreatedEvent message" ,ucmlogging::Info);
		auto_ptr<CreatedEvent> ce = venCreatedEvent->createdEvent("200", "OK", er);

		xmlFileCreation requestLog("oadrCreatedEvent.xml", ce->requestBody());
		xmlFileCreation responseLog("oadrResponse.xml", ce->responseBody());
		oadrPayload *payload = ce->response();

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
		log.logger("Caught exception: " + string(ex.what()) + "!!!Failed to send oadrCreatedEvent!!!",ucmlogging::Error);
	}

	//writing the details of valid events to eventdetails.data file.
	log.logger("Valid Events - >>", ucmlogging::Info);
	remove(eventdetails);
	for (intIterator=eventidmodmap.begin(); intIterator!=eventidmodmap.end(); ++intIterator)
	{
		string reqID = "";
		string eventstatus = "Pending";
		log.logger(intIterator->first, ucmlogging::Info);
		map<string,unsigned int>::iterator intiterator;
		map<string,string>::iterator reqIDIterator;
		map<string,string>::iterator optIDIterator;
		timeIterator = eventiddtstartmap.find(intIterator->first);
		intiterator = eventiddurationmap.find(intIterator->first);
		stringIterator = eventidoptstatusmap.find(intIterator->first);
		reqIDIterator = eventidreqidmap.find(intIterator->first);
		optIDIterator = eventidoptidmap.find(intIterator->first);

		if(eventActiveSignalCheck(time(0), timeIterator->second,intiterator->second))
			eventstatus = "Active";

		for (itr = response.oadrEvent().begin(); itr != response.oadrEvent().end(); itr++)
		{
			if(itr->eiEvent().eventDescriptor().eventID().compare(intIterator->first) == 0)
			{
				if(!(itr->oadrResponseRequired().compare("always")))
					reqID = reqIDIterator->second;
			}
		}
		sdb.insertDBeventdetails(reqID,
								 intIterator->first,
								 intIterator->second,
								 timeIterator->second,
								 intiterator->second,
								 eventstatus,
								 stringIterator->second,
								 optIDIterator->second);
	}

	// if any valid event present, start the Eventthread.
	if(eventidmodmap.size())
	{
		EventHandler eh(getVenId(), getVtnId());
		thread Event_Thread(&EventHandler::eventThread, &eh, response , venCreatedEvent, getEventiddtstartmap(), getEventiddurationmap(), cvEvent, cvSignal, notifierP);
		Event_Thread.detach();
	}
	return true;
}

/**
    event handler thread - wait until the event start time and spawns signal thread.
    @param oadrDistributeEvent payload
    @param eventiddtstartmap
    @param eventiddurationmap
    @param condition variable for notifying
    @param notifierP - location where notify context is checked
    @return - nothing
*/
void EventHandler::eventThread(oadrDistributeEventType payload, VEN2b *venCreatedEvent,
							   map <string,time_t> eventiddtstartmap,
							   map <string,unsigned int> eventiddurationmap,
							   condition_variable *cvEvent, condition_variable cvSignal[], char *notifierP)
{
	sqliteDB sdb;
	sdb.sqliteDBwriteInt("eventthread",1, thread_status, "threadstatus");
	eiEventSignalsType EventSignalresponse;
	SignalHandler SH;
	mutex cv_Event;
	unique_lock<mutex> lkEvent(cv_Event);
	ucmlogging log;

	vector<thread> signalThreads;
	unsigned int signalCount;

	notifierEvent = notifierP;
	strcpy(notifierEventVal,"EVENT");

	oadrDistributeEventType::oadrEvent_iterator itr;
	EventResponses er;

	log.logger("**********Event Thread Started****************", ucmlogging::Info);

	for (itr = payload.oadrEvent().begin();itr != payload.oadrEvent().end(); itr++)
	{
		time_t currentEvent;

		timeIterator = eventiddtstartmap.find(itr->eiEvent().eventDescriptor().eventID());
		uintIterator = eventiddurationmap.find(itr->eiEvent().eventDescriptor().eventID());

		if((timeIterator != eventiddtstartmap.end()) && //to check if event exist..
		  (!(eventActiveSignalCheck(time(0), timeIterator->second, uintIterator->second))))
		{
			//get event's dtstart time
			currentEvent =  getEventTime(itr);
			EventSignalresponse = itr->eiEvent().eiEventSignals();

			log.logger(itr->eiEvent().eventDescriptor().eventID() + "  Waiting for Event dtstart time - " + asctime(localtime(&currentEvent)), ucmlogging::Info);
			if(cvEvent->wait_until(lkEvent, chrono::system_clock::from_time_t(currentEvent),[](){return (!strcmp(notifierEvent,notifierEventVal));}))
			{
				log.logger("Notify Received for Event!!! Stopping Events thread!!!", ucmlogging::Info);
				goto fin;
			}
			else
			{
				int customerOverride;
				customerOverride = sdb.sqliteDBreadInt("customeroverride", hmiven, "hmiparam");
				bool optstate=true;
				EventHandler eh;
				eh.getdata();
				stringIterator = eh.eventidoptstatusmap.find(itr->eiEvent().eventDescriptor().eventID());
				if(stringIterator != eh.eventidoptstatusmap.end())
				{
					if(stringIterator->second.compare("opt-in")==0)
						optstate = true;
					else
						optstate = false;
				}
				if((customerOverride==0) && optstate)
				{
					log.logger(itr->eiEvent().eventDescriptor().eventID() + " - Wait Timeout!!! ", ucmlogging::Info);
					log.logger("Starting signals for Event - " + itr->eiEvent().eventDescriptor().eventID(), ucmlogging::Info);
					//spawn Signalthreads
					stringstream temp;
					temp<<EventSignalresponse.eiEventSignal().size();
					log.logger("No of Signals - " + temp.str(), ucmlogging::Info);
					//Storing the number of Signals present (in the event) in signalThread variable. Used while notifying (cancelling) the Signalthread
					sdb.sqliteDBwriteInt("signalthread",EventSignalresponse.eiEventSignal().size(), thread_status, "threadstatus");
					eiEventSignalsType::eiEventSignal_iterator sigItr = EventSignalresponse.eiEventSignal().begin();
					remove(signaldetails);
					for(signalCount=1;((signalCount<=EventSignalresponse.eiEventSignal().size()) && ((signalCount-1)<=maxsignal));signalCount++)
					{
						signalThreads.push_back(thread(&SignalHandler::eventSignalThread, &SH, EventSignalresponse, signalCount, &cvSignal[signalCount-1], notifierP));
						if(!(sdb.insertDBsignaldetails(sigItr->signalID(),(signalCount-1))))
							log.logger("signaldetails.db update failed for Signal - " + sigItr->signalID(), ucmlogging::Error);
						sigItr++;
					}
					sleep(1);
					EventHandler eh;
					eh.updateEventDetails();
				}
				else
				{
					if((customerOverride==1) && (!(itr->oadrResponseRequired().compare("always"))))
					{
						EventResponses er;
						VENManager venm;
						er.addEventResponse("200", "OK", itr->eiEvent().eventDescriptor().eventID(), itr->eiEvent().eventDescriptor().modificationNumber(),
												OptTypeType::optOut, payload.requestID());
						if(venm.sendCreatedEvent(venCreatedEvent,er))
							log.logger("Opt-out sent successfully for Event - " + itr->eiEvent().eventDescriptor().eventID(), ucmlogging::Info);
						else
							log.logger("Opt-out failed for Event - " + itr->eiEvent().eventDescriptor().eventID(), ucmlogging::Error);
					}
					sdb.sqliteDBdeletestring("eventdetails", eventdetails, "eventid", itr->eiEvent().eventDescriptor().eventID());
				}
			}
		}
	} //end of for loop -event itr

	fin:
	sdb.sqliteDBwriteInt("eventthread",0, thread_status, "threadstatus");
	for(auto& thread : signalThreads)
		thread.detach();
	log.logger("----------------Exit from Event Thread----------------------", ucmlogging::Info);
} // end of event handler function

