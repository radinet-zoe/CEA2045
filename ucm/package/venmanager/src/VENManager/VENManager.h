/*
 * VENManager.h
 *
 *  Created on: 08-Dec-2014
 *      Author: dinesh
 */

#ifndef VENMANAGER_H_
#define VENMANAGER_H_

#include "Main.h"

class sqliteDB;
class OptScheduleCreate;

class VENManager
{
private:
	string m_baseURL;
	string m_venName;
	string m_venID;
	string m_registrationID;
	string m_vtnID;

public:

	VENManager(string m_baseURL="",
			   string m_venName="",
			   string m_venID = "",
			   string m_registrationID="",
			   string m_vtnID="");

	virtual ~VENManager();

	void updateParameter();

	void startDataLogThread(VEN2b *venObject, int restart);

	void registerVEN(VEN2b *ven);

	int getThreadStatus(string threadName);

	void waitNotifier(string notifierString);

	void sqliteInitialise(void);

	bool createFolderStructure(void);

	void createFolder(string path);

	void changePermission(string path);

	void removeFolder(string path);

	bool ifregistered();

	bool ifurlandNameExist();

	bool sendCreatedEvent(VEN2b *venObject, EventResponses er);

	bool updateEventsOptout(VEN2b *venObject);

	bool updateEventsOptin(VEN2b *venObject, string optID);

	void Login(void);
	void StartPolling(VEN2b *venObject);
	void StopPolling(void);
	void RegisterVEN(VEN2b *venObject);
	void QueryRegistration(VEN2b *venObject);
	void CancelRegistration(VEN2b *venObject);
	void ClearRegistration(void);
	void ReRegistration(VEN2b *venObject);
	void OptEvent(VEN2b *venObject);
	void CreateOpt(VEN2b *venObject);
	void CancelOpt(VEN2b *venObject);
	void StartLogThread(VEN2b *venObject);
	void StopLogThread(void);
	void OpenComPort(void);
	void CloseComPort(void);
	void QueryOperation(void);
	void SendOutsideComm(void);
	void ShedEvent(void);
	void EndShed(void);
	void LoadUp(void);
	void CriticalPeakEvent(void);
	void GridEmergency(void);
	void GridGuidanceGoodTime(void);
	void GridGuidanceBadTime(void);
	void GridGuidanceNeutral(void);
	void SetPrice(void);

	const string& getBaseUrl() const {
		return m_baseURL;
	}

	void setBaseUrl(const string& baseUrl) {
		m_baseURL = baseUrl;
	}

	const string& getRegistrationId() const {
		return m_registrationID;
	}

	void setRegistrationId(const string& registrationId) {
		m_registrationID = registrationId;
	}

	const string& getVenId() const {
		return m_venID;
	}

	void setVenId(const string& venId) {
		m_venID = venId;
	}

	const string& getVenName() const {
		return m_venName;
	}

	void setVenName(const string& venName) {
		m_venName = venName;
	}

	const string& getVtnId() const {
		return m_vtnID;
	}

	void setVtnId(const string& vtnId) {
		m_vtnID = vtnId;
	}
};



#endif /* VENMANAGER_H_ */
