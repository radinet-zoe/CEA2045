/*
 * sqliteDB.cpp
 *
 *  Created on: 26-Sep-2014
 *      Author: dinesh
 */


#include "sqliteDB.h"
#include <sqlite3.h>

map <string,string> DBVariablemap;

//for signaldetails map
map <string,unsigned int> *signalindexmapPntr;

//for eventdetails  map
map <string,int> *eventidmodmapPntr;
map <string,time_t> *eventiddtstartmapPntr;
map <string,unsigned int> *eventiddurationmapPntr;
map <string,string> *eventidstatusmapPntr;
map <string,string> *eventidoptstatusmapPntr;
map <string,string> *eventidreqidmapPntr;
map <string,string> *eventidoptidmapPntr;

//for reportdetails map
map<string, string> *rrid_rsidMapPntr;
map<string, string> *rrid_reportTypeMapPntr;
map<string, string> *rrid_granMapPntr;
map<string, string> *rrid_rbdurationMapPntr;
map<string, string> *rrid_durMapPntr;
map<string, time_t> *rrid_startMapPntr;
map<string, unsigned int> *rrid_cvMapPntr;

//for optschedules map
map <string,time_t> *optIDStartTimemapPntr;
map <string,time_t> *optIDStopTimemapPntr;

/**
    Method to hold values from sqlite database during query
    @param *data - parameter passed during query. Used for sending variable to be read.
    @param argc - no of variables in database
    @param argv - Values in database
    @param  azColName - column name in database
    @return - status
*/

static int DBRead(void *data, int argc, char **argv, char **azColName)
{
	int i;
	string &variable = *static_cast<string*>(data);
	for(i=0; i<argc; i++)
	{
		if(!(strcmp(variable.c_str(),azColName[i])))
			DBVariablemap.insert (pair<string,string>(azColName[i],argv[i] ? argv[i] : ""));
	}
	return 0;
}

/**
    Method to hold values from signal details sqlite database during query
    @param *data - parameter passed during query. Not Used.
    @param argc - no of variables in database
    @param argv - Values in database
    @param  azColName - column name in database
    @return - status
*/

static int DBReadsignalindex(void *data, int argc, char **argv, char **azColName)
{
	signalindexmapPntr->insert (pair<string,unsigned int>(argv[0], atoi(argv[1])));
	return 0;
}

/**
Method to hold values from event details sqlite database during query
@param *data - parameter passed during query. Not Used.
@param argc - no of variables in database
@param argv - Values in database
@param  azColName - column name in database
@return - status
*/
static int DBReadeventdetails(void *data, int argc, char **argv, char **azColName)
{
	stringstream starttime;
	time_t dtstart;
	starttime << string(argv[3]);
	starttime >> dtstart;

	eventidmodmapPntr->insert (pair<string,unsigned int>(argv[1],atoi(argv[2])));
	eventiddtstartmapPntr->insert (pair<string,time_t>(argv[1],dtstart));
	eventiddurationmapPntr->insert (pair<string,unsigned int>(argv[1],atoi(argv[4])));
	eventidstatusmapPntr->insert (pair<string,string>(argv[1],argv[5]));
	eventidoptstatusmapPntr->insert (pair<string,string>(argv[1],argv[6]));
	eventidreqidmapPntr->insert (pair<string,string>(argv[1],argv[0]));
	eventidoptidmapPntr->insert (pair<string,string>(argv[1],argv[7]));
	return 0;
}

/**
Method to hold values from report details sqlite database during query
@param *data - parameter passed during query. Not Used.
@param argc - no of variables in database
@param argv - Values in database
@param  azColName - column name in database
@return - status
*/
static int DBReadreportdetails(void *data, int argc, char **argv, char **azColName)
{
	stringstream starttime;
	time_t dtstart;
	starttime << string(argv[5]);
	starttime >> dtstart;

	rrid_rsidMapPntr->insert (pair<string,string>(argv[0],argv[1]));
	rrid_reportTypeMapPntr->insert (pair<string,string>(argv[0],argv[2]));
	rrid_granMapPntr->insert (pair<string,string>(argv[0],argv[3]));
	rrid_rbdurationMapPntr->insert (pair<string,string>(argv[0],argv[4]));
	rrid_durMapPntr->insert (pair<string,string>(argv[0],argv[6]));
	rrid_startMapPntr->insert (pair<string,time_t>(argv[0],dtstart));
	rrid_cvMapPntr->insert (pair<string,unsigned int>(argv[0],atoi(argv[7])));
	return 0;
}

