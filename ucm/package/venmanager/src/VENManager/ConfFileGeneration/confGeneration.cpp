/*
 * confGeneration.cpp
 *
 *  Created on: 30-Oct-2014
 *      Author: dinesh
 */


#include "confGeneration.h"

#define signalConfParamSize 6
#define reportConfUsageParamSize 8
#define reportConfStatusParamSize 9

//parameters seperated by colon
string signalConfParam = "SignalName:SignalType:Units:ValueType:ValueasRange:Values";

//values seperated by comma
string signalConfValue = "SIMPLE,level,None,integer,yes,0:3," \
						 "ELECTRICITY_PRICE,priceMultiplier,None,any,yes,any," \
						 "ELECTRICITY_PRICE,price,currencyPerKWh,any,yes,any," \
						 "LOAD_DISPATCH,level,powerXXX,float,yes,-10:10," \
						 "LOAD_CONTROL,x-LoadControlOffset,None,integer,yes,any," \
						 "LOAD_CONTROL,x-LoadControlSetpoint,None,any,yes,any";

//parameters seperated by colon
string reportConfUsageParam = "RSID:reportName:rID:reportType:readingType:Unit:itemUnits:oadrReportPayload";

/*NOTE: Values should be put in order of RSID.
 * i.e The same RSID's must be placed together
 */
//values seperated by commas
string reportConfUsageValue = "12345,METADATA_TELEMETRY_USAGE,123,usage,Direct_Read,power:EnergyReal:RealEnergy,Wh,payloadFloat:value";


//parameters seperated by colon
string reportConfStatusParam = "RSID:reportName:rID:reportType:readingType:oadrReportPayload:oadrReportPayload:oadrReportPayload:oadrReportPayload";

/*NOTE: Values should be put in order of RSID.
 * i.e The same RSID's must be placed together
 */
//values seperated by commas
string reportConfStatusValue = "22345,METADATA_TELEMETRY_STATUS,223,xresourceStatus,xnotApplicable,oadrPayloadResourceStatus:oadrOnline,oadrPayloadResourceStatus:oadrManualOverride,oadrPayloadResourceStatus:oadrCapacity:oadrMax,oadrPayloadResourceStatus:oadrCapacity:oadrCurrent";




confGeneration::confGeneration()
{

}

/**
 * Method to genetare reports.conf
 */
void confGeneration::generateReportConf()
{
	ofstream myfile;
	ofstream myfiledplog;
	stringstream contentParamU;
	stringstream contentvalueU;
	stringstream contentParamS;
	stringstream contentvalueS;
	string singlecontentParam;
	string singlecontentValue;
	string RSID,rID;
	int i=0;
	bool sameRSID = false;
	unsigned int dpval = 1;

	myfile.open (reportConffile, ios::out);
	myfiledplog.open (dpconffile, ios::out);

	contentParamU<<reportConfUsageParam;
	contentvalueU<<reportConfUsageValue;
	contentParamS<<reportConfStatusParam;
	contentvalueS<<reportConfStatusValue;


	while(getline(contentvalueU, singlecontentValue, ','))
	{
		if(i==0)
		{
			getline(contentParamU, singlecontentParam, ':');
			if(RSID.compare(singlecontentValue)!=0)
			{
				RSID = singlecontentValue;
				myfile << "[RS_" << RSID << "]" << '\n';
				sameRSID = false;
			}
		}
		else if(i==1)
		{
			getline(contentParamU, singlecontentParam, ':');
			if(!sameRSID)
			{
				myfile << singlecontentParam<<"="<< singlecontentValue<< '\n';
				sameRSID = true;
			}
		}
		else if(i==2)
		{
			getline(contentParamU, singlecontentParam, ':');
			myfile << "RS_"<<RSID<<"_"<<singlecontentParam<<"="<< singlecontentValue<< '\n';
			rID= singlecontentValue;
		}
		else
		{
			getline(contentParamU, singlecontentParam, ':');
			myfile << singlecontentParam<<"="<< singlecontentValue<< '\n';
		}
		i++;
		if(singlecontentParam.compare("oadrReportPayload")==0)
		{
			myfiledplog << "rID"<<rID<<"_"<<singlecontentValue<<"="<<dpval<<'\n';
			dpval++;
		}
		if(i==reportConfUsageParamSize)
		{
			i=0;
			contentParamU.clear();
			contentParamU<<reportConfUsageParam;
		}
	}
	myfile << '\n' <<'\n';

	i=0;

	while(getline(contentvalueS, singlecontentValue, ','))
	{
		if(i==0)
		{
			getline(contentParamS, singlecontentParam, ':');
			if(RSID.compare(singlecontentValue)!=0)
			{
				RSID = singlecontentValue;
				myfile << "[RS_" << RSID << "]" << '\n';
				sameRSID = false;
			}
		}
		else if(i==1)
		{
			getline(contentParamS, singlecontentParam, ':');
			if(!sameRSID)
			{
				myfile << singlecontentParam<<"="<< singlecontentValue<< '\n';
				sameRSID = true;
			}
		}
		else if(i==2)
		{
			getline(contentParamS, singlecontentParam, ':');
			myfile << "RS_"<<RSID<<"_"<<singlecontentParam<<"="<< singlecontentValue<< '\n';
			rID= singlecontentValue;
		}
		else
		{
			getline(contentParamS, singlecontentParam, ':');
			myfile << singlecontentParam<<"="<< singlecontentValue<< '\n';
		}
		i++;
		if(singlecontentParam.compare("oadrReportPayload")==0)
		{
			myfiledplog << "rID"<<rID<<"_"<<singlecontentValue<<"="<<dpval<<'\n';
			dpval++;
		}
		if(i==reportConfStatusParamSize)
		{
			i=0;
			contentParamS.clear();
			contentParamS<<reportConfStatusParam;
		}
	}
}

/**
 * Method to genetare signals.conf
 */
void confGeneration::generateSignalConf()
{
	ofstream myfile;
	stringstream content;
	string singlecontent;
	int i=0;

	myfile.open (signalsConfFile, ios::out);
	content<<signalConfParam;

	while(getline(content, singlecontent, ':'))
	{
		if(i++==0)
			myfile << "[" << singlecontent << "]" << '\n';
		else
			myfile << singlecontent << " ="<< '\n';
	}
	myfile << '\n';
	i=0;

	content.clear();
	content<<signalConfValue;
	while(getline(content, singlecontent, ','))
	{
		if(i++==0)
				myfile << "[" << singlecontent << "]" << '\n';
		else
			myfile << singlecontent << '\n';
		if(i==6)
		{
			i=0;
			myfile << '\n';
		}
	}
}
