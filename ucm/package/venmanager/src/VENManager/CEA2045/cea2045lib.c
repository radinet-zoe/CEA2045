#include "cea2045lib.h"

//Globals
//volatile bool STOP;
unsigned char txBuffer[256];				  // Buffer for characters to be sent to SGD
unsigned char rxBuffer[256];				  // Buffer for characters received from SGD
unsigned char linkAckBuff[2];
struct cea2045BasicStruct basic;
struct cea2045InterStruct inter;
struct linkStruct linkData;
struct devInfoStruct devInfo;				  // Device Information structure
struct commodityreadStruct commodityreadInfo; //Commodity Read structure

unsigned char setenergyprice_respcode = -1; 		//Set energy price response code
unsigned char settempoffset_respcode = -1; 		//Set temperature offset response code
unsigned char setsetpoint_respcode = -1;			//Set setpoint response code
unsigned char commoditysubscr_respcode = -1;


struct termios tio;
struct timeval rxStartTime;
struct timeval txStartTime;
struct timeval currentTime;
struct timeval pendingTime;
struct timeval pendAckTime;
struct timeval txDelayTime;
struct timeval pendingTxTime;
time_t shedEndTime = 0;
time_t currentTimeSec = 0;
int expectedMsgSize = 0;
int rxCount = 0;
bool AppAckstate=false;

int cea2045_fd;
FILE *fp;
bool pendingTxLinkAck = false;
bool pendingLinkAck = false;
bool LinkAckSent = false;
struct timeval select_timeout;
fd_set read_fds;

int ucmtype = 0; // AC UCM board

static pthread_mutex_t logfilemutex = PTHREAD_MUTEX_INITIALIZER;

/**
 *==========================================================================
 * Function to map oadr signals to cea2045 messages and sent to sgd
 * Called from SignalHandlerClass of VEN manager.
 * Also called when values are set in HMI page.
 
 * @param SignalName - Signal Name (Sent from VTN to VEN in oadrDistributeEvent payload)
 * @param SignalName - Signal Type (Sent from VTN to VEN in oadrDistributeEvent payload)
 * @param SignalVal - Signal Value (Sent from VTN to VEN in oadrDistributeEvent payload)
 * @param param - Duration in case of SHED, LOADUP, CPP, GRID_EMERGENCY cmds.
 * 				  Good/Bad/Neutral time to use energy in case of GRID_GUIDANCE cmd.
 * @return boolean value - true if expected response recieved on time.
 * 						   otherwise false
 *==========================================================================
 */
 
