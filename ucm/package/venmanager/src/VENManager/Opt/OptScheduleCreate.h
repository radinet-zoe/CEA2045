/*
 * OptScheduleCreate.h
 *
 *  Created on: 23-Oct-2014
 *      Author: dinesh
 */

#ifndef OPTSCHEDULECREATE_H_
#define OPTSCHEDULECREATE_H_

#include "../Main.h"

class OptScheduleCreate
{
private:
	string optIDVal;
public:
	OptScheduleCreate();

	bool OptSchCreate(VEN2b *venCreateOpt, string startTime, string stopTime, string reason, string resource = "");

	const string& getOptIdVal() const {
		return optIDVal;
	}

	void setOptIdVal(const string& optIdVal) {
		optIDVal = optIdVal;
	}
};



#endif /* OPTSCHEDULECREATE_H_ */
