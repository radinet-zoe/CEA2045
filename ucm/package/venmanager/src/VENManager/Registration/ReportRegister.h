/*
 * ReportRegister.h
 *
 *  Created on: 22-Sep-2014
 *      Author: dinesh
 */

#ifndef REPORTREGISTER_H_
#define REPORTREGISTER_H_


#include "../Main.h"
#include <oadr/request/report/ReportWrapper.h>
#include <oadr/ven/VEN2b.h>
#include <oadr/ven/http/HttpCurl.h>

class reportRegister
{
private:
	string m_venID;

protected:
	map <string, ReportEnumeratedType>::iterator ReportTypeItr;
	map <string, ReadingTypeEnumeratedType>::iterator ReadtypeItr;
	map <string, ReportWrapper::eEnergyItemType>::iterator eEnergyItr;
	map <string, ReportWrapper::ePowerItemType>::iterator ePowerItr;

	map<string, string>::iterator stringIterator;
	map<string,vector<string>>::const_iterator VstringIterator;
	vector<string>::const_iterator vsitr;

public:
	reportRegister(string m_venID = "");

	bool ReportRegister(VEN2b *venRegisterReport);

	string getMarketContext();

	const string& getVenId() const {
		return m_venID;
	}
};


#endif /* REPORTREGISTER_H_ */
