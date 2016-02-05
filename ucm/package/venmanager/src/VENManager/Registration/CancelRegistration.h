/*
 * CancelRegistration.h
 *
 *  Created on: 26-Aug-2014
 *      Author: dinesh
 */

#ifndef CANCELREGISTRATION_H_
#define CANCELREGISTRATION_H_

#include "../Main.h"
#include <oadr/ven/VEN2b.h>

using namespace std;

class cancelParty
{
public:
	cancelParty();
	bool CancelParty(VEN2b *ven);

};


#endif /* CANCELREGISTRATION_H_ */
