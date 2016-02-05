/*
 * main.cpp
 *
 *  Created on: 22-Aug-2014
 *      Author: dinesh
 */
#include "Main.h"
extern "C"
{
	#include "CEA2045/cea2045lib.h"
}
/**
 * Main Program
 */
int main()
{
	//making the program taken over by the init process.
	pid_t process_id = 0;
	process_id = fork();

	if (process_id < 0){
		cout<<"Process VENManager Failed!!!"<<endl;
		exit(1);
	}

	// PARENT PROCESS. Need to kill it.
	if (process_id > 0){
		cout<<"Process VENManager started. Process ID - "<< process_id<<endl;
		sleep(1);
		exit(0);
	}

	//elevating to root permission
	if (setuid(geteuid()) < 0){
		cout<<"Root Permission failed!!!Process ending!!"<<endl;
		exit(1);
	}

	sqliteDB sdb;
	ucmlogging log;
	VENManager vm;
	clearRegistration cr;

	//Create Folder Structure
	if(!(vm.createFolderStructure()))
	{
		cout<<"Conf Files missing!!!Process VENManager Failed!!!"<<endl;
		exit(1);
	}

	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//creating the databases and assigning initial values
	sdb.sqliteDBCreate();

	//Initialise db with default values.
	vm.sqliteInitialise();

	//Clearing all registration related details (VEN id and Registration id)
	cr.clearregistration();

	log.logger("Opening Comm Port to SGD", ucmlogging::Info);
	
	//Open Serial port
	if(check_ucmtype() != 0)
	{
		log.logger("DC Mode connection", ucmlogging::Info);
		if(open_serialport("/dev/ttyACM0") != 0)
		{
			log.logger("Comm Port to SGD opening Error ", ucmlogging::Error);
			//exit(1);
		}
		else
		{
			sdb.sqliteDBwriteString("commport","/dev/ttyACM0", hmiven,"cea2045config");
			log.logger("Comm Port to DC SGD opened successfully ", ucmlogging::Info);
		}
		
	}
	else
	{
		log.logger("AC Mode connection", ucmlogging::Info);
		if(open_serialport("/dev/ttyATH0") != 0)
		{
			log.logger("Comm Port to SGD opening Error ", ucmlogging::Error);
			//exit(1);
		}
		else
		{
			sdb.sqliteDBwriteString("commport","/dev/ttyATH0", hmiven,"cea2045config");
			log.logger("Comm Port to AC SGD opened successfully ", ucmlogging::Info);
		}		
	}

	// start main
	log.logger("Main Thread Started", ucmlogging::Info);

	// update the VEN/VTN parameters
	vm.updateParameter();

	VEN2b venObject(new HttpCurl(), vm.getBaseUrl(),vm.getVenName());

	//start datalogging thread
	vm.startDataLogThread(&venObject, 0);

	//register VEN
	log.logger("Registering VEN", ucmlogging::Info);
	vm.registerVEN(&venObject);

	while(1)
	{
		int flag=0;
		flag=sdb.sqliteDBreadInt("flag", hmiven, "hmivenflag");
		if(flag && flag!=9)
		{
			stringstream temp;
			temp<<flag;
			log.logger("HMI interaction detected!!! Value - "+temp.str(), ucmlogging::Info);

			vm.updateParameter();

			venObject.setVenName(sdb.sqliteDBreadString("venname", hmiven, "hmiparam"));
			venObject.setBaseUrl(sdb.sqliteDBreadString("vtnurl", hmiven, "hmiparam"));

			switch (flag)
			{
				case LOGIN:{
					vm.Login();
					break;
				}case STARTPOLLING:{
					vm.StartPolling(&venObject);
					break;
				}case STOPPOLLING:{
					vm.StopPolling();
					break;
				}case REGISTERVEN:{
					vm.RegisterVEN(&venObject);
					break;
				}case QUERYREG:{
					vm.QueryRegistration(&venObject);
					break;
				}case CANCELREG:{
					vm.CancelRegistration(&venObject);
					break;
				}case CLEARREG:{
					vm.ClearRegistration();
					break;
				}case REREGISTER:{
					vm.ReRegistration(&venObject);
					break;
				}case OPTEVENT:{
					vm.OptEvent(&venObject);
					break;
				}case CREATEOPT:{
					vm.CreateOpt(&venObject);
					break;
				}case CANCELOPT:{
					vm.CancelOpt(&venObject);
					break;
				}case STARTLOG:{
					vm.StartLogThread(&venObject);
					break;
				}case STOPLOG:{
					vm.StopLogThread();
					break;
				}case OPENCOMMPORT:{
					vm.OpenComPort();
					break;
				}case CLOSECOMMPORT:{
					vm.CloseComPort();
					break;
				}case QUERYOPERATION:{
					vm.QueryOperation();
					break;
				}case SENDOUTSIDECOMM:{
					vm.SendOutsideComm();
					break;
				}case SHEDEVET:{
					vm.ShedEvent();
					break;
				}case ENDSHED:{
					vm.EndShed();
					break;
				}case LOADUPSIG:{
					vm.LoadUp();
					break;
				}case CRITICALPEAKEVENT:{
					vm.CriticalPeakEvent();
					break;
				}case GRIDEMERGENCY:{
					vm.GridEmergency();
					break;
				}case GRIDGUIDANCEGOOD:{
					vm.GridGuidanceGoodTime();
					break;
				}case GRIDGUIDANCEBAD:{
					vm.GridGuidanceBadTime();
					break;
				}case GRIDGUIDANCENEUTRAL:{
					vm.GridGuidanceNeutral();
					break;
				}case SETPRICE:{
					vm.SetPrice();
					break;
				}default:{}
			}
		}
		sleep(2);
	}
	if(close_serialport() != 0)
		log.logger("Comm Port closing Error ", ucmlogging::Error);
	return 1;
}

/********************************************************************************/
