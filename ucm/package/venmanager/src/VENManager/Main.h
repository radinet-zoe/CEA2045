//*********************************************************************************************************************
// main.h

#ifndef MAIN_H_
#define MAIN_H_

#include <map>
#include "iostream"
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <pthread.h>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <chrono>
#include <vector>
#include <condition_variable>

#include "Registration/Registerparty.h"
#include "Registration/RegistrationQuery.h"
#include "Registration/ClearRegistration.h"
#include "Registration/CancelRegistration.h"
#include "PollHandler.h"
#include "UCMLogging.h"
#include "VENManager.h"
#include "XMLFileCreation.h"
#include "CustomerOverride.h"
#include "Event/EventHandler.h"
#include "Report/ReportHandler.h"
#include "Registration/ReportRegister.h"
#include "SqliteDB/sqliteDB.h"
#include "Opt/OptScheduleCreate.h"
#include "Opt/OptScheduleCancel.h"
#include "ConfFileGeneration/confGeneration.h"
#include "LogThread/logThread.h"

#include <oadr/ven/VEN2b.h>
#include <oadr/ven/http/HttpCurl.h>
#include <oadr/request/report/ReportWrapper.h>

// Need to replace all the pre-processor directives with constants.
#define loginUsername "admin"
#define loginPassword "admin"

#define home_path "/opt/venmanager"

#define data_path home_path "/data" 			//data folder Path
#define utils_path home_path "/utils" 			//utils folder Path
#define conf_path home_path "/conf" 			//conf folder Path
#define Logpath home_path "/log" 				//log folder Path
#define ftpsupdate utils_path "/ftpsupdate" 	//ftpsupdate folder Path
#define XMLFilePath Logpath "/xml/" 			//xml folder Path
#define ReportXMLfolder "report/"				//give only folder name. path is XMLFilePath..
#define RegReportXMLfolder "registeredreport/"  //give only folder name. path is XMLFilePath..

#define ven_param data_path "/venparam.db"
#define thread_status data_path "/threadstatus.db"
#define hmiven data_path "/hmiven.db"
#define optschedule data_path "/optoutschedules.db"
#define reportdetails data_path "/reportrequests.db"
#define signaldetails data_path "/signaldetails.db"
#define eventdetails data_path "/eventdetails.db"

#define signalsConfFile conf_path "/signals.conf"
#define reportConffile conf_path "/reports.conf"
#define dpconffile conf_path "/dplogging.conf"

#define dplogfile data_path "/dplogging.csv"

#define ucmLogPath Logpath "/UCM.log"
#define InvalidReportEvent ftpsupdate "/Invalid_Events_Reports.log"
#define venClientCert home_path "/certificates/VEN"
//#define CEA2045logFile Logpath "/CEA2045.log"

//Maximum number of signal or report thread that can run at a same time.
#define maxsignal 20
#define maxreport 20

using namespace std;

//case Macros definition
#define LOGIN 1
#define STARTPOLLING 2
#define STOPPOLLING 3
#define REGISTERVEN 4
#define QUERYREG 5
#define CANCELREG 6
#define CLEARREG 7
#define REREGISTER 8
#define OPTEVENT 10
#define CREATEOPT 11
#define CANCELOPT 12
#define STARTLOG 13
#define STOPLOG 14
#define OPENCOMMPORT 50
#define CLOSECOMMPORT 51
#define QUERYOPERATION 52
#define SENDOUTSIDECOMM 53
#define SHEDEVET 54
#define ENDSHED 55
#define LOADUPSIG 56
#define CRITICALPEAKEVENT 57
#define GRIDEMERGENCY 58
#define GRIDGUIDANCEGOOD 59
#define GRIDGUIDANCEBAD 60
#define GRIDGUIDANCENEUTRAL 61
#define SETPRICE 62



#endif /* MAIN_H_ */
