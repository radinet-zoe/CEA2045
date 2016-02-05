/*
 * OptScheduleCancel.h
 *
 *  Created on: 23-Oct-2014
 *      Author: dinesh
 */

#ifndef OPTSCHEDULECANCEL_H_
#define OPTSCHEDULECANCEL_H_

#include "../Main.h"

class OptScheduleCancel
{
public:

	OptScheduleCancel();

	bool OptSchCancel(VEN2b *venCancelOpt, string optID);
};



#endif /* OPTSCHEDULECANCEL_H_ */
