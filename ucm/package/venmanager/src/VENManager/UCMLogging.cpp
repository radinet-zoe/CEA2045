/*
 * UCMLogging.cpp
 *
 *  Created on: 05-Sep-2014
 *      Author: dinesh
 */


#include "UCMLogging.h"
#include <iomanip>

/**
  Constructor for ucmlogging
 */
ucmlogging::ucmlogging()
{
	msgTypeMap.insert(pair<ucmlogging::msgType, string>(ucmlogging::Info,"Info"));
	msgTypeMap.insert(pair<ucmlogging::msgType, string>(ucmlogging::Warning,"Warning"));
	msgTypeMap.insert(pair<ucmlogging::msgType, string>(ucmlogging::Error,"Error"));
}

/**
  Destructor for ucmlogging
 */
ucmlogging::~ucmlogging()
{
	msgTypeMap.clear();
}

/**
    Method to log info to log file
    @param message - here log message is specified
    @param status - here status("info", "warning", "error") is specified
*/
void ucmlogging::logger(string message, ucmlogging::msgType msgStatus)
{
	time_t t = time(0);   // get time now
	ofstream myfile;
	string time = string(asctime(localtime(&t)));

	ifstream file( ucmLogPath, ios::binary | ios::ate);
	float length = file.tellg();
	length = length / (1024*1024);
	file.close();

	time.erase(remove(time.begin(), time.end(), '\n'), time.end());
	message.erase(remove(message.begin(), message.end(), '\n'), message.end());

 	myfile.open (ucmLogPath, ios_base::app);

 	myfile << "[" << time << "]";

 	stringIterator = msgTypeMap.find(msgStatus);
	myfile << setfill (' ') << setw (7) << stringIterator->second << ": ";

	myfile << message <<endl;
	myfile.close();

}

/**
    Method to log invalidReport/Events
    @param ID - here event/report ID is specified
    @param Reason - here Reason is specified
*/
void ucmlogging::logInvalidEventReport(string ID, string Reason)
{
	time_t t = time(0);   // get time now
	ofstream myfile;
	string time = string(asctime(localtime(&t)));

	time.erase(remove(time.begin(), time.end(), '\n'), time.end());

 	myfile.open (InvalidReportEvent, ios_base::app);
 	myfile << "[" << time << "] - ";

 	myfile << ID << " - ";
 	myfile << Reason << endl;
	myfile.close();

}