//TODO: Handle SLEEPSTATE/WAKESTATE conditions
bool oadr_cea_EventMap(const char *signalname, const char *signaltype, const char *signalval, int param)
{
	time_t cycleTime = 0, currentTimeSec = 0;			
	int rtn,event = 0;
	bool responseflag = false, stop = false;
	int signalType = 1;  //1-for Basic, 2- for Intermediate

	AppAckstate = 0;

	if(!strcmp(signalname,"SIMPLE"))
		event = 1;
	else if(!strcmp(signalname,"ELECTRICITY_PRICE"))
		event = 2;
	else if(!strcmp(signalname,"LOAD_DISPATCH"))
		event = 3;
	else if(!strcmp(signalname,"LOAD_CONTROL"))
		event = 4;
	else if(!strcmp(signalname,"END_SHED"))
		event = 5;
	else if(!strcmp(signalname,"GRID_GUIDANCE"))
		event = 6;

	rtn = linkLayer();

	cycleTime = time(&currentTimeSec);
	while (stop == false){


		currentTimeSec = time(&currentTimeSec);
		if (cycleTime <= currentTimeSec){
			switch (event){		
				
			case 1:{
					if (linkData.txBlock == false){
						int Duration = param * 60;	//Duration in seconds
						int simpleVal = atoi(signalval);
						event = 10;
						cycleTime = currentTimeSec + 10;										//Do next step in 10 seconds
						//signal value = 0  for signal type = SIMPLE
						if(simpleVal == 0){
							logger("Issue LOADUP command (Opcode2 is Duration)");
							rtn = CEA2045basic(LOADUP, Duration, 0);
							if (rtn == 0){
								basic.appAckRtn = LOADUP;
								basic.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
						//signal value = 1  for signal type = SIMPLE
						else if(simpleVal == 1){
							logger("Issue SHED command (Opcode2 is Duration)");
							rtn = CEA2045basic(SHED, Duration, 0);
							if (rtn == 0){
								basic.appAckRtn = SHED;
								basic.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
						//signal value = 2  for signal type = SIMPLE
						else if(simpleVal == 2){
							logger("Issue CRITICAL PEAK command (Opcode2 is Duration)");
							rtn = CEA2045basic(CPP, Duration, 0);
							if (rtn == 0){
								basic.appAckRtn = CPP;
								basic.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
						//signal value = 3  for signal type = SIMPLE
						else if(simpleVal == 3){
							logger("Issue GRID EMERGENCy command (Opcode2 is Duration)");
							rtn = CEA2045basic(GRID_EMERGENCY, Duration, 0);
							if (rtn == 0){
								basic.appAckRtn = GRID_EMERGENCY;
								basic.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
					}
					else{
						cycleTime += 1;															//Wait 1 second and try again
					}
					break;
				}

			case 2:{
					if(!strcmp(signaltype, "priceMultiplier")){
						float relativeprice = (float)strtod(signalval,NULL);
						if (linkData.txBlock == false){
							event = 10;
							cycleTime = currentTimeSec + 10;									//Do next step in 10 seconds
							logger("Issue PRESENT RELATIVE PRICE command (Opcode 2 is relativeprice)");
							rtn = CEA2045basic(PRESENT_RELATIVE_PRICE, 0, relativeprice);		//Issue present relative price command
							if (rtn == 0){
								basic.appAckRtn = PRESENT_RELATIVE_PRICE;
								basic.timeoutTime = time(&currentTimeSec) + 4;
							}


						}
						else{
							cycleTime += 1;														//Wait 1 second and try again
						}	
					}
					else if(!strcmp(signaltype, "price")){
						signalType = 2; // intermediate message
						float relativeprice = (float)strtod(signalval,NULL);
						if (linkData.txBlock == false){
							event = 10;
							cycleTime = currentTimeSec + 10;									//Do next step in 10 seconds
							//Set energy price command
							logger("Issue Set ENERGY PRICE  command");
							//CEA2045inter_SetEnergyPrice(ENERGY_PRICE, 0, currentPrice, currencyCode(840 = USdollar), digitsaftrdecimal);
							rtn = CEA2045inter_SetEnergyPrice(ENERGY_PRICE, 0, relativeprice, 840, 2);	//Issue set energy price command to sgd
							if(rtn == 0){
								inter.respOpCode1 = ENERGY_PRICE;
								inter.respOpCode2 = 0x80;
								inter.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
						else{
							cycleTime += 1;														//Wait 1 second and try again
						}
					}
					break;
				}

			case 3:	{
					if (linkData.txBlock == false){
						// Value from VTN will be in -10 to 10 scale, change it to -127 to 127 scale
						int powerlevel = atoi(signalval) * 12.7;
						event = 10;
						cycleTime = currentTimeSec + 10;										//Do next step in 10 seconds
						logger("Issue POWER LEVEL command (Opcode 2 is powerlevel)");
						rtn = CEA2045basic(POWER_LEVEL, powerlevel, 0);							
						if (rtn == 0){
							basic.appAckRtn = POWER_LEVEL;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else{
						cycleTime += 1;															//Wait 1 second and try again
					}
					break;
				}
				
			case 4:{
					if(!strcmp(signaltype, "x-LoadControlOffset")){
						signalType = 2; // intermediate message
						int tempoffset = atoi(signalval);
						if (linkData.txBlock == false){
							cycleTime = currentTimeSec + 10;									//Do next step in 10 seconds
							event = 10;
							//Set temp offset command
							logger("Issue Set TEMPERATURE OFFSET command");
							rtn = CEA2045inter_SetTempOffset(TEMP_OFFSET, 2, tempoffset, 0);	//Issue set temp offse command to sgd
							if (rtn == 0){
								inter.respOpCode1 = TEMP_OFFSET;
								inter.respOpCode2 = 0x82; 
								inter.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
						else{
							cycleTime += 1;														//Wait 1 second and try again
						}
					}
					else if(!strcmp(signaltype, "x-LoadControlSetpoint")){
						signalType = 2; // intermediate message
						int setpoint = atoi(signalval);
						if (linkData.txBlock == false){
							cycleTime = currentTimeSec + 10;									//Do next step in 10 seconds
							event = 10;
							logger("Issue Set SET POINT command(unit 0 ='F and 1 = 'C");
							//CEA2045inter_SetSetPoint(SETPOINT, 3,, uint16_t devType(40=central AC), int units(1=degC), int16_t setpoint1)
							rtn = CEA2045inter_SetSetPoint(SETPOINT, 3, devInfo.devType, 1, setpoint, 0);	//Issue set set point command to sgd
							if (rtn == 0){
								inter.respOpCode1 = SETPOINT;
								inter.respOpCode2 = 0x83;
								inter.timeoutTime = time(&currentTimeSec) + 4;
							}
						}
						else{
							cycleTime += 1;														//Wait 1 second and try again
						}
					}
						break;
				}	
			//This must be sent once from the UCM to the SGD followed by load shed/Power Level/CPP/Grid Emergency/Grid Guidance	command
			case 5:{
					if (linkData.txBlock == false) {
						cycleTime = currentTimeSec + 10;										//Do next step in 10 seconds
						event = 10;
						logger("Issue END SHED command");
						rtn = CEA2045basic(END_SHED, 0, 0);										//Issue end shed command
						if (rtn == 0) {
							basic.appAckRtn = END_SHED;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else {
						cycleTime += 1;															//Wait 1 second and try again
					}
					break;
				}
			//This is sent when a request for neutral /bad /good time to use energy option is set from HMI page.
			case 6:{
					if (linkData.txBlock == false) {
						int secparam = param;
						cycleTime = currentTimeSec + 10;										//Do next step in 10 seconds
						event = 10;
						//Issue grid guidance command											
						logger("Issue GRID GUIDANCE command (Opcode2 is good/bad/neutral time to use energy)");
						rtn = CEA2045basic(GRID_GUIDANCE, secparam, 0);	
						//Second parameters are: 
						// 0 = Bad time to use energy
						// 1 = Neutral
						// 2 = Good time to use energy
						if (rtn == 0) {
							basic.appAckRtn = GRID_GUIDANCE;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else {
						cycleTime += 1;															//Wait 1 second and try again
					}
					break;
				}
			
			default:
					stop = true;
					break;
			}
		}


		usleep(200);
		//basic message
		if(signalType == 1){
			if(AppAckstate){
				return true;
			}
		}

		//intermediate message
		if(signalType == 2){
			if((setenergyprice_respcode==0) || (settempoffset_respcode==0) || (setsetpoint_respcode==0)){
				return true;
			}
		}

		if (basic.timeoutTime > 0 && basic.timeoutTime < time(&currentTimeSec)) {
			logger("Timed out waiting for response");
			basic.appAckRtn = 0;
			linkData.rxCount = 0;
			basic.pendingOpState = false;
			basic.timeoutTime = 0;
		}
		
		//Process pending inter message operations
		if (inter.timeoutTime > 0 && inter.timeoutTime < time(&currentTimeSec)) {
			logger("Timed out waiting for intermediate request response");
			linkData.rxCount = 0;
			inter.timeoutTime = 0;
			inter.respOpCode1 = 0;
			inter.respOpCode2 = 0;
		}
																				// Sleep for 1ms to allow other threads to operate
	}

	//if((linkData.nakVal == -1) && (basic.online == true))
	//	responseflag = true;
/*	if((linkData.nakVal == -1) && (setenergyprice_respcode == 0))
		responseflag = true;
	if((linkData.nakVal == -1) && (!strcmp(signaltype, "x-LoadControlOffset")) && (settempoffset_respcode == 0))
		responseflag = true;
	if((linkData.nakVal == -1) && (!strcmp(signaltype, "x-LoadControlSetpoint")) && (setsetpoint_respcode == 0))
		responseflag = true;
*/
	return responseflag;
} 

/**
 *==========================================================================
 * This function is called to send Set Commodity Subscription request.
 * To send the supported commodity types to the SGD.
 * @param 	Nothing
 * @return true - success
 * 		  false - invalid msg/response code error
 *==========================================================================
 */
 bool setCommoditySubscription()
 {
	int rtn, commodity;
	bool responseflag = false, stop = false;
	time_t cycleTime = 0, currentTimeSec = 0;

	commodity = 1;
	rtn = linkLayer();
	cycleTime = time(&currentTimeSec);
	while(stop == false) {

		currentTimeSec = time(&currentTimeSec);
		if (cycleTime <= currentTimeSec){
			switch(commodity){
			case 1:
					//Send Set commodity Subscription request to SGD Once
					if (linkData.txBlock == false){
						cycleTime = currentTimeSec + 10;									//Do next step in 10 seconds
						commodity = 2;
						logger("Log Datapoints: Issue Set COMMODITY SUBSCRIPTION request");
						//commotype1, updatefreq1, commotype2, updatefreq2, commotype3, updatefreq3
						rtn = CEA2045inter_Setcommodity(COMMODITY, 1, 0, 1, 6, 2, 7, 3);	//Issue Commodity Subscription request
						if (rtn == 0){
							inter.respOpCode1 = COMMODITY;
							inter.respOpCode2 = 0x81;
							inter.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else{
						cycleTime += 1;														//Wait 1 second and try again
					}
					break;

			default:
					stop = true;
					break;
			}
		}

		//Process pending inter message operations
		if (inter.timeoutTime > 0 && inter.timeoutTime < time(&currentTimeSec)) {
			logger("Timed out waiting for intermediate request response\n");
			linkData.rxCount = 0;
			inter.timeoutTime = 0;
		}
	}
	if(commoditysubscr_respcode == 0)
		responseflag = true;

	return responseflag;
}

/**
 *==========================================================================
 * This function is called to collect data points from sgd for logging and reporting.
 * Called from ReportHandler class of VEN manager.
 
 * @param 	status - Outside communication Status
 * @return true - success
 * 		  false - invalid msg/response code error
 *==========================================================================
 */
 
bool oadr_cea_logReportInfo(int status)
{
	FILE *logfp;
	bool responseflag = false, stop = false;
	time_t cycleTime = 0, currentTimeSec = 0;
	int rtn, datapoint = 1;
	basic.opState = 5;
	basic.online = false;

	commodityreadInfo.capcurrent = 0;
	commodityreadInfo.capmax = 0;
	commodityreadInfo.payload = 0;
	commodityreadInfo.respCode = 7;//0 for success  and 1 to 7 - error


	rtn = linkLayer();
	cycleTime = time(&currentTimeSec);
	while(stop == false){

		currentTimeSec = time(&currentTimeSec);
		if (cycleTime <= currentTimeSec){
			
			switch(datapoint){
			case 1:
					//Send communication status COMM_STATUS to SGD
					if (linkData.txBlock == false){
						cycleTime = currentTimeSec + 5;										//Do next step in 10 seconds
						datapoint = 2;
						logger("Log Datapoints: Issue OUTSIDE COMMUNICATION CONNECTION STATUS command (Opcode2 is connectionStatus)");
						rtn = CEA2045basic(COMM_STATUS, status, 0);							//Issue comm status command
						//Second parameters are:
						// 0 = No/Lost connection
						// 1 = Found/Good connection
						// 3 = Poor/unreliable connection
						if (rtn == 0){
							basic.appAckRtn = COMM_STATUS;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else{
						cycleTime += 1;											//Wait 1 second and try again
					}
					break;
			
			case 2:
					//Query Oper State of SGD
					if (linkData.txBlock == false) {
						cycleTime = currentTimeSec + 5;						//Do next step in 10 seconds
						datapoint = 3;												
						logger("Log Datapoints: Issue OPERATING STATE Request");
						rtn = CEA2045basic(OPER_STATE_REQ, 0, 0);										
						if (rtn == 0) {
							basic.pendingOpState = true;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else {
						cycleTime += 1;											//Wait 1 second and try again
					}
					break;
				
			case 3:
					//Send Get commodity Read request to SGD
					if (linkData.txBlock == false){
						cycleTime = currentTimeSec + 5;									//Do next step in 10 seconds
						datapoint = 4;
						logger("Log Datapoints: Issue COMMODITY READ request");
						rtn = CEA2045inter(COMMODITY, 0);								//Issue Commodity read request
						if (rtn == 0){
							inter.respOpCode1 = COMMODITY;
							inter.respOpCode2 = 0x80;
							inter.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else{
						cycleTime += 1;														//Wait 1 second and try again
					}
					break;	
							
			default:
			{
				//======================================================================================
				//write online, manualoverride, payload, capmax, capcurrent, operating state to log file
				//======================================================================================
				char Online[6], Override[6];
				if(basic.online)
					strcpy(Online,"true");
				else
					strcpy(Online,"false");

				if(basic.overRide)
					strcpy(Override,"true");
				else
					strcpy(Override,"false");

				logfp = fopen(dploggingfile, "a+");
				time_t t = time(0);
				struct tm * now = localtime( & t );
				if(basic.opState == 0)
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "Idle_Normal");
				else if(basic.opState == 1)
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "Running_Normal");
				else if(basic.opState == 2)
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							 commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "Running_Curtailed_Grid");
				else if(basic.opState == 3)
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "Running_Heightened_Grid");
				else if(basic.opState == 4)
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "Idle_Grid");
				else if(basic.opState == 5)
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "SGD_Error_Condition");
				else
					fprintf(logfp, "[%d-%d-%d/%d:%d:%d],%f,%s,%s,%f,%f,%s\n",
							(now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec),
							commodityreadInfo.payload, Online, Override, commodityreadInfo.capmax, commodityreadInfo.capcurrent, "Unused_state");
				fclose(logfp);
				stop = true;
				responseflag = true;
				break;
			}
			}
		}
		
		if (basic.timeoutTime > 0 && basic.timeoutTime < time(&currentTimeSec)) {
			//We have timed out waiting for a response
			logger("Timed out waiting for response");
			basic.appAckRtn = 0;
			linkData.rxCount = 0;
			basic.pendingOpState = false;
			basic.timeoutTime = 0;
		}
		
		//Process pending inter message operations
		if (inter.timeoutTime > 0 && inter.timeoutTime < time(&currentTimeSec)) {
			logger("Timed out waiting for intermediate request response\n");
			linkData.rxCount = 0;
			inter.timeoutTime = 0;
		}
	}
	return responseflag;
}
/**
 *==========================================================================
 * This function is called to check for customer overide/sleep/wake infinitely.
 * @param  Nothing
 * @return Nothing
 *==========================================================================
 */
 void checkCustOverride()
 {
	int rtn;

	while(1){
		rtn = linkLayer();
		if (linkData.rxCount > 0){
			if (rxBuffer[0] == 8) {
				// This is a basic or intermediate message
				if (rxBuffer[1] == 1) {
					// This is a basic message
					logger("Processing basic message");
					LinkAckSent = false;
					rtn = CEA2045basicRx();
				}
				else {
					// This is an intermediate message rxBuffer[1] == 2
					logger("Processing intermediate message");
					rtn = CEA2045interRx();
				}
			}
			else if (rxBuffer[0] == 9){
				logger("pass through message");
			}
		}
		//Process pending basic message operations
		if (linkData.txBlock == false && (basic.pendingAppAck == true || basic.pendingAppNak == true) && (LinkAckSent == true)) {
			if (basic.pendingAppAck == true) {
				logger("Issue Application Ack command");
				rtn = CEA2045basic(APP_ACK, basic.appAckSend, 0);							//Issue Application Ack reply
			}
			else {
				logger("Issue Application Nak command");
				rtn = CEA2045basic(APP_NAK, basic.appAckSend, 0);							//Issue Application Nak reply
			}
			basic.pendingAppAck = false;
			basic.pendingAppNak = false;
		}
	//	usleep(1000);
	}
 }


/**
 *==========================================================================
 * This function is called to send Outside Communication Status to SGD
 * @param 	status - Outside communication Status
 * @return - true if sent 
 * 			 otherwise false
 *==========================================================================
 */
bool sendCommstatus(int status)
{
	int rtn, stat;
	bool responseflag = false, stop = false;
	time_t cycleTime = 0, currentTimeSec = 0;
	stat = 1;
	AppAckstate = 0;

	rtn = linkLayer();
	cycleTime = time(&currentTimeSec);
	while(stop == false) {
		currentTimeSec = time(&currentTimeSec);
		if (cycleTime <= currentTimeSec) {
			switch(stat) {
			case 1:
					if (linkData.txBlock == false) {
						cycleTime = currentTimeSec + 10;
						stat += 1;												//Do next step in 10 seconds
						logger("Issue OUTSIDE COMMUNICATION CONNECTION STATUS command");
						rtn = CEA2045basic(COMM_STATUS, status, 0);	//Issue comm status command
						if (rtn == 0) {
							basic.appAckRtn = COMM_STATUS;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else {
						cycleTime += 1;																	//Wait 1 second and try again
					}
					break;
					
			default:
					stop = true;
					break;			
			}
		}
		
		if (basic.timeoutTime > 0 && basic.timeoutTime < time(&currentTimeSec)) {
			//We have timed out waiting for a response
			logger("Timed out waiting for response");
			basic.appAckRtn = 0;
			linkData.rxCount = 0;
			basic.pendingOpState = false;
			basic.timeoutTime = 0;
		}	
	}	

	if((linkData.nakVal == -1) && (basic.online == true))
		responseflag = true;
	return responseflag;
}

/**
 *==========================================================================
 * This function is called to query the SGDs operational state
 * @param No parameters
 * @return - current operational state
 *==========================================================================
 */
 
//TODO: Handle SLEEPSTATE
int getOperationalState()
{
	int rtn, opstate = 0,oper;
	bool stop = false;
	time_t cycleTime = 0, currentTimeSec = 0;
	oper = 1;
	rtn = linkLayer();
	cycleTime = time(&currentTimeSec);
	while(stop == false){
		currentTimeSec = time(&currentTimeSec);
		if (cycleTime <= currentTimeSec){
			switch(oper){
			case 1:
					if (linkData.txBlock == false) {
						cycleTime = currentTimeSec + 10;						//Do next step in 10 seconds
						oper += 1;												
						logger("Issue OPERATIONAL STATE Request");
						rtn = CEA2045basic(OPER_STATE_REQ, 0, 0);										
						if (rtn == 0) {
							basic.pendingOpState = true;
							basic.timeoutTime = time(&currentTimeSec) + 4;
						}
					}
					else {
						cycleTime += 1;											//Wait 1 second and try again
					}
					break;
					
			default:
					stop = true;
					break;
			}
		}
		
		if (basic.timeoutTime > 0 && basic.timeoutTime < time(&currentTimeSec)) {
			//We have timed out waiting for a response
			logger("Timed out waiting for response");
			basic.appAckRtn = 0;
			linkData.rxCount = 0;
			basic.pendingOpState = false;
			basic.timeoutTime = 0;
		}	
	}
	opstate = (int)basic.opState;
	return opstate;
}


/**
 *==========================================================================
 * This function is called to get device information.
 * @param No parameter
 * @return boolean value - true on recieving expected response on time.
 * 						   otherwise false
 *==========================================================================
 */
 
 //TODO: Handle SLEEPSTATE/CUSTOVERRIDE
 bool getDeviceInfo()
{
	int rtn,dev;
	bool responseflag = false, stop = false;
	time_t cycleTime = 0, currentTimeSec = 0;
	
	dev = 1;
	rtn = linkLayer();

	cycleTime = time(&currentTimeSec);
	while(stop == false) {
		currentTimeSec = time(&currentTimeSec);
		if (cycleTime <= currentTimeSec) {
			switch(dev){
			case 1:
					//Send Get device information request from SGD
					if (linkData.txBlock == false){
						dev +=1;
						cycleTime = currentTimeSec + 10;										//Do next step in 10 seconds
						logger("Issue DEVICE INFO request");
						rtn = CEA2045inter(INFO_REQ, 1);			
						if (rtn == 0){
							inter.respOpCode1 = INFO_REQ;
							inter.respOpCode2 = 0x81;
							inter.timeoutTime = time(&currentTimeSec) + 4;
						}
					}	
					else{
						cycleTime += 1;															//Wait 1 second and try again
					}
					break;
					
			default:
					stop = true;
					break;
			}
		}
		
		//Process pending inter message operations
		if (inter.timeoutTime > 0 && inter.timeoutTime < time(&currentTimeSec)) {
			logger("Timed out waiting for intermediate request response");
			linkData.rxCount = 0;
			inter.timeoutTime = 0;	
			inter.respOpCode1 = 0;
			inter.respOpCode2 = 0;		
		}
	}
	if(devInfo.respCode == 0)
		responseflag = true;
	return responseflag;
}

/**
 *==========================================================================
 * This function is called to process basic messages received from the SGD
 * Syntax is:
 * int rtnval = CEA2045basicRx();
 * @return command ID on success
 * @return -1 on invalid message received
 *==========================================================================
 */
int CEA2045basicRx()
{
	int rtn = 0;
	
	basic.opCode1 = rxBuffer[4];
	basic.opCode2 = rxBuffer[5];

	//Check if Application Ack is pending
	if (basic.appAckRtn > 0) {
		//Verify Application Ack value
		if (rxBuffer[4] == APP_ACK) {
			basic.online = true;
			if (rxBuffer[5] == basic.appAckRtn) {
				//Proper application ack was returned
				rtn = rxBuffer[4];
				AppAckstate = 1;
				logger("Application ACK received");
			}
			else {
				//Wrong application ack was returned
				logger("Wrong Application ACK received");
				rtn = -1;
			}
		}
		//*******************************************
		//Verify Application Nak value -- Newly added
		//*******************************************
		if (rxBuffer[4] == APP_NAK) 
		{
			rtn = rxBuffer[4];
			if (rxBuffer[5] == 0x00) 		
				logger("Application NAK received -- No reason given");
			else if(rxBuffer[5] == 0x01)
				logger("Application NAK received -- Opcode1 not supported");
			else if(rxBuffer[5] == 0x02)
				logger("Application NAK received -- Opcode2 invalid	");
			else if(rxBuffer[5] == 0x03)
				logger("Application NAK received -- Busy");
			else if(rxBuffer[5] == 0x04)
				logger("Application NAK received -- Length Invalid");	
			else 
			{
				//Wrong application nak was returned
				logger("Wrong Application NAK received");
				rtn = -1;
			}
		}
	}
	else if (basic.pendingOpState == true) {
		//Get returned operational state
		if (rxBuffer[4] == OPER_STATE_RESP) {
			//Operational state was returned
			basic.opState =  rxBuffer[5];
			rtn = rxBuffer[4];
			logger("Operational state received");
		}
		else {
			logger("Operational state not received");
			rtn = -1;
		}
	}
	else if (rxBuffer[4] == CUST_OVERRIDE) {
		//Operator Override state received - rxBuffer[5] = 0 is override disabled, 1 is override enabled
		if (rxBuffer[5] == 0) {
			basic.overRide = false;
			logger("CUSTOMER OVERRIDE Not in effect");
		}
		else {
			basic.overRide = true;
			logger("CUSTOMER OVERRIDE set");
		}
		//Send application Ack/Nak
		rtn = rxBuffer[4];
		if((rxBuffer[5] == 0) || (rxBuffer[5] == 1))
		{
			if((rxBuffer[2] == 0x00) && (rxBuffer[3] == 0x02))
			{
				basic.pendingAppAck = true;
				basic.appAckSend = CUST_OVERRIDE;			
			}
			else
			{
				//NAK for Length invalid
				logger("Customer Override - Length Invalid");
				basic.pendingAppNak = true;
				basic.appAckSend = 0x04;	
			}
		}
		else
		{
			//NAK for opcode2 invalid
			logger("Customer Override - opcode2 invalid");
			basic.pendingAppNak = true;
			basic.appAckSend = 0x02;
		}
	}
	else if (rxBuffer[4] == SLEEP_STATE) {
		//Sleep command received from SGD
		basic.sleepState = true;
		logger("SLEEP command received");
		//Send application Ack/Nak
		rtn = rxBuffer[4];
		if(rxBuffer[5] == 0)
		{
			if((rxBuffer[2] == 0x00) && (rxBuffer[3] == 0x02))
			{
				basic.pendingAppAck = true;
				basic.appAckSend = SLEEP_STATE;
			}
			else
			{
				//NAK for Length invalid
				logger("Sleep command - Length invalid");
				basic.pendingAppNak = true;
				basic.appAckSend = 0x04;		
			}
		}
		else
		{
			//NAK for opcode2 invalid
			logger("Sleep command - opcode2 invalid");
			basic.pendingAppNak = true;
			basic.appAckSend = 0x02;
		}
	}
	else if (rxBuffer[4] == WAKE_STATE) {
		//Wake command received from SGD
		basic.sleepState = false;
		logger("WAKE command received");
		//Send application Ack
		rtn = rxBuffer[4];
		if(rxBuffer[5] == 0)
		{
			if((rxBuffer[2] == 0x00) && (rxBuffer[3] == 0x02))
			{
				basic.pendingAppAck = true;
				basic.appAckSend = WAKE_STATE;		
			}
			else
			{
				//NAK for Length invalid
				logger("Wake command - Length invalid");
				basic.pendingAppNak = true;
				basic.appAckSend = 0x04;			
			}
		}
		else
		{
			//NAK for opcode2 invalid
			logger("Wake command - opcode2 invalid");
			basic.pendingAppNak = true;
			basic.appAckSend = 0x02;
		}
	}

	//TODO: NAK for '0x00 = No reason given' and '0x03 = Busy'	
	basic.appAckRtn = 0;
	linkData.rxCount = 0;
	basic.pendingOpState = false;
	basic.timeoutTime = 0;
 
	return rtn;
} 			

/**
 *==========================================================================
 * This function is called to process intermediate messages received from
 * the SGD
 * Syntax is:
 * int rtnval = CEA2045interRx();
 * @return command ID on success
 * @return -1 on invalid message received
 *==========================================================================
 */
int CEA2045interRx()
{
	int rtn = 0, commodityIndex;
	int messageSize;
	setenergyprice_respcode = -1;
	setsetpoint_respcode = -1;
	settempoffset_respcode = -1;
	commoditysubscr_respcode = -1;
	devInfo.respCode = -1;
	commodityreadInfo.respCode = -1;
	
	//Check if response is pending
	if (inter.respOpCode1 != 0) 
	{
		 //Verify response codes
		if (rxBuffer[6] == 0x00)
		{
			//This is a valid response
			//Verify Response opcodes
			if (rxBuffer[4] == inter.respOpCode1 && rxBuffer[5] == inter.respOpCode2)
			{
				logger("response code 0x00 -- success");
				// Calculate message size
				messageSize = rxBuffer[2] *256 + rxBuffer[3];
				if (rxBuffer[4] == INFO_REQ && rxBuffer[5] == 0x81) 
				{
					//This is a device information response
					logger("device information response recieved");
					devInfo.respCode = rxBuffer[6];		//Response code
					devInfo.ceaVer[0] = rxBuffer[7];	//CEA-2045 version
					devInfo.ceaVer[1] = rxBuffer[8];	//CEA-2045 version
					devInfo.vendorID = rxBuffer[9] * 256 + rxBuffer[10];	//Vendor ID integer
					devInfo.devType = rxBuffer[11] * 256 + rxBuffer[12];	//Device type
					devInfo.devRev = rxBuffer[13] * 256 + rxBuffer[14];	//Device revision
					devInfo.capBitmap = ((unsigned long)rxBuffer[15] << 24) + ((unsigned long)rxBuffer[16] << 16) + ((unsigned long)rxBuffer[17] << 8) + rxBuffer[18];	//Capability bitmap
					devInfo.reserve = rxBuffer[19];		//Reserved
					if (messageSize >= 32) {
						memcpy(&devInfo.modelNum[0],&rxBuffer[20],16);	//Model number
					}
					if (messageSize >= 48) {
						memcpy(&devInfo.serialNum[0],&rxBuffer[36],16);	//Serial number
					}
					if (messageSize >= 53) {
						devInfo.firmYear = rxBuffer[52];	//Firmware year
						devInfo.firmMonth = rxBuffer[53];	//Firmware month
						devInfo.firmDay = rxBuffer[54];		//Firmware day
						devInfo.firmMajor = rxBuffer[55];	//Firmware major
						devInfo.firmMinor = rxBuffer[56];	//Firmware minor
					}
					inter.respOpCode1 = 0;
					inter.timeoutTime = 0;
					rtn = rxBuffer[4];
				}
				//newly added case
				else if (rxBuffer[4] == COMMODITY && rxBuffer[5] == 0x80) 
				{
					logger("Commodity read request response recieved");
					commodityreadInfo.respCode = rxBuffer[6];		//Response code
					commodityreadInfo.commoditycode[0] = rxBuffer[7];
					commodityreadInfo.instantaneousrate[0] = ((uint64_t)rxBuffer[8] << 40) + ((uint64_t)rxBuffer[9] << 32) + ((unsigned long)rxBuffer[10] << 24) + ((unsigned long)rxBuffer[11] << 16) + ((unsigned long)rxBuffer[12] << 8) + rxBuffer[13];
					commodityreadInfo.cumulativeamount[0] = ((uint64_t)rxBuffer[14] << 40) + ((uint64_t)rxBuffer[15] << 32) + ((unsigned long)rxBuffer[16] << 24) + ((unsigned long)rxBuffer[17] << 17) + ((unsigned long)rxBuffer[18] << 8) + rxBuffer[19];
					
					commodityreadInfo.commoditycode[1] = rxBuffer[20];
					commodityreadInfo.instantaneousrate[1] = ((uint64_t)rxBuffer[21] << 40) + ((uint64_t)rxBuffer[22] << 32) + ((unsigned long)rxBuffer[23] << 24) + ((unsigned long)rxBuffer[24] << 16) + ((unsigned long)rxBuffer[25] << 8) + rxBuffer[26];
					commodityreadInfo.cumulativeamount[1] = ((uint64_t)rxBuffer[27] << 40) + ((uint64_t)rxBuffer[28] << 32) + ((unsigned long)rxBuffer[29] << 24) + ((unsigned long)rxBuffer[30] << 16) + ((unsigned long)rxBuffer[31] << 8) + rxBuffer[32];
					
					commodityreadInfo.commoditycode[2] = rxBuffer[33];
					commodityreadInfo.instantaneousrate[2] = ((uint64_t)rxBuffer[34] << 40) + ((uint64_t)rxBuffer[35] << 32) + ((unsigned long)rxBuffer[36] << 24) + ((unsigned long)rxBuffer[37] << 16) + ((unsigned long)rxBuffer[38] << 8) + rxBuffer[39];
					commodityreadInfo.cumulativeamount[2] = ((uint64_t)rxBuffer[40] << 40) + ((uint64_t)rxBuffer[41] << 32) + ((unsigned long)rxBuffer[42] << 24) + ((unsigned long)rxBuffer[43] << 16) + ((unsigned long)rxBuffer[44] << 8) + rxBuffer[45];
					
					for(commodityIndex = 0; commodityIndex < 3; commodityIndex++){
						if((commodityreadInfo.commoditycode[commodityIndex] == 0x00) || (commodityreadInfo.commoditycode[commodityIndex] == 0x80)){
							commodityreadInfo.payload = commodityreadInfo.cumulativeamount[commodityIndex];
						}
						else if((commodityreadInfo.commoditycode[commodityIndex] == 0x06) || (commodityreadInfo.commoditycode[commodityIndex] == 0x86)){
							commodityreadInfo.capmax = commodityreadInfo.cumulativeamount[commodityIndex];
						}
						else if((commodityreadInfo.commoditycode[commodityIndex] == 0x07) || (commodityreadInfo.commoditycode[commodityIndex] == 0x87)){
							commodityreadInfo.capcurrent = commodityreadInfo.cumulativeamount[commodityIndex];
						}
					}
					inter.respOpCode1 = 0;
					inter.timeoutTime = 0;
					rtn = rxBuffer[4];
				}
				else if (rxBuffer[4] == COMMODITY && rxBuffer[5] == 0x81)
				{
					logger("Set commodity Subscription response recieved\n");
					commoditysubscr_respcode = rxBuffer[6];
					inter.respOpCode1 = 0;
					inter.timeoutTime = 0;
					rtn = rxBuffer[4];
				} 
				else if (rxBuffer[4] == ENERGY_PRICE && rxBuffer[5] == 0x80) 
				{
					logger("set Energy Price response recieved");
					setenergyprice_respcode = rxBuffer[6];		//Response code
					inter.respOpCode1 = 0;
					inter.timeoutTime = 0;
					rtn = rxBuffer[4];
				}
				else if (rxBuffer[4] == TEMP_OFFSET && rxBuffer[5] == 0x82) 
				{
					logger("set Temperature Offset response recieved");
					settempoffset_respcode = rxBuffer[6];		//Response code
					inter.respOpCode1 = 0;
					inter.timeoutTime = 0;
					rtn = rxBuffer[4];
				}
				else if (rxBuffer[4] == SETPOINT && rxBuffer[5] == 0x83) 
				{
					logger("set Set Point response recieved");
					setsetpoint_respcode = rxBuffer[6];		//Response code
					inter.respOpCode1 = 0;
					inter.timeoutTime = 0;
					rtn = rxBuffer[4];
				}

			}
		}
		else if(rxBuffer[6] == 0x01){
			rtn = -1;
			logger("response code 0x01 -- Command not implemented");
		}
		else if(rxBuffer[6] == 0x02){
			rtn = -1;
			logger("response code 0x02 -- Bad Value – one or more values in the message are invalid");
		}
		else if(rxBuffer[6] == 0x03){
			rtn = -1;
			logger("response code 0x03 -- Command Length Error – command is too long");
		}
		else if(rxBuffer[6] == 0x04){
			rtn = -1;
			logger("response code 0x04 -- Response Length Error – response is too long");
		}
		else if(rxBuffer[6] == 0x05){
			rtn = -1;
			logger("response code 0x05 -- Busy");
		}
		else if(rxBuffer[6] == 0x06){
			rtn = -1;
			logger("response code 0x06 -- other");
		}
		linkData.rxCount = 0;
	}
 
	return rtn;
} 			

/**
 *==========================================================================
 * This function is called to send an Intermediate message to the SGD
 * Syntax is:
 * int rtnval = CEA2045inter(byte opcode1,byte opcode2);
 * @return 0 on success
 * @return 1 on failure to interpret a value
 * @return 2 on routine called when blocked
 *==========================================================================
 */
int CEA2045inter(unsigned char opcode1,unsigned char opcode2)
{
	bool valid;
	int rtnval;
	
	if (linkData.txBlock == true) {
		return 2;					//Routine called when blocked	
	}
	valid = true;
	rtnval = 0;
	txBuffer[3] = 0x02;
	linkData.txCount = 6;

	if (valid == true) {
		expectedMsgSize = 2;		//Set size of expected message
		txBuffer[0] = 0x08;			//Message type
		txBuffer[1] = 0x02;
		txBuffer[2] = 0x00;			//Payload length
		txBuffer[3] = 0x02; 
		txBuffer[4] = opcode1;		//Message opcode
		txBuffer[5] = opcode2;		//Message operand
	}
	return rtnval;
}

/**
 *==========================================================================
 * This function is called to send an Set Commodity Subscrition message to the SGD
 * Syntax is:
 * int rtnval = CEA2045inter(byte opcode1,byte opcode2);
 * @return 0 on success
 * @return 2 on routine called when blocked
 *==========================================================================
 */
int CEA2045inter_Setcommodity(unsigned char opcode1,unsigned char opcode2, int commtype1, int ufreq1,
							  int commtype2, int ufreq2, int commtype3, int ufreq3)
{
	bool valid;
	int rtnval;
	
	if (linkData.txBlock == true) {
		return 2;					//Routine called when blocked	
	}
	valid = true;
	rtnval = 0;
	txBuffer[3] = 0x0B;
	linkData.txCount = 15;

	if (valid == true) {
		expectedMsgSize = 11;		//Set size of expected message
		txBuffer[0] = 0x08;			//Message type
		txBuffer[1] = 0x02;
		txBuffer[2] = 0x00;			//Payload length
		txBuffer[3] = 0x0B; 
		txBuffer[4] = opcode1;		//Message opcode
		txBuffer[5] = opcode2;		//Message operand
		txBuffer[6] = commtype1;
		txBuffer[7] = ufreq1 >> 8;
		txBuffer[8] = ufreq1 & 0xff;
		txBuffer[9] = commtype2;
		txBuffer[10] = ufreq2 >> 8;
		txBuffer[11] = ufreq2 & 0xff;
		txBuffer[12] = commtype3;
		txBuffer[13] = ufreq3 >> 8;
		txBuffer[14] = ufreq3 & 0xff;
	}
	return rtnval;

}


/**
 *==========================================================================
 * This function is called to send Set Energy Price message to the SGD
 * to set the energy price
 * Syntax is:
 * int rtnval = CEA2045inter_SetEnergyPrice(unsigned char opcode1,unsigned char opcode2, uint32_t currentPrice, uint32_t currencyCode, uint digitsafterDecimal);
 * @return 0 on success
 * @return 1 on failure to interpret a value
 * @return 2 on routine called when blocked
 *==========================================================================
 */
int CEA2045inter_SetEnergyPrice(unsigned char opcode1,unsigned char opcode2, uint32_t currentPrice, uint32_t currencyCode,
								uint digitsafterDecimal)
{
	bool valid;
	int rtnval;

	if (linkData.txBlock == true) {
		return 2;		//Routine called when blocked
	}
	valid = true;
	rtnval = 0;
	txBuffer[3] = 0x07;
	linkData.txCount = 11;
	
	if (valid == true) {
		expectedMsgSize = 7;		//Set size of expected message
		txBuffer[0] = 0x08;			//Message type
		txBuffer[1] = 0x02;
		txBuffer[2] = 0x00;			//Payload length
		txBuffer[3] = 0x07;
		txBuffer[4] = opcode1;		//Message opcode
		txBuffer[5] = opcode2;		//Message operand
		txBuffer[6] = (currentPrice >> 8);
		txBuffer[7] = (currentPrice & 0xff);
		txBuffer[8] = (currencyCode >> 8);
		txBuffer[9] = (currencyCode & 0xff);
		txBuffer[10] = digitsafterDecimal;
	}
	return rtnval;

}
/**
 *==========================================================================
 * This function is called to send Set temperature Offset message to the SGD
 * Syntax is:
 * int rtnval = CEA2045inter_SetTempOffset(unsigned char opcode1,unsigned char opcode2,uint currentOffset);
 * @return 0 on success
 * @return 1 on failure to interpret a value
 * @return 2 on routine called when blocked
 *==========================================================================
 */
int CEA2045inter_SetTempOffset(unsigned char opcode1,unsigned char opcode2, uint currentOffset, uint unit)
{
	bool valid;
	int rtnval;

	if (linkData.txBlock == true) {
		return 2;		//Routine called when blocked
	}
	valid = true;
	rtnval = 0;
	txBuffer[3] = 0x04;
	linkData.txCount = 8;
	
	if (valid == true) {
		expectedMsgSize = 3;		//Set size of expected message
		txBuffer[0] = 0x08;			//Message type
		txBuffer[1] = 0x02;
		txBuffer[2] = 0x00;			//Payload length
		txBuffer[3] = 0x04;
		txBuffer[4] = opcode1;		//Message opcode
		txBuffer[5] = opcode2;		//Message operand
		txBuffer[6] = currentOffset;
		txBuffer[7] = unit;
	}
	return rtnval;
}
/**
 *==========================================================================
 * This function is called to send an inter message to the SGD
 * to set set point
 * Syntax is:
 * int rtnval = CEA2045inter_SetSetPoint(unsigned char opcode1, unsigned char opcode2, uint16_t devType, unsigned char units, int16_t setpoint1);
 * @return 0 on success
 * @return 1 on failure to interpret a value
 * @return 2 on routine called when blocked
 *==========================================================================
 */
int CEA2045inter_SetSetPoint(unsigned char opcode1, unsigned char opcode2, uint16_t devType, int units, int16_t setpoint1, int16_t setpoint2)
{
	bool valid;
	int rtnval;

	if (linkData.txBlock == true) {
		return 2;		//Routine called when blocked
	}
	valid = true;
	rtnval = 0;
	txBuffer[3] = 0x09;
	linkData.txCount = 13;
	
	if (valid == true) {
		expectedMsgSize = 7;		//Set size of expected message
		txBuffer[0] = 0x08;			//Message type
		txBuffer[1] = 0x02;
		txBuffer[2] = 0x00;			//Payload length
		txBuffer[3] = 0x09;
		txBuffer[4] = opcode1;		//Message opcode
		txBuffer[5] = opcode2;		//Message operand
		txBuffer[6] = (devType >> 8);
		txBuffer[7] = (devType & 0xff);
		txBuffer[8] = units;
		txBuffer[9] = (setpoint1 >> 8);
		txBuffer[10] = (setpoint1 & 0xff);
		txBuffer[11] = (setpoint2 >> 8);
		txBuffer[12] = (setpoint2 & 0xff);
	}
	return rtnval;

}


/**
 *==========================================================================
 * This function is called to send a Basic message to the SGD
 * Syntax is:
 * int rtnval = CEA2045basic(byte COMMAND,int cmdparam);
 * @return 0 on success
 * @return 1 on failure to interpret a value
 * @return 2 on routine called when blocked
 *==========================================================================
 */
int CEA2045basic(unsigned char cmd,long cmdparam, float cmdparamf)
{
	unsigned char opcode1 = 0;		// opcode1
	unsigned char opcode2 = 0;		// opcode2
	long tempval;
	bool valid;
	int rtnval;

	if (linkData.txBlock == true) {
		return 2;		//Routine called when blocked	
	}
	valid = true;
	rtnval = 0;
	switch(cmd) {
		case SHED:
		case TIME_REMAINING_PRICE:
		case CPP:
		case GRID_EMERGENCY:
		case LOADUP:
			opcode1 = cmd;
			tempval = sqrt(cmdparam/2);
			if (tempval > 0xff) opcode2 = 0xff;
			else opcode2 = tempval;
			if (cmd != TIME_REMAINING_PRICE) {
				shedEndTime = time(&currentTimeSec) + cmdparam;
			} 
			break;
		case END_SHED:		
			opcode1 = cmd;
			opcode2 = 0;
			shedEndTime = 0;
			break;
		case COMM_STATUS:		
		case OPER_STATE_REQ:		
		case SIMPLE_TIME_SYNC:		
		case APP_ACK:
		case APP_NAK:
		case GRID_GUIDANCE:
			opcode1 = cmd;
			opcode2 = cmdparam;
			break;
		case PRESENT_RELATIVE_PRICE:
		case NEXT_RELATIVE_PRICE:
			opcode1 = cmd;
			opcode2 = calcRelativePriceByte(cmdparamf);
			break;
		case POWER_LEVEL:
			opcode1 = cmd;
			if (cmdparam > 0) {		// Power absorbed
				if (cmdparam > 127) {
					opcode2 = 127;
				}
				else
					opcode2 = cmdparam;	//case newly added
			}
			else {					// Power produced
				cmdparam = -cmdparam;
				if (cmdparam > 127) {
					opcode2 = 127; 	
				}
				else
					opcode2 = cmdparam; //case newly added
				opcode2 = opcode2 || 0x80;
			} 			
			break;
		default:
			valid = false;
			break;
	}	
	if (valid == true) {
//		linkData.txBlock = false;	//Prevent any new messages until this completes	
		expectedMsgSize = 2;		//Set size of expected message
		txBuffer[0] = 0x08;			//Message type
		txBuffer[1] = 0x01;
		txBuffer[2] = 0x00;			//Payload length (always 2)
		txBuffer[3] = 0x02; 
		txBuffer[4] = opcode1;		//Message opcode
		txBuffer[5] = opcode2;		//Message operand
		linkData.txCount = 6;
	}
	return rtnval;
}

/**
 *==========================================================================
 * Link layer functionality. Trigger this routine every millisecond or so.
 * If desired this routine and its supporting subroutines can be made a 
 * separate process, thread or interrupt routine. Data could then be passed
 * via shared memory, pipe, etc.
 * @return 0 on success
 * @return 1 on link ack received
 * @return -1 on link nak - nak value in linkData.nakVal
 * @return -2 on link ack/nak timeout
 *==========================================================================
 */
int linkLayer()
{
	int newChars=0;
	int payld;
	int ret_val;
	if (linkData.sunSpec == false) {
		// This is a normal CEA2045 SGD device
		//Get any new characters from SGD
		if (linkData.rxCount == 0) {		// Only check for new received data if app layer is done with last message
			// Check for received data 
			FD_ZERO(&read_fds);
			FD_SET(cea2045_fd, &read_fds);
			select_timeout.tv_sec = 0;
			select_timeout.tv_usec = 1000;

			newChars = read(cea2045_fd, &rxBuffer[rxCount], sizeof(rxBuffer - rxCount));

			if (newChars > 0) {
				rxCount += newChars;
				if (pendingLinkAck == true) {
					//If waiting for link ack
					if (rxCount > 1) {
						//At least 2 characters have been received so check for link ack/nak
						addTimeOffset(&txDelayTime, 100000);
						linkData.txBlock = true;
						pendingLinkAck = false;
						expectedMsgSize = 0;
						rxCount = 0;
						txStartTime.tv_sec = 0;
						if (rxBuffer[0] == 6 && rxBuffer[1] == 0) {
							logger("Recieved link ack");
							//Link ack received
							linkData.nakVal = -1;
							return 1;
						}
						else if (rxBuffer[0] == 0x15) {
							//Link nak received
							linkData.nakVal = rxBuffer[1];
							return -1;
						}
					}
					
				}
				else {
					if (rxCount == newChars) {
						//Start of a received message
						gettimeofday(&rxStartTime, NULL);
					}
					if (expectedMsgSize == 0) {
						//Message size has not been determined
						if ((rxBuffer[0] == 0x08 || rxBuffer[0] == 0x09) && rxCount > 3) {
							// enough characters have been received to calculate message size
							expectedMsgSize = (rxBuffer[2] * 256) + rxBuffer[3] + 6;
						}
					}
					if (expectedMsgSize > 0 && rxCount >= expectedMsgSize) {
						// Complete message received - validate checksum
						rxStartTime.tv_sec = 0;
						if (calcChecksum(rxBuffer, expectedMsgSize - 2, true) == 0) {
							linkAckBuff[0] = 0x06;
							linkAckBuff[1] = 0x0;
							linkData.rxCount = expectedMsgSize -2;
							rxCount = 0;
							expectedMsgSize = 0;
							addTimeOffset(&pendingTxTime, 40000);		//send ack in 40ms (tMA)
							pendingTxLinkAck = true;
							linkData.txBlock = true;
						}
						else {
							linkAckBuff[0] = 0x15;
							linkAckBuff[1] = 0x03;			// Nak for bad checksum
							rxCount = 0;					// Flush receive buffer
							expectedMsgSize = 0;
							addTimeOffset(&pendingTxTime, 40000);		//send ack in 40ms (tMA)
							pendingTxLinkAck = true;
							linkData.txBlock = true;
						}
						//===============================================
						// add additional code for other link errors
						//===============================================
					}
				}
			}
			else if (rxStartTime.tv_sec > 0){
				//Check for timeout on message
				if (calcTimeDiff(&rxStartTime) > 100) {
					//It has been over 100ms since start of message so nak message
					rxStartTime.tv_sec = 0;
					linkAckBuff[0] = 0x15;
					linkAckBuff[1] = 0x05;			// Nak for message timeout
					rxCount = 0;					// Flush receive buffer
					expectedMsgSize = 0;
					if(ucmtype == 0)
					{
						system("/bin/echo \"1\" > /sys/class/gpio/gpio21/value");
					}
					if((write(cea2045_fd, &linkAckBuff[0], 2)) == -1)	// Write a link nak to the SGD
						logger("Error writing link Ack/Nak to comm port");
					if(ucmtype == 0)
					{
						system("/bin/echo \"0\" > /sys/class/gpio/gpio21/value");
					}
					addTimeOffset(&txDelayTime, 100000);		//Don't send messages for 100ms
					linkData.txBlock = true;
				}
			}
			else if (pendingLinkAck == true) {
				//Check for timeout waiting on link ack/nak
				if (calcTimeDiff(&txStartTime) > 120) {
					//It has been over 120ms since message was sent so timeout
					txStartTime.tv_sec = 0;
					pendingLinkAck = false;
					rxCount = 0;					// Flush receive buffer
					expectedMsgSize = 0;
					linkData.txBlock = false;
					return -2;
				}
			}
			if (pendingTxLinkAck == true) {
				//Check if it is time to send pending link ack/nak
				if (calcTimeDiff(&pendingTxTime) >= 0) {
					logger("Sending Link Ack/Nak");
					LinkAckSent = true;
					if(ucmtype == 0)
					{
						system("/bin/echo \"1\" > /sys/class/gpio/gpio21/value");
					}
					if((write(cea2045_fd, &linkAckBuff[0], 2)) == -1)	// Write a link nak to the SGD
						logger("Error writing link Ack/Nak to comm port");
					if(ucmtype == 0)
					{
						system("/bin/echo \"0\" > /sys/class/gpio/gpio21/value");
					}
					pendingTxLinkAck = false;
					addTimeOffset(&txDelayTime, 100000);		//Don't send messages for 100ms --> tIM
					linkData.txBlock = true;
				} 
			}
		}
		if (linkData.txBlock == true) {
			//Check if txDelayTime has expired
			if (calcTimeDiff(&txDelayTime) >= 0) {
				txDelayTime.tv_sec = 0;		//Clear the block on transmitting messages
				linkData.txBlock = false;
			}
		}
		if (linkData.txBlock == false && linkData.txCount > 0) {
			// Transmit data to SGD
			if (txStartTime.tv_sec == 0) {
				// This is a new message to be sent
				calcChecksum(txBuffer, linkData.txCount, false);
				if(ucmtype == 0)
				{
					system("/bin/echo \"1\" > /sys/class/gpio/gpio21/value");
				}
				if((write(cea2045_fd, &txBuffer[0], linkData.txCount + 2)) == -1)	// Write data in txBuffer to the CEA 2045 port
					logger("Error writing Message to comm port");
				if(ucmtype == 0)
				{
					system("/bin/echo \"0\" > /sys/class/gpio/gpio21/value");
				}
				//for capturing the messages sent to sgd and writing to dplogging csv file
				//============================================================================
				pthread_mutex_lock(&logfilemutex);
				fp = fopen(CEA2045logFile, "a+");
				fprintf(fp, "\t     Payload: ");
				for(payld = 0; payld < linkData.txCount + 2; payld++){
					fprintf(fp, "/0x%x", txBuffer[payld]);
				}
				fprintf(fp,"\n");
				fclose(fp);
				pthread_mutex_unlock(&logfilemutex);
				//============================================================================
				gettimeofday(&txStartTime, NULL);
				addTimeOffset(&txDelayTime, 150000);		//Don't send messages for 150ms or until Link ack received
				linkData.txBlock = true;
				linkData.txCount = 0;
				pendingLinkAck = true;
				expectedMsgSize = 2;		//Set size of expected message
			}
		}
	}
	else {
		// Put SunSpec code here for CSI project
	}
	return 0;
}


/**
 ***************************************************************************
 * This function calculates time in milliseconds from passed value to now. 
 * A negative number says the passed time is still in the future.
 * A positive number says the passed time is still in the past.
 *
 * @param compareTime  A pointer to a timeval structure.
 * @return long int number of milliseconds
 ***************************************************************************
 */
long int calcTimeDiff(struct timeval *compareTime) {
long int t1;

	gettimeofday(&currentTime, NULL);
	t1 = (currentTime.tv_sec - compareTime->tv_sec) * 1000 + ((currentTime.tv_usec - compareTime->tv_usec) / 1000);
	return t1;
}

/**
 ******************************************************************************
 * This function adds a passed number of microseconds to now in timeval struct 
 *
 * @param timestruct  A pointer to a timeval structure
 * @param adder  Number of micriseconds to add to current time.
 ******************************************************************************
 */
void addTimeOffset(struct timeval *timestruct, long int adder) {

	gettimeofday(timestruct, NULL);
	timestruct->tv_usec += adder;
	while (timestruct->tv_usec > 999999) {
		timestruct->tv_sec += 1;
		timestruct->tv_usec -= 1000000;
	}
}

/**
 ******************************************************************************
 * This calculates the byte value for the relative price function formula:
 * Byte value = (-62 + SQRT(4096 + (32768*RP))) / 2 
 *
 * @param relativePrice float ratio of current price / normal price
 ******************************************************************************
 */

unsigned char calcRelativePriceByte(float relativePrice) {
	float f1;
	
	if (relativePrice > 9.825) {
		return 0xff;
	}
	else if (relativePrice < 0) {
		return 0x0;
	}
	f1 = (-62 + sqrt(4096 + (32768 * relativePrice))) / 2;
	return (int) (f1+.05);

}

/**
 ***************************************************************************
 * This function calculates the check sum bytes for the passed basic serial 
 * message and attaches it to the end of the message. 
 *
 * @param buffData  A pointer to the message string string.
 * @param msgSize  Size of the message without checksum.
 * @param validate True if this is a checksum test, false otherwise.
 * @return 0 on success
 * @return -1 if checksum fails validation
 ***************************************************************************
 */
int calcChecksum(unsigned char* buffData, int msgSize, bool validate) {
	// Message is in array buffData
	int i;
	int checksum1 = 0xaa;
	int checksum2 = 0;

	for (i = 0; i < msgSize; i++) {
		checksum1 += buffData[i];
		checksum1 = checksum1 % 0xff;
		checksum2 += checksum1;
		checksum2 = checksum2 % 0xff;
	}
	if (validate != true) {
		// Copy checksum to end of buffer
		buffData[msgSize] = 255 - ((checksum1 + checksum2) % 255);
		buffData[msgSize + 1] = 255 - ((checksum1 + buffData[msgSize]) % 255);
		return 0;
	}
	else {
		// Verify the last two characters of buffer match the checksum calculated
		if (buffData[msgSize] == 255 - ((checksum1 + checksum2) % 255)) {
			if (buffData[msgSize + 1] == 255 - ((checksum1 + buffData[msgSize]) % 255)) {
				return 0;
			}
		}
		return -1;
	}
}

/**
 ***************************************************************************
 * Initialize the serial port 
 *
 * @return file descriptor for com port
 ***************************************************************************
 */
int initSerialPort(char commport[])
{
	int tty_fd = -1;
	//check_ucmtype();
	//tty_fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	logger("Trying to connect to comm port\n");
	tty_fd = open(commport, O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (tty_fd == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		logger("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}
	
	tcgetattr(tty_fd, &tio);
	tio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	tcflush(tty_fd, TCIFLUSH);
	tcsetattr(tty_fd, TCSANOW, &tio);

	return tty_fd;
}

/**
 ***************************************************************************
 * Open serial port
 * @return 0 on success
 * 		   3 on failure
 ***************************************************************************
 */
//int open_serialport(char commport[])
int open_serialport(const char *commport)
{
	char  comm[20];
	
	strcpy(comm,commport);
	cea2045_fd = initSerialPort(comm);	//open the device(com port) to be non-blocking
	if (cea2045_fd < 0) {
		logger("Error opening Comm Port!!");
		perror(commport);
		return 3;			// Exit on failure
	}
	else {
		logger("Comm Port Opened!!");
		return 0;
	}

}

/***********************************************************
* Check UCM type, if gpio26 is 0, then AC UCM else DC UCM
***********************************************************/

int check_ucmtype()
{
	FILE *ucmptr;
	char ch;
	ucmptr = fopen("/sys/class/gpio/gpio26/value", "r");
	if (ucmptr == NULL)
	{
		return 2;

	}
	ch = fgetc(ucmptr);
	fclose(ucmptr);
	if(ch == '0')
	{     
		logger("UCM is in AC mode");		
		ucmtype = 0; // AC UCM
		return 0;
	}
	else
	{
		logger("UCM is in DC mode");
		ucmtype = 1; // DC UCM
		return 1;
	}
}

/**
 ***************************************************************************
 * close serial port
 ***************************************************************************
 */
int close_serialport()
{
	if(close(cea2045_fd) == 0) {
		logger("Comm Port closed!!");
		return 0;
	}
	else {
		logger("Error closing Comm Port!!");
		return 1;
	}
}

/**
 ***************************************************************************
 * Function to log info to logfile
 * @param log message
 ***************************************************************************
 */
 void logger(const char *message)
 {
	pthread_mutex_lock(&logfilemutex);
	fp = fopen(CEA2045logFile, "a+");
	time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	fprintf(fp, "[%d-%d-%d/%d:%d:%d] ", (now->tm_mday), (now->tm_mon + 1), (now->tm_year + 1900), (now->tm_hour), (now->tm_min), (now->tm_sec));
//	fprintf(fp, "%s ", asctime(now));
	fprintf(fp, "%s\n", message);
	fclose(fp);
	pthread_mutex_unlock(&logfilemutex);
 }
 
