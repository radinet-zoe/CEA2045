/*
 * EventHandler.h
 *
 *  Created on: 27-Aug-2014
 *      Author: dinesh
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include "../Main.h"
#include <oadr/request/RequestEvent.h>
#include <oadr/ven/VEN2b.h>
#include "SignalHandler.h"

class VENManager;
class EventHandler
{
private:
	string m_venID;
	string m_vtnID;
	string responseCode;
	string responseDescription;
	string m_optID;
	bool optOut;


public:

	map <string,int> eventidmodmap;
	map <string,time_t> eventiddtstartmap;
	map <string,unsigned int> eventiddurationmap;
	map <string,unsigned int> signalcvindexmap;
	map <string,string> eventidstatusmap;
	map <string,string> eventidoptstatusmap;
	map <string,string> eventidreqidmap;
	map <string,string> eventidoptidmap;

	map <string,time_t> optIDStartTimemap;
	map <string,time_t> optIDStopTimemap;

	map<string,int>::iterator intIterator;
	map<string,unsigned int>::iterator uintIterator;
	map<string,time_t>::iterator timeIterator;
	map<string,string>::iterator stringIterator;

	EventHandler(string m_venID="", string m_vtnID="");

	void eventThread(oadrDistributeEventType payload,VEN2b *venCreatedEvent,
				     map <string,time_t> eventiddtstartmap,
				     map <string,unsigned int> eventiddurationmap,
				     condition_variable *cvEvent, condition_variable cvSignal[], char *notifierP);

	bool eventSchedular(VEN2b *venCreatedEvent, VENManager *vm, oadrDistributeEventType payload,
					    condition_variable *cvEvent, condition_variable cvSignal[], char *notifierP);

	bool eventActiveSignalCheck(time_t current, time_t event, unsigned int duration);

	void getsignalindexmap(void);

	void updateEventDetails ();

	bool updateEventDetailsOptstatus(string optID);

	void getdata();

	bool validateEvent(VENManager *vm, string reqID, oadrDistributeEventType::oadrEvent_iterator itr,
				       time_t *previousEvent, bool *previousDuration);

	bool ifEventidexists(string eventID);

	bool optOutStatus(time_t eventTime, unsigned int Duration);

	void insertDB(string eventID, int modificationNumber, unsigned int Duration, time_t dtstart, string eStatus,
				  string erequestID, string optStatus, string optID);

	void clearDB(string eventID);

	bool eventDateCheck(time_t current, time_t event);

	bool validateSignals(oadrDistributeEventType::oadrEvent_iterator singleEvent, string *responseCode,
						 string *responseDescription, string *responsemsg);

	bool validatePayloadFloatValues(intervals *signalIntervals, string valueType, string values, string *responsemsg, string signalName);

	bool validateVTNID(string VTNID);

	bool validateVENID(string VENID);

	bool validatepartyID(string VEN_Name, string PartyID);

	bool validatemodificationNo(string eventID, int modificationNo);

	unsigned int durationExtract (string duration);

	time_t getEventTime(oadrDistributeEventType::oadrEvent_iterator itr);



	const string& getVtnId() const {
		return m_vtnID;
	}

	void setVtnId(const string& vtnId) {
		m_vtnID = vtnId;
	}

	bool isOptOut() const {
		return optOut;
	}

	void setOptOut(bool optOut) {
		this->optOut = optOut;
	}

	const string& getResponseCode() const {
		return responseCode;
	}

	void setResponseCode(const string& responseCode) {
		this->responseCode = responseCode;
	}

	const string& getResponseDescription() const {
		return responseDescription;
	}

	void setResponseDescription(const string& responseDescription) {
		this->responseDescription = responseDescription;
	}

	const map<string, time_t>& getEventiddtstartmap() const {
		return eventiddtstartmap;
	}

	void setEventiddtstartmap(const map<string, time_t>& eventiddtstartmap) {
		this->eventiddtstartmap = eventiddtstartmap;
	}

	const map<string, unsigned int>& getEventiddurationmap() const {
		return eventiddurationmap;
	}

	void setEventiddurationmap(
			const map<string, unsigned int>& eventiddurationmap) {
		this->eventiddurationmap = eventiddurationmap;
	}

	const map<string, int>& getEventidmodmap() const {
		return eventidmodmap;
	}

	void setEventidmodmap(const map<string, int>& eventidmodmap) {
		this->eventidmodmap = eventidmodmap;
	}

	const string& getVenId() const {
		return m_venID;
	}

	void setVenId(const string& venId) {
		m_venID = venId;
	}

	const string& getOptId() const {
		return m_optID;
	}

	void setOptId(const string& optId) {
		m_optID = optId;
	}
};

#endif /* EVENTHANDLER_H_ */
