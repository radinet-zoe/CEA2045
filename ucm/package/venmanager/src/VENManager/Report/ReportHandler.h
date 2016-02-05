/*
 * ReportHandler.h
 *
 *  Created on: 19-Sep-2014
 *      Author: dinesh
 */

#ifndef REPORTHANDLER_H_
#define REPORTHANDLER_H_

#include "../Main.h"
#include <oadr/request/report/ReportWrapper.h>
#include <oadr/ven/VEN2b.h>
#include <oadr/ven/http/HttpCurl.h>

class ReportHandler
{
private:
	string m_venID;
	int responseCode;
	string responseDescription;

public:

	//maps for config file parameters..
	map<string, vector<string>> rsid_rid_Map;	// --> map(rsid, rids>
	map<string, vector<string>> ridPayloadMap;	// --> map(rid, payloads>
	map<string, string> rsid_ReportNameMap;		// --> map(rsid, report name>
	map<string, string> rid_ReadingTypeMap;		//--> map(rid, reading type>
	map<string, string> rid_ReportTypeMap;		// --> map(rid, report type>
	map<string, string> rid_UnitMap;			//--> map(rid, unit>
	map<string, string> rid_ItemUnitsMap;		//--> map(rid, item units>

	//maps from enumdatatypes..
	map <string, ReportEnumeratedType> ReporttypeEnum;
	map <string, ReadingTypeEnumeratedType> ReadingtypeEnum;
	map <string, ReportWrapper::eEnergyItemType> eUnit;
	map <string, ReportWrapper::ePowerItemType> pUnit;

	//maps for reportdata file..
	map<string, string> rrid_rsidMap;
	map<string, string> rrid_reportTypeMap;
	map<string, string> rrid_granMap;
	map<string, string> rrid_rbdurationMap;
	map<string, string> rrid_durMap;
	map<string, time_t> rrid_startMap;
	map<string, unsigned int> rrid_cvMap;

	//map for logdata value
	map<string, float> rid_PayloadvalMap;
	map<string, bool> rid_onlineMap;
	map<string, bool> rid_manualMap;
	map<string, float> rid_CapMaxMap;
	map<string, float> rid_CapCurrentMap;

	//Iterator
	map<string, string>::iterator stringIterator;
	map<string,time_t>::iterator timeIterator;
	map<string, unsigned int>::iterator intIterator;
	map<string, bool>::iterator boolIterator;
	map<string, float>::iterator floatIterator;

	map<string,vector<string>>::const_iterator VstringIterator;
	vector<string>::const_iterator vsitr;

	map <string, ReportEnumeratedType>::iterator ReportTypeItr;
	map <string, ReadingTypeEnumeratedType>::iterator ReadtypeItr;
	map <string, ReportWrapper::eEnergyItemType>::iterator eEnergyItr;
	map <string, ReportWrapper::ePowerItemType>::iterator ePowerItr;

	ReportHandler(string m_venID);

	void reportWaitNotifier(int cvIndex, condition_variable cvRpt[], char *notifierP);

	string logfileExtractValue (string RID, string parameter);

	void extractPayloadValUsage (string rID, float *Payload);

	void extractPayloadValStatus (string rID, float *CapMax, float *CapCurrent, bool *online, bool *manual);

	void updateReportUsage (VEN2b *venReport, ReportWrapper *telemetryUsage, time_t Reportdtstart,string RRID,time_t createdDateTime);

	void updateReportStatus (VEN2b *venReport, ReportWrapper *telemetryStatus, time_t Reportdtstart,string RRID,time_t createdDateTime);

	void getReportconf(void);

	void getReportdata(void);

	void extractEnumValues(void);

	void insertDB(string RRID, string RSID, string reportType, string granularity,
			      string reportBackduration,  time_t startTime, string duration,
				  unsigned int cvIndex);

	void clearDB(void);

	unsigned int  durationExtract (string duration);

	bool reportDateTimeCheck(time_t current, time_t report);

	bool updateDataFile(string reportSpecifierID);

	time_t getReportTime(oadrCreateReportType::oadrReportRequest_iterator RptItr);

	void reportThread(VEN2b *venReport, oadrCreateReportType::oadrReportRequest_iterator RptItr, condition_variable *cv,char *notifierP);

	void metadataThread(VEN2b *venReport, oadrCreateReportType::oadrReportRequest_iterator RptItr, condition_variable cvRpt[], condition_variable *cv,char *notifierP);

	int ValidateReport(oadrCreateReportType::oadrReportRequest_iterator RptItr);

	bool reportScheduler(VEN2b *venReport, oadrCreateReportType CreateReportPayload, condition_variable cvRpt[], char *notifierP);

	bool cancelReport(VEN2b *venCancelReport, oadrCancelReportType CancelRptPayload,condition_variable cvRpt[], char *notifierP);

	const string& getVenId() const {
		return m_venID;
	}

	void setVenId(const string& venId) {
		m_venID = venId;
	}

	int getResponseCode() const {
		return responseCode;
	}

	void setResponseCode(int responseCode) {
		this->responseCode = responseCode;
	}

	const string& getResponseDescription() const {
		return responseDescription;
	}

	void setResponseDescription(const string& responseDescription) {
		this->responseDescription = responseDescription;
	}

	const map<string, string>& getRidItemUnitsMap() const {
		return rid_ItemUnitsMap;
	}

	void setRidItemUnitsMap(const map<string, string>& ridItemUnitsMap) {
		rid_ItemUnitsMap = ridItemUnitsMap;
	}

	const map<string, string>& getRidReadingTypeMap() const {
		return rid_ReadingTypeMap;
	}

	void setRidReadingTypeMap(const map<string, string>& ridReadingTypeMap) {
		rid_ReadingTypeMap = ridReadingTypeMap;
	}

	const map<string, string>& getRidReportTypeMap() const {
		return rid_ReportTypeMap;
	}

	void setRidReportTypeMap(const map<string, string>& ridReportTypeMap) {
		rid_ReportTypeMap = ridReportTypeMap;
	}

	const map<string, string>& getRidUnitMap() const {
		return rid_UnitMap;
	}

	void setRidUnitMap(const map<string, string>& ridUnitMap) {
		rid_UnitMap = ridUnitMap;
	}

	const map<string, vector<string> >& getRidPayloadMap() const {
		return ridPayloadMap;
	}

	void setRidPayloadMap(const map<string, vector<string> >& ridPayloadMap) {
		this->ridPayloadMap = ridPayloadMap;
	}

	const map<string, string>& getRsidReportNameMap() const {
		return rsid_ReportNameMap;
	}

	void setRsidReportNameMap(const map<string, string>& rsidReportNameMap) {
		rsid_ReportNameMap = rsidReportNameMap;
	}

	const map<string, vector<string> >& getRsidRidMap() const {
		return rsid_rid_Map;
	}

	void setRsidRidMap(const map<string, vector<string> >& rsidRidMap) {
		rsid_rid_Map = rsidRidMap;
	}
};



#endif /* REPORTHANDLER_H_ */
