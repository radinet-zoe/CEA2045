/*
 * PollHandler.h
 *
 *  Created on: 01-Sep-2014
 *      Author: dinesh
 */

#ifndef POLLHANDLER_H_
#define POLLHANDLER_H_

#include "Main.h"
class VENManager;
class pollHandler
{
private:
	string m_venID;
	string m_registrationID;
	string m_vtnID;
	unsigned int pollFreq;
	unsigned int pollExit;

public:

	pollHandler(string m_venID = "",
			    string m_registrationID="",
			    string m_vtnID="",
			    unsigned int pollFreq=10);

	bool eventDateCheck(time_t current, time_t event);

	void startPolling(VEN2b *venPoll, VENManager *vm, condition_variable *cvPoll, condition_variable *cvEvent, condition_variable cvSignal[], condition_variable cvReport[],char *notifierP);

	const string& getRegistrationId() const {
		return m_registrationID;
	}

	void setRegistrationId(const string& registrationId) {
		m_registrationID = registrationId;
	}

	const string& getVenId() const {
		return m_venID;
	}

	void setVenId(const string& venId) {
		m_venID = venId;
	}

	unsigned int getPollFreq() const {
		return pollFreq;
	}

	void setPollFreq(unsigned int pollFreq) {
		this->pollFreq = pollFreq;
	}

	const string& getVtnId() const {
		return m_vtnID;
	}

	void setVtnId(const string& vtnId) {
		m_vtnID = vtnId;
	}

};


#endif /* POLLHANDLER_H_ */
