/*
 * UCMLogging.h
 *
 *  Created on: 05-Sep-2014
 *      Author: dinesh
 */

#ifndef UCMLOGGING_H_
#define UCMLOGGING_H_

#include "Main.h"

using namespace std;

class ucmlogging
{

public:
	enum msgType
	{
		Info,
		Warning,
		Error
	};

	ucmlogging();

	~ucmlogging();

	map<ucmlogging::msgType, string> msgTypeMap;

	map<ucmlogging::msgType, string>::iterator stringIterator;

	void logger(string message, ucmlogging::msgType msgStatus);

	void logInvalidEventReport(string ID, string Reason);
};


#endif /* UCMLOGGING_H_ */
