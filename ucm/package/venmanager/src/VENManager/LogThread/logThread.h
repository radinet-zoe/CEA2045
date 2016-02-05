/*
 * logThread.h
 *
 *  Created on: 06-Nov-2014
 *      Author: dinesh
 */

#ifndef LOGTHREAD_H_
#define LOGTHREAD_H_


#include "../Main.h"

class logThread
{
private:
	unsigned int logFreq;
	unsigned int logExit;

public:

	logThread(unsigned int logFreq=1);

	void startLog(condition_variable *cvLog, char *notifierP);

	unsigned int getLogExit() const {
		return logExit;
	}

	void setLogExit(unsigned int logExit) {
		this->logExit = logExit;
	}

	unsigned int getLogFreq() const {
		return logFreq;
	}

	void setLogFreq(unsigned int logFreq) {
		this->logFreq = logFreq;
	}
};



#endif /* LOGTHREAD_H_ */
