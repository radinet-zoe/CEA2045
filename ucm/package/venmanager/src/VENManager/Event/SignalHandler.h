/*
 * SignalHandler.h
 *
 *  Created on: 28-Aug-2014
 *      Author: dinesh
 */

#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_

#include <oadr/request/RequestEvent.h>
#include "../Main.h"
#include "../UCMLogging.h"

using namespace std;


class SignalHandler
{
private:

public:
	void eventSignalThread(eiEventSignalsType payload, unsigned int signalCount, condition_variable *cv, char *notifierP);
	bool updateSignalData(string signalID);
};

#endif /* SIGNALHANDLER_H_ */