/**
Method to hold values from optschedules sqlite database during query
@param *data - parameter passed during query. Not Used.
@param argc - no of variables in database
@param argv - Values in database
@param  azColName - column name in database
@return - status
*/
static int DBReadoptschedule(void *data, int argc, char **argv, char **azColName)
{
	stringstream stream;
	time_t starttime, endtime;

	stream << string(argv[0]);
	stream >> starttime;
	stream.clear();
	stream << string(argv[1]);
	stream >> endtime;

	optIDStartTimemapPntr->insert (pair<string,time_t>(argv[3],starttime));
	optIDStopTimemapPntr->insert (pair<string,time_t>(argv[3],endtime));
	return 0;
}

/**
    Method to query sqlite database
    @param queryMessage - database query message
    @return - status
*/

bool sqliteDB::sqliteDBquery(const char *queryMessage, string path)
{
	ucmlogging log;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open(path.c_str(), &db);

	if(rc)
	{
		log.logger("Can't open database: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return false;
	}
	sqlite3_busy_timeout(db,3000); //1s of wait time for busy
	rc = sqlite3_exec(db, queryMessage, 0, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
		return false;
	}

	sqlite3_close(db);
	return true;
}

/**
    Method to create and add default values to sqlite database
    @return - status
*/

bool sqliteDB::sqliteDBCreate(void)
{
	ucmlogging log;
	const char *sql;
	struct stat st = {0};
	bool retStatus=true;

	//VEN_PARAMr. written from c++ side and read by HMI
	/* Create SQL statement */
	sql = "CREATE TABLE venparam("  \
			"vtnid 				VARCHAR," \
			"registrationid		VARCHAR," \
			"venid				VARCHAR," \
			"vtnconnstatus		BOOLEAN	DEFAULT 0," \
			"cea2045status		BOOLEAN	DEFAULT 0);";

	if(sqliteDBquery(sql, ven_param))
	{
		log.logger("Table venparam Created Successfully!!!", ucmlogging::Info);
		sql = "INSERT INTO venparam (vtnid,registrationid,venid,vtnconnstatus,cea2045status) "  \
				"VALUES ('', '', '', 0, 0);";
		if(sqliteDBquery(sql, ven_param))
			log.logger("Table venparam default values added Successfully!!!", ucmlogging::Info);
		else
			retStatus= false;
	}
	else
		retStatus= false;

	//Thread status
	/* Create SQL statement */
	sql = "CREATE TABLE threadstatus("  \
			"pollthread 		INTEGER		DEFAULT 0," \
			"eventthread 		INTEGER		DEFAULT 0," \
			"signalthread 		INTEGER		DEFAULT 0," \
			"logthread	 		INTEGER		DEFAULT 0," \
			"reportthread 		INTEGER		DEFAULT 0);";

	if(sqliteDBquery(sql, thread_status))
	{
		log.logger("Table threadstatus Created Successfully!!!", ucmlogging::Info);
		 //Create SQL statement
		sql = "INSERT INTO threadstatus (pollthread,eventthread,signalthread,logthread,reportthread) "  \
				"VALUES (0, 0, 0, 0, 0 );";
		if(sqliteDBquery(sql, thread_status))
			log.logger("Table threadstatus default values added Successfully!!!", ucmlogging::Info);
		else
			retStatus= false;
	}
	else
		retStatus= false;
	if ((stat(hmiven, &st) == -1))
	{
		//HMIVEN_FLAG
		/* Create SQL statement */
		sql = "CREATE TABLE hmivenflag("  \
				"flag 		INTEGER		DEFAULT 0," \
				"errormsg 		TEXT);";

		if(sqliteDBquery(sql, hmiven))
		{
			log.logger("Table hmivenflag Created Successfully!!!", ucmlogging::Info);
			sql = "INSERT INTO hmivenflag (flag,errormsg) "  \
					"VALUES (0, '' );";
			if(sqliteDBquery(sql, hmiven))
				log.logger("Table hmivenflag default values added Successfully!!!", ucmlogging::Info);
			else
				retStatus= false;
		}
		else
			retStatus= false;

		//HMI Parameter. written from HMI side and read by C++
		/* Create SQL statement */
		sql = "CREATE TABLE hmiparam("  \
				"oadrusername 		VARCHAR," \
				"oadrpassword 		VARCHAR," \
				"venname 			VARCHAR," \
				"vtnurl		 		VARCHAR," \
				"pollfreq	 		INTEGER		DEFAULT 30," \
				"ssltls		 		BOOLEAN," \
				"enableven 			BOOLEAN," \
				"drprogram		 	TEXT," \
				"customeroverride	BOOLEAN," \
				"minsampling 		INTEGER		DEFAULT 5," \
				"maxsampling 		INTEGER		DEFAULT 5," \
				"vtnusername		VARCHAR," \
				"vtnpassword	 	VARCHAR," \
				"ftpslogurl			VARCHAR," \
				"ftpslogging		INTEGER		DEFAULT 10," \
				"upgradeurl			VARCHAR," \
				"autoupgrade		INTEGER		DEFAULT 10," \
				"ftpcerturl			VARCHAR," \
				"ftpusername		VARCHAR," \
				"ftppassword		VARCHAR," \
				"ftpscerturl		VARCHAR,"
				"ftpsusername		VARCHAR," \
				"ftpspassword		VARCHAR);";

		if(sqliteDBquery(sql, hmiven))
		{
			log.logger("Table hmiparam Created Successfully!!!", ucmlogging::Info);
			sql = "INSERT INTO hmiparam (oadrusername,oadrpassword,venname,vtnurl,pollfreq,ssltls,enableven,drprogram,customeroverride,minsampling,maxsampling,vtnusername,vtnpassword,ftpslogurl,ftpslogging,upgradeurl,autoupgrade,ftpcerturl,ftpusername,ftppassword,ftpscerturl,ftpsusername,ftpspassword) "  \
					"VALUES ('', '', '', '', 30, 0, 0, '', 0, 5, 5, '', '', '', 10, '', 10, '', '', '', '', '', '');";
			if(sqliteDBquery(sql, hmiven))
				log.logger("Table hmiparam default values added Successfully!!!", ucmlogging::Info);
			else
				retStatus= false;
		}
		else
			retStatus= false;

		//opt config
		/* Create SQL statement */
		sql = "CREATE TABLE optconfig("  \
			  "eventid 			VARCHAR," \
			  "starttime 		VARCHAR," \
			  "endtime	 		VARCHAR," \
			  "optoutreason	 	VARCHAR," \
			  "optid 			VARCHAR);";

		if(sqliteDBquery(sql, hmiven))
		{
			log.logger("Table optconfig Created Successfully!!!", ucmlogging::Info);
			//Create SQL statement
			sql = "INSERT INTO optconfig (eventid,starttime,endtime,optoutreason,optid) "  \
				  "VALUES ('', '', '', '', '' );";
			if(sqliteDBquery(sql, hmiven))
				log.logger("Table optconfig default values added Successfully!!!", ucmlogging::Info);
			else
				retStatus= false;
		}
		else
			retStatus= false;

		//cea2045 config
		/* Create SQL statement */
		sql = "CREATE TABLE cea2045config("  \
			  "commport 		VARCHAR," \
			  "operstate 		INTEGER," \
			  "commstatus	 	INTEGER," \
			  "duration	 		INTEGER," \
			  "relativeprice 	FLOAT);";

		if(sqliteDBquery(sql, hmiven))
		{
			log.logger("Table cea2045config Created Successfully!!!", ucmlogging::Info);
			//Create SQL statement
			sql = "INSERT INTO cea2045config (commport,operstate,commstatus,duration,relativeprice) "  \
				  "VALUES ('', 0, 0, 0, 0.0 );";
			if(sqliteDBquery(sql, hmiven))
				log.logger("Table cea2045config default values added Successfully!!!", ucmlogging::Info);
			else
				retStatus= false;
		}
		else
			retStatus= false;
	}
	if ((stat(optschedule, &st) == -1))
	{
		//optschedule config
		/* Create SQL statement */
		sql = "CREATE TABLE optoutschedules("  \
			  "starttime 		VARCHAR," \
			  "endtime 			VARCHAR," \
			  "optoutreason	 	VARCHAR," \
			  "optid 			VARCHAR);";

		if(sqliteDBquery(sql, optschedule))
			log.logger("Table optoutschedules Created Successfully!!!", ucmlogging::Info);
		else
			retStatus= false;
	}

	return retStatus;
}

/**
 * Method to replace apostrophe character if any and illegal characters.
 * @param content
 * @return updated string
 */
string sqliteDB::removeIllegalchars(string content)
{
	string illegalChars = "<>\"";
	string aposChars = "'";
	string::iterator it;

	for (it = content.begin() ; it < content.end() ; ++it)
	{
		bool found = illegalChars.find(*it) != string::npos;
		if(found)
		{
			*it = ' ';
		}
	}
	size_t found = content.find("'");
	while(found<=content.length())
	{
		content.replace(found,1,"''");
		found = content.find("'", found+2);
	}

	return content;

}
/**
    Method to write single string variable to sqlite database
    @param variable - Column name
    @param - value - here value is specified
    @param - path - filename
    @param - tableName
    @return - status
*/

bool sqliteDB::sqliteDBwriteString(string variable, string value, string path, string tableName)
{
	string sql;
	value = removeIllegalchars(value);
	sql = "UPDATE "+tableName+" SET "+variable+" = '"+value+"';";

	if(!(sqliteDBquery(sql.c_str(), path)))
		return false;

	return true;

}

/**
    Method to write string variable to sqlite database matching to condition
    @param variable - Column name
    @param - value - here value is specified
    @param - path - filename
    @param - tableName
    @param - condvariable
    @param - condvalue
    @return - status
*/

bool sqliteDB::sqliteDBwriteStringWithCond(string variable, string value, string path, string tableName,
										   string condvariable, string condvalue)
{
	string sql;
	value = removeIllegalchars(value);
	sql = "UPDATE "+tableName+" SET "+variable+" = '"+value+"' WHERE "+condvariable+" = '"+condvalue+"';";

	if(!(sqliteDBquery(sql.c_str(), path)))
		return false;

	return true;

}

/**
    Method to write single integer variable to sqlite database
    @param variable - Column name
    @param - intValue - here value is specified
    @param - path - filename
    @param - tableName
    @return - status
*/
bool sqliteDB::sqliteDBwriteInt(string variable, int intValue, string path, string tableName)
{
	string sql;
	stringstream temp;
	temp<<intValue;
	sql = "UPDATE "+tableName+" SET "+variable+" = "+temp.str()+";";

	if(!(sqliteDBquery(sql.c_str(),path)))
		return false;

	return true;
}

/**
    Method to read a single sting variable from sqlite database
    @param variable - Column name
    @param - path - filename
    @param - tableName
    @return - string - value of the datatype
*/

string sqliteDB::sqliteDBreadString(string variable, string path, string tableName)
{
	ucmlogging log;
	string sql, retString;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sql = "SELECT * from "+tableName;
	DBVariablemap.clear();

	rc = sqlite3_open(path.c_str(), &db);

	if(rc)
	{
		log.logger("Can't open database: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return "";
	}
	sqlite3_busy_timeout(db,3000);
	rc = sqlite3_exec(db, sql.c_str(), DBRead, &variable, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
		return "";
	}

	sqlite3_close(db);
	sIterator = DBVariablemap.find(variable);
	if(sIterator != DBVariablemap.end())
		retString = sIterator->second;

	return retString;
}

/**
    Method to read a single integer variable from sqlite database
    @param variable - here data type is specified
    @param - path - filename
    @param - tableName
    @return - int - value of the datatype
*/

int sqliteDB::sqliteDBreadInt(string variable, string path, string tableName)
{
	ucmlogging log;
	string sql;
	int retInt = 0;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sql = "SELECT * from "+tableName;
	DBVariablemap.clear();

	rc = sqlite3_open(path.c_str(), &db);

	if(rc)
	{
		log.logger("Can't open database: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return retInt;
	}
	sqlite3_busy_timeout(db,3000);
	rc = sqlite3_exec(db, sql.c_str(), DBRead, &variable, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
		return retInt;
	}

	sqlite3_close(db);
	sIterator = DBVariablemap.find(variable);
	if(sIterator != DBVariablemap.end())
	{
		try
		{
			retInt = atoi(sIterator->second.c_str());
		}
		catch(...)
		{
			log.logger("Invalid Integer value for "+variable+"!!! Using 0.", ucmlogging::Error);
			return 0;
		}
	}
	return retInt;
}

/**
    Method to delete a single row from sqlite DB
    @param table name
    @param - path - filename
    @param variable - here data type is specified
    @param value
    @return - status
*/
bool sqliteDB::sqliteDBdeletestring(string tableName, string path, string variable,string value)
{
	ucmlogging log;
	string sql;

	sql = "DELETE from "+tableName+" where "+variable+"='"+value+"';";

	if(!(sqliteDBquery(sql.c_str(), path)))
	{
		log.logger(path+" delete failed for "+tableName+"where "+variable+" = " + value, ucmlogging::Error);
		return false;
	}
	return true;
}


/**
    Method to insert value to signaldetails sqlite database
    @param signalID
    @param - cvindex
    @return - status
*/
bool sqliteDB::insertDBsignaldetails(string signalID, unsigned int cvIndex)
{
	ucmlogging log;
	string sql;
	bool retStatus=true;
	struct stat st = {0};

	//signalID - cvindex
	// Create SQL statement
	if ((stat(signaldetails, &st) == -1))
	{
		sql = "CREATE TABLE signaldetails("  \
			  "signalid 	VARCHAR," \
			  "cvindex 		INTEGER);";

		if(sqliteDBquery(sql.c_str(), signaldetails))
			log.logger("Table signaldetails Created Successfully!!!", ucmlogging::Info);
		else
			return false;
		VENManager vm;
		vm.changePermission(signaldetails);
	}

	//Create SQL statement
	stringstream stream;
	stream <<cvIndex;
	sql = "INSERT INTO signaldetails (signalid,cvindex) VALUES ('"+signalID+"',"+stream.str()+");";
	if(sqliteDBquery(sql.c_str(), signaldetails))
		log.logger("Values added Successfully for table \"signaldetails\"!!!", ucmlogging::Info);
	else
		retStatus= false;

	return retStatus;
}

/**
    Method to read a signal cvindex and store map
    @param pointer to signalcvindex map
    @return - nothing
*/


void sqliteDB::readDBsignaldetails(map <string,unsigned int> *signalcvindexmap)
{
	string sql;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	ucmlogging log;
	signalindexmapPntr = signalcvindexmap;
	sql = "SELECT * from signaldetails";
	signalindexmapPntr->clear();

	rc = sqlite3_open(signaldetails, &db);

	if(rc)
	{
		log.logger("Can't open database signaldetails: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return;
	}
	sqlite3_busy_timeout(db,3000);

	rc = sqlite3_exec(db, sql.c_str(), DBReadsignalindex, &sql, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db);
}


/**
    Method to insert value to eventdetails sqlite database
    @param - requestID
    @param - eventID
    @param - modification no
    @param - start time
    @param - duration
    @param - status - active or pending
    @param - optstatus - opt-in or opt-out_hmi or opt-out_sch
    @param - optID
    @return - status
*/
bool sqliteDB::insertDBeventdetails(string requestid, string eventid, int modificationno,
									time_t dtstart, unsigned int  duration, string status,
									string optstatus, string optid)
{
	ucmlogging log;
	string sql;
	bool retStatus=true;
	struct stat st = {0};

	//eventdetails
	// Create SQL statement
	if ((stat(eventdetails, &st) == -1))
	{
		sql = "CREATE TABLE eventdetails("  \
			  "requestid 			VARCHAR," \
			  "eventid	 			VARCHAR," \
			  "modificationno 		INTEGER," \
			  "dtstart			 	VARCHAR," \
			  "duration			 	VARCHAR," \
			  "status			 	VARCHAR," \
			  "optstatus		 	VARCHAR," \
			  "optid			 	VARCHAR);";

		if(sqliteDBquery(sql.c_str(), eventdetails))
			log.logger("Table eventdetails Created Successfully!!!", ucmlogging::Info);
		else
			return false;
		VENManager vm;
		vm.changePermission(eventdetails);
	}

	//Create SQL statement
	stringstream modno;
	modno << modificationno;
	stringstream starttime;
	starttime << dtstart;
	stringstream dur;
	dur << duration;

	sql = "INSERT INTO eventdetails (requestid,eventid,modificationno,dtstart,duration,status,optstatus,optid) VALUES " \
		  "('"+requestid+"','"+eventid+"',"+modno.str()+",'"+ starttime.str()+"','"+dur.str()+"','" \
		  +status+"','"+optstatus+"','"+optid+"');";
	if(sqliteDBquery(sql.c_str(), eventdetails))
		log.logger("Values added Successfully for table \"eventdetails\"!!!", ucmlogging::Info);
	else
		retStatus= false;

	return retStatus;
}

/**
    Method to read a event details
    @param pointer to eventid-mod map
    @param pointer to eventid-starttime map
    @param pointer to eventid-duration map
    @param pointer to eventid-status map
    @param pointer to eventid-optstatus map
    @param pointer to eventid-reqID map
    @param pointer to eventid-optID map
    @return - nothing
*/
void sqliteDB::readDBeventdetails(map <string,int> *eventidmodmap,
								  map <string,time_t> *eventiddtstartmap,
								  map <string,unsigned int> *eventiddurationmap,
								  map <string,string> *eventidstatusmap,
								  map <string,string> *eventidoptstatusmap,
								  map <string,string> *eventidreqidmap,
								  map <string,string> *eventidoptidmap)
{
	string sql;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	ucmlogging log;

	eventidmodmapPntr = eventidmodmap;
	eventiddtstartmapPntr = eventiddtstartmap;
	eventiddurationmapPntr = eventiddurationmap;
	eventidstatusmapPntr = eventidstatusmap;
	eventidoptstatusmapPntr = eventidoptstatusmap;
	eventidreqidmapPntr = eventidreqidmap;
	eventidoptidmapPntr = eventidoptidmap;

	sql = "SELECT * from eventdetails";

	eventidmodmapPntr->clear();
	eventiddtstartmapPntr->clear();
	eventiddurationmapPntr->clear();
	eventidstatusmapPntr->clear();
	eventidoptstatusmapPntr->clear();
	eventidreqidmapPntr->clear();
	eventidoptidmapPntr->clear();


	rc = sqlite3_open(eventdetails, &db);

	if(rc)
	{
		log.logger("Can't open database eventdetails: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return;
	}

	sqlite3_busy_timeout(db,3000);

	rc = sqlite3_exec(db, sql.c_str(), DBReadeventdetails, &sql, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
}


/**
    Method to insert value to reportdetails sqlite database
    @param - reportrequestid
    @param - reportspecifierid
    @param - reporttype
    @param - granularity
    @param - reportbackdur
    @param - dtstart
    @param - duration
    @param - cvindex
    @return - status
*/
bool sqliteDB::insertDBreportdetails(string reportrequestid, string reportspecifierid,
									 string reporttype, string granularity,
									 string reportbackdur, time_t dtstart,
									 string  duration, unsigned int cvindex)
{
	ucmlogging log;
	string sql;
	bool retStatus=true;
	struct stat st = {0};

	//eventdetails
	// Create SQL statement
	if ((stat(reportdetails, &st) == -1))
	{
		sql = "CREATE TABLE reportdetails("  \
			  "reportrequestid 		VARCHAR," \
			  "reportspecifierid	VARCHAR," \
			  "reporttype 			VARCHAR," \
			  "granularity			VARCHAR," \
			  "reportbackdur		VARCHAR," \
			  "dtstart			 	VARCHAR," \
			  "duration		 		VARCHAR," \
			  "cvindex			 	INTEGER);";

		if(sqliteDBquery(sql.c_str(), reportdetails))
			log.logger("Table reportdetails Created Successfully!!!", ucmlogging::Info);
		else
			return false;
		VENManager vm;
		vm.changePermission(reportdetails);
	}

	//Create SQL statement
	stringstream stream;
	stream << cvindex;
	stringstream starttime;
	starttime << dtstart;

	sql = "INSERT INTO reportdetails (reportrequestid,reportspecifierid,reporttype,granularity," \
			"reportbackdur,dtstart,duration,cvindex) VALUES " \
		  "('"+reportrequestid+"','"+reportspecifierid+"','"+reporttype+"','"+ granularity+"','"+reportbackdur+"','" \
		  +starttime.str()+"','"+duration+"','"+stream.str()+"');";
	if(sqliteDBquery(sql.c_str(), reportdetails))
		log.logger("Values added Successfully for table \"reportdetails\"!!!", ucmlogging::Info);
	else
		retStatus= false;

	return retStatus;
}

/**
    Method to read a event details
    @param pointer to reportrequestid-reportspecifierid map
    @param pointer to reportrequestid-report type map
    @param pointer to reportrequestid-granularity map
    @param pointer to reportrequestid-reportbac duration map
    @param pointer to reportrequestid-duration map
    @param pointer to reportrequestid-start time map
    @param pointer to reportrequestid-cvindex map
    @return - nothing
*/


void sqliteDB::readDBreportdetails(map<string, string> *rrid_rsidMap,
								   map<string, string> *rrid_reportTypeMap,
								   map<string, string> *rrid_granMap,
								   map<string, string> *rrid_rbdurationMap,
								   map<string, string> *rrid_durMap,
								   map<string, time_t> *rrid_startMap,
								   map<string, unsigned int> *rrid_cvMap)
{
	string sql;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	ucmlogging log;

	rrid_rsidMapPntr = rrid_rsidMap;
	rrid_reportTypeMapPntr = rrid_reportTypeMap;
	rrid_granMapPntr = rrid_granMap;
	rrid_rbdurationMapPntr = rrid_rbdurationMap;
	rrid_durMapPntr = rrid_durMap;
	rrid_startMapPntr = rrid_startMap;
	rrid_cvMapPntr = rrid_cvMap;

	sql = "SELECT * from reportdetails";

	rrid_rsidMapPntr->clear();
	rrid_reportTypeMapPntr->clear();
	rrid_granMapPntr->clear();
	rrid_rbdurationMapPntr->clear();
	rrid_durMapPntr->clear();
	rrid_startMapPntr->clear();
	rrid_cvMapPntr->clear();


	rc = sqlite3_open(reportdetails, &db);

	if(rc)
	{
		log.logger("Can't open database reportdetails: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return;
	}

	sqlite3_busy_timeout(db,3000);

	rc = sqlite3_exec(db, sql.c_str(), DBReadreportdetails, &sql, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
}

/**
    Method to insert opt - out time in to optschedules database
    @param start time
    @param stop time
    @param reason - string
    @param optID
    @return - status
*/
bool sqliteDB::insertDBoptschedules(string startTime, string stopTime,string Reason, string optID)
{
	ucmlogging log;
	string sql;
	bool retStatus = true;

	//Create SQL statement
	sql = "INSERT INTO optoutschedules (starttime,endtime,optoutreason,optid) "  \
		  "VALUES ('"+startTime+"','"+stopTime+"','"+Reason+"','"+optID+"');";
	if(sqliteDBquery(sql.c_str(), optschedule))
		log.logger("Table optoutschedules - opt schedule value added Successfully!!!", ucmlogging::Info);
	else
		retStatus= false;

	return retStatus;
}

/**
    Method to read a event details
    @param pointer to optIDStartTime map
    @param pointer to optIDStopTime map
    @return - nothing
*/


void sqliteDB::readDBoptschedules(map <string,time_t> *optIDStartTimemap,
								  map <string,time_t> *optIDStopTimemap)
{
	string sql;
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	ucmlogging log;

	optIDStartTimemapPntr = optIDStartTimemap;
	optIDStopTimemapPntr = optIDStopTimemap;

	sql = "SELECT * from optoutschedules";

	optIDStartTimemapPntr->clear();
	optIDStopTimemapPntr->clear();

	rc = sqlite3_open(optschedule, &db);

	if(rc)
	{
		log.logger("Can't open database optschedule: " + string(sqlite3_errmsg(db)) , ucmlogging::Error);
		return;
	}

	sqlite3_busy_timeout(db,3000);

	rc = sqlite3_exec(db, sql.c_str(), DBReadoptschedule, &sql, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log.logger("SQL error: "+ string(zErrMsg) , ucmlogging::Error);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
}

