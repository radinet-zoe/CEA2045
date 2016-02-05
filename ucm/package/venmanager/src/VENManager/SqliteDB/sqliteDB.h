/*
 * sqliteDB.h
 *
 *  Created on: 26-Sep-2014
 *      Author: dinesh
 */

#ifndef SQLITEDB_H_
#define SQLITEDB_H_

#include "../Main.h"

using namespace std;

class sqliteDB
{
public:

	 bool sqliteDBquery(const char *queryMessage, string path);

	 bool sqliteDBCreate(void);

	 string removeIllegalchars(string content);

	 bool sqliteDBwriteString(string variable, string value,  string path, string tableName);

	 bool sqliteDBwriteStringWithCond(string variable, string value, string path, string tableName,
	 		     				      string condvariable, string condvalue);

	 bool sqliteDBwriteInt(string variable, int intValue, string path, string tableName);

	 string sqliteDBreadString(string variable, string path, string tableName);

	 int sqliteDBreadInt(string variable, string path, string tableName);

	 bool sqliteDBdeletestring(string tableName, string path, string variable,string value);

	 bool insertDBsignaldetails(string signalID, unsigned int cvIndex);

	 void readDBsignaldetails(map <string,unsigned int> *signalcvindexmap);

	 bool insertDBeventdetails(string requestid, string eventid, int modificationno,
			 	 	 	 	   time_t dtstart, unsigned int  duration, string status,
			 	 	 	 	   string optstatus, string optid);

	 void readDBeventdetails(map <string,int> *eventidmodmap,
			 	 	 	 	 map <string,time_t> *eventiddtstartmap,
			 	 	 	 	 map <string,unsigned int> *eventiddurationmap,
			 	 	 	 	 map <string,string> *eventidstatusmap,
			 	 	 	 	 map <string,string> *eventidoptstatusmap,
			 	 	 	 	 map <string,string> *eventidreqidmap,
			 	 	 	 	 map <string,string> *eventidoptidmap);

	 bool insertDBreportdetails(string reportrequestid, string reportspecifierid,
	 							string reporttype, string granularity,
	 							string reportbackdur, time_t dtstart,
	 							string  duration, unsigned int cvindex);

	 void readDBreportdetails(map<string, string> *rrid_rsidMap,
	 								    map<string, string> *rrid_reportTypeMap,
	 								    map<string, string> *rrid_granMap,
	 								    map<string, string> *rrid_rbdurationMap,
	 								    map<string, string> *rrid_durMap,
	 								    map<string, time_t> *rrid_startMap,
	 								    map<string, unsigned int> *rrid_cvMap);

	 bool insertDBoptschedules(string startTime, string stopTime,string Reason, string optID);

	 void readDBoptschedules(map <string,time_t> *optIDStartTimemap,
			 	 	 	 	 map <string,time_t> *optIDStopTimemap);


	 map<string,string>::iterator sIterator;
};



#endif /* SQLITEDB_H_ */
