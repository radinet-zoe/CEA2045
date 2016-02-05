/*
 * PollHandler.cpp
 *
 *  Created on: 01-Sep-2014
 *      Author: dinesh
 */

#include "PollHandler.h"

using namespace std;

char *notifierPoll = new char[20];
char *notifierPollVal = new char[20];

/**
    pollHandler Constructor
    @param venID - VEN ID returned while registration
    @param registrationID - Registration ID returned while registration
    @param vtnID - VTN ID returned while registration
    @param pollFreq - poll frequency in seconds from user
 */
pollHandler::pollHandler(string venID, string registrationID, string vtnID, unsigned int pollFreq) :
	m_venID(venID),
	m_registrationID(registrationID),
	m_vtnID(vtnID),
	pollFreq(pollFreq)
{
	pollExit=1;
}

/**
    Polling thread, which polls at a specified frequency, for any messages from VTN 
    @param pointer to class VEN2b
    @param pointer to class VENManager
    @param condition variable for notifying
    @param notifierpointer - location where notify context is checked
    @return - nothing
*/
void pollHandler::startPolling(VEN2b *venPoll, VENManager *vm, condition_variable *cvPoll, condition_variable *cvEvent, condition_variable cvSignal[], condition_variable cvReport[], char *notifierP)
{
	//poll starts...
	sqliteDB sdb;
	sdb.sqliteDBwriteInt("pollthread",1, thread_status, "threadstatus");
	ucmlogging log;
	log.logger("Poll Thread Started", ucmlogging::Info);
	//wait_Untill
	mutex cv_Poll;
	unique_lock<mutex> lkPoll(cv_Poll);
	notifierPoll = notifierP;
	strcpy(notifierPollVal,"POLL");

	EventHandler eh(getVenId(), getVtnId());

	while(pollExit)
	{
		auto now = chrono::system_clock::now();

		if(cvPoll->wait_until(lkPoll, now + chrono::seconds(getPollFreq()), [](){return (!strcmp(notifierPoll,notifierPollVal));}))
		{
			log.logger("Notification received for Poll. Poll Stopping!!!", ucmlogging::Info);
			pollExit=0;
		}
		else
		{
			log.logger("Sending Poll Request", ucmlogging::Info);

			try
			{
				auto_ptr<Poll> pollmsg = venPoll->poll();
				oadrPayload *payload = pollmsg->response();

				//log file creation.
				xmlFileCreation requestLog("poll.xml", pollmsg->requestBody());

				if (payload == NULL)
				{
					log.logger("Invalid response from poll: ", ucmlogging::Warning);
					xmlFileCreation responseLog("oadrResponse.xml", pollmsg->responseBody());
				}

				else if ((payload->oadrSignedObject().oadrResponse().present()))
				{
					log.logger("Received oadrResponse: ", ucmlogging::Info);
					xmlFileCreation responseLog("oadrResponse.xml", pollmsg->responseBody());
				}
				else if ((payload->oadrSignedObject().oadrRequestReregistration().present()))
				{
					pollExit=0; //stopping poll
					vm->waitNotifier("EVENT");
					if((vm->getThreadStatus("EVENT")))
					{
						log.logger("Failed to Stop event thread. event thread still running!!!Re-Registration failed!!!", ucmlogging::Error);
						goto nextPoll;
					}
					vm->waitNotifier("SIGNAL");
					if((vm->getThreadStatus("SIGNAL")))
					{
						log.logger("Failed to Stop signal thread(s). signal thread(s) still running!!!Re-Registration failed!!!", ucmlogging::Error);
						goto nextPoll;
					}
					vm->waitNotifier("REPORT");
					if((vm->getThreadStatus("REPORT")))
					{
						log.logger("Failed to Stop report thread(s). report thread(s) still running!!!Re-Registration failed!!!", ucmlogging::Error);
						goto nextPoll;
					}

					remove(eventdetails);
					remove(reportdetails);
					remove(signaldetails);

					xmlFileCreation responseLog("oadrRequestReregistration.xml", pollmsg->responseBody());
					log.logger("Reregister Request Received", ucmlogging::Info);

					log.logger("Sending create party registration", ucmlogging::Info);
					//create party registration from ven to vtn
					registerParty rh(getVenId(), getRegistrationId());
					rh.RegisterParty(venPoll);
					log.logger("Notification received for Poll. Poll Stopping!!!", ucmlogging::Info);
					goto PollFin;
				}
				else if ((payload->oadrSignedObject().oadrDistributeEvent().present()))
				{

					xmlFileCreation responseLog("oadrDistributeEvent.xml", pollmsg->responseBody());
					log.logger("Distribute Event Request Received", ucmlogging::Info);

					vm->waitNotifier("EVENT");
					bool status;
					log.logger("Sending created event", ucmlogging::Info);
					//starting event thread...
					oadrDistributeEventType eventPayload = payload->oadrSignedObject().oadrDistributeEvent().get();
					status = eh.eventSchedular(venPoll, vm, payload->oadrSignedObject().oadrDistributeEvent().get(), cvEvent, cvSignal, notifierP);
					if(!status)
						log.logger("Event Scheduling Failed", ucmlogging::Warning);
				}
				else if (payload->oadrSignedObject().oadrCreateReport().present())
				{
					oadrCreateReportType CreateRptPayload = payload->oadrSignedObject().oadrCreateReport().get();
					oadrCreateReportType::oadrReportRequest_iterator RptItr;
					for(RptItr = CreateRptPayload.oadrReportRequest().begin(); RptItr!=CreateRptPayload.oadrReportRequest().end();RptItr++)
						xmlFileCreation responseLog(string(ReportXMLfolder)+RptItr->reportRequestID()+"_oadrCreateReport.xml", pollmsg->responseBody());
					log.logger("Create Report Request Received", ucmlogging::Info);
					bool status;
					ReportHandler RH(getVenId());
					status = RH.reportScheduler(venPoll, CreateRptPayload, cvReport,notifierP);
					if(!status)
						log.logger("Report Scheduling Failed", ucmlogging::Warning);

				}
				else if (payload->oadrSignedObject().oadrRegisterReport().present())
				{
					xmlFileCreation responseLog("oadrRegisterReport.xml", pollmsg->responseBody());
					log.logger("Register Report Request Received", ucmlogging::Info);
					//TODO send RegisteredReport
				}
				else if (payload->oadrSignedObject().oadrCancelReport().present())
				{
					xmlFileCreation responseLog(string(ReportXMLfolder)+"oadrCancelReport.xml", pollmsg->responseBody());
					log.logger("Cancel Report Request Received", ucmlogging::Info);
					bool status;
					ReportHandler RH(getVenId());
					oadrCancelReportType CancelRptPayload = payload->oadrSignedObject().oadrCancelReport().get();
					status = RH.cancelReport(venPoll, CancelRptPayload, cvReport,notifierP);
					if(!status)
						log.logger("Cancel Report Failed", ucmlogging::Warning);
				}
				else if (payload->oadrSignedObject().oadrUpdateReport().present())
				{
					xmlFileCreation responseLog(string(ReportXMLfolder)+"oadrUpdateReport.xml", pollmsg->responseBody());
					log.logger("Update Report Request Received", ucmlogging::Info);
					//TODO send UpdatedReport
				}
				else if (payload->oadrSignedObject().oadrCancelPartyRegistration().present())
				{
					xmlFileCreation responseLog("oadrCancelPartyRegistration.xml", pollmsg->responseBody());
					log.logger("Cancel Party Registration Request Received", ucmlogging::Info);
					pollExit=0; //stopping poll
					vm->waitNotifier("EVENT");
					if((vm->getThreadStatus("EVENT")))
					{
						log.logger("Failed to Stop event thread. event thread still running!!!Cancel Registration failed!!!", ucmlogging::Error);
						goto nextPoll;
					}
					vm->waitNotifier("SIGNAL");
					if((vm->getThreadStatus("SIGNAL")))
					{
						log.logger("Failed to Stop signal thread(s). signal thread(s) still running!!!Cancel Registration failed!!!", ucmlogging::Error);
						goto nextPoll;
					}
					vm->waitNotifier("REPORT");
					if((vm->getThreadStatus("REPORT")))
					{
						log.logger("Failed to Stop report thread(s). report thread(s) still running!!!Cancel Registration failed!!!", ucmlogging::Error);
						goto nextPoll;
					}

					clearRegistration cr;
					cr.clearregistration();
					remove(eventdetails);
					remove(reportdetails);
					remove(signaldetails);

					//TODO send CancelledPartyRegistration
					log.logger("Notification received for Poll. Poll Stopping!!!", ucmlogging::Info);
					goto PollFin;
				}
				else
				{
					log.logger("Invalid message received for poll response",ucmlogging::Warning);
					xmlFileCreation responseLog("oadrResponse.xml", pollmsg->responseBody());
				}
			}
			catch (CurlException& ex)
			{
				log.logger("caught exception: " + string(ex.what()) + "!!!Poll Failed!!!",ucmlogging::Error);
			}
			nextPoll:
			//poll ends
			stringstream temp;
			temp<<getPollFreq();
			log.logger("Poll ends-- " + temp.str() + " Seconds for next Poll!!!", ucmlogging::Info);
		}
	}
	PollFin:
	sdb.sqliteDBwriteInt("pollthread",0, thread_status, "threadstatus");
}

/********************************************************************************/


