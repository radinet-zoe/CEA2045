/*
 * CustomerOverride.cpp
 *
 *  Created on: 08-Jan-2015
 *      Author: dinesh
 */


#include "CustomerOverride.h"


/**
    CustomerOverride Constructor
 */
CustomerOverride::CustomerOverride()
{

}


void CustomerOverride::getOverrideStatus(VEN2b *venObject)
{

	sqliteDB sdb;
	ucmlogging log;
	ifstream fin;
	VENManager vm;

	log.logger("Starting Customer Override monitor Thread ", ucmlogging::Info);

	time_t currentTime = time(0);
	while(1)
	{
		int customerOverride = 0;
		bool keepLooping = true;
		string retValue;
		stringstream stream;
		string lastLine;
		string eventID;
		int PosNumber = 0;

		currentTime+=60;
		this_thread::sleep_until (chrono::system_clock::from_time_t (currentTime));
		log.logger("Reading Customer Override Value from dplogging.csv file", ucmlogging::Info);

		fin.open(dplogfile);
		if(fin.fail())
		{
			log.logger("Failed to open dplogging.csv file!!!", ucmlogging::Error);
			goto next;
		}


		fin.seekg(-2,ios_base::end);                // go to one spot before the EOF
		while(keepLooping)
		{
			char ch;
			fin.get(ch);                            // Get current byte's data

			if((int)fin.tellg() <= 1)				// The first line is the last line
			{
				fin.seekg(0);
				keepLooping = false;
			}
			else if(ch == '\n') 					// If the data was a newline indicates the last line in file
				keepLooping = false;
			else									// If the data was neither a newline nor at the 0 byte
				fin.seekg(-2,ios_base::cur);
		}

		getline(fin, lastLine);
		stream<<lastLine;
		while(getline(stream, retValue, ','))
		{
			if(PosNumber == 3) //value 3 refers to customer override status..
			{
				log.logger("customer override - " + retValue, ucmlogging::Info);
				if(!retValue.compare("true"))
					customerOverride = 1;
				else
					customerOverride = 0;
			}
			PosNumber++;
		}
		fin.close();
		next:
		if(customerOverride)
		{
			//update the database
			sdb.sqliteDBwriteInt("customeroverride",1, hmiven, "hmiparam");
			//notify the active events
			if((vm.getThreadStatus("SIGNAL")))
			{
				//send opt-out for that event
				EventHandler eh;
				eh.getdata();

				for (eh.timeIterator=eh.eventiddtstartmap.begin(); eh.timeIterator!=eh.eventiddtstartmap.end(); ++eh.timeIterator)
				{
					eh.stringIterator = eh.eventidstatusmap.find(eh.timeIterator->first);
					if(!(eh.stringIterator->second.compare("Active")))
					{
						EventResponses er;
						VENManager venm;
						eventID = eh.timeIterator->first;
						eh.stringIterator = eh.eventidreqidmap.find(eh.timeIterator->first);
						eh.intIterator = eh.eventidmodmap.find(eh.timeIterator->first);

						er.addEventResponse("200", "OK", eh.timeIterator->first, eh.intIterator->second, OptTypeType::optOut, eh.stringIterator->second);
						if(venm.sendCreatedEvent(venObject,er))
							log.logger("Opt-out sent successfully for Event - " + eh.timeIterator->first, ucmlogging::Info);
						else
							log.logger("Opt-out failed for Event - " + eh.timeIterator->first, ucmlogging::Error);
					}
				}


				vm.waitNotifier("SIGNAL");
				if((vm.getThreadStatus("SIGNAL")))
				{
					sdb.sqliteDBwriteString("errormsg","Failed to stop signal thread(s). Signal thread(s) still running!!!", hmiven, "hmivenflag");
					sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
					log.logger("Failed to stop signal thread(s). Signal thread(s) still running!!! Event opt-out failed!!!", ucmlogging::Error);
				}
				else
				{
					if(!(sdb.sqliteDBdeletestring("eventdetails", eventdetails, "eventid", eventID)))
					{
						sdb.sqliteDBwriteString("errormsg","Failed to delete "+eventID+" from eventdetails.db!!!", hmiven, "hmivenflag");
						sdb.sqliteDBwriteInt("flag",9, hmiven, "hmivenflag");
					}
					else
					{
						log.logger("Deleted eventID - " +eventID+" from database!!!", ucmlogging::Info);
						sdb.sqliteDBwriteInt("flag",0, hmiven, "hmivenflag"); //reset the flag
					}
				}
			}
		}
		else
		{
			//update the database
			sdb.sqliteDBwriteInt("customeroverride",0, hmiven, "hmiparam");

		}
	}
}

